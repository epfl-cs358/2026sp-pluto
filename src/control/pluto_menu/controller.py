import struct
from nicegui import ui, events, app
from pluto_menu import navigation_bar
from pluto_input.input_manager import InputManager
from pluto_server import message, server
import asyncio

MAX_SPEED = 255


@ui.page("/controller")
def controller_page():
    navigation_bar()

    input_manager = InputManager()
    movement_state = {"w": False, "a": False, "s": False, "d": False}

    pluto_controller: server.PlutoController = app.extra["PLUTO_CONTROLLER"]

    def handle_key(e: events.KeyEventArguments):
        key = e.key.name.lower()
        if key in movement_state:
            movement_state[key] = e.action.keydown
            input_manager.update_from_keyboard(**movement_state)

    async def poll_gamepad():
        js_code = """
            (() => {
                const gp = navigator.getGamepads()[0];
                if (!gp) return null;
                return { axes: gp.axes };
            })()
        """
        try:
            data = await ui.run_javascript(js_code, timeout=0.5)
            if data:
                input_manager.update_from_gamepad(data["axes"][0], data["axes"][1])
        except Exception:
            pass

    async def connect_to_robot():
        ui.notify("Scanning network for Pluto...", type="info")

        # Disable button so user can't spam it while scanning
        connect_button.disable()
        connect_button.text = "Scanning..."

        # Run the blocking scan in a background thread
        found = await asyncio.to_thread(pluto_controller.scan_for_robot, 3.0)

        if not found:
            ui.notify("Could not find Pluto. Is it powered on?", type="negative")
            connect_button.enable()
            connect_button.text = "Connect & Take Control"
            return

        if pluto_controller.connect():
            assert pluto_controller.target_addr is not None
            ui.notify(
                f"Connected via {pluto_controller.target_addr[0]}!", type="positive"
            )
            # Update the UI to show success
            connect_button.text = "Connected"
            connect_button.classes(replace="bg-green-600 text-white")
            connection_status.set_text(f"IP: {pluto_controller.target_addr[0]}")
            connection_status.classes(replace="text-green-500 font-bold text-lg")

            msg = message.create_control_begin(10000)
            pluto_controller.send_messages([msg])
        else:
            ui.notify("Found Pluto, but handshake failed.", type="negative")
            connect_button.enable()
            connect_button.text = "Connect & Take Control"

    def trigger_behavior(behavior_kind: message.MessageBehaviorKind):
        if pluto_controller.is_connected:
            msg = message.create_behavior(behavior_kind)
            pluto_controller.send_messages([msg])
            ui.notify(f"Sent: {behavior_kind.name}")

    def trigger_stop():
        if pluto_controller.is_connected:
            msg = message.Message(
                message.MessageFamilyKind.KIND_MOVE,
                message.MessageMoveKind.MOVE_STOP_FOR,
                0,
            )
            pluto_controller.send_messages([msg])
            ui.notify("Sent: STOP", type="warning")

    with ui.column().classes("items-center q-gutter-md").style(
        "padding: 24px; margin: 0 auto; max-width: 800px;"
    ):
        ui.label("Pluto Command Center").style("font-size: 1.6rem; font-weight: bold;")

        with ui.row().classes("items-center q-gutter-md q-mb-md"):
            connect_button = ui.button(
                "Connect & Take Control", on_click=connect_to_robot, icon="search"
            ).classes("bg-blue-600 text-white")
            connection_status = ui.label("Disconnected").classes(
                "text-red-500 font-bold text-lg"
            )

        with ui.card().classes("w-full items-center p-4"):
            ui.label("Quick Actions").classes("text-lg font-bold q-mb-sm")
            with ui.row().classes("q-gutter-md"):
                ui.button(
                    "Sit",
                    on_click=lambda: trigger_behavior(
                        message.MessageBehaviorKind.BEHAVIOR_SIT
                    ),
                ).classes("bg-indigo-500")
                ui.button(
                    "Flip",
                    on_click=lambda: trigger_behavior(
                        message.MessageBehaviorKind.BEHAVIOR_FLIP
                    ),
                ).classes("bg-indigo-500")
                ui.button(
                    "Bow",
                    on_click=lambda: trigger_behavior(
                        message.MessageBehaviorKind.BEHAVIOR_BOW
                    ),
                ).classes("bg-indigo-500")
                ui.button("Stop All", on_click=trigger_stop).classes(
                    "bg-red-600 text-white font-bold"
                )

        ui.separator().classes("w-full q-my-md")
        ui.label("Hardware Inputs Active (WASD or Gamepad)").classes(
            "text-green-600 font-bold"
        )
        vector_label = ui.label("Vector: (0.00, 0.00)").classes("text-2xl font-mono")

        ui.separator().classes("w-full q-my-md")
        ui.label("Live Telemetry Feed").classes("text-lg font-bold")
        telemetry_log = ui.log(max_lines=15).classes(
            "w-full h-48 bg-gray-900 text-green-400 font-mono text-sm p-3 rounded"
        )

        def update_robot_loop():
            vx, vy = input_manager.get_movement_vector()
            vector_label.set_text(f"Vector: ({vx:.2f}, {vy:.2f})")

            if pluto_controller.is_connected and (vx != 0.0 or vy != 0.0):
                forward_back = int(vy * MAX_SPEED)
                left_right = int(vx * MAX_SPEED)
                msg = message.create_move_by(forward_back, left_right, duration_ms=100)
                pluto_controller.send_messages([msg])

            if pluto_controller.is_connected:
                for msg in pluto_controller.get_latest_messages():
                    if msg.family == message.MessageFamilyKind.KIND_SENSOR:
                        if msg.kind == message.MessageSensorKind.SENSOR_DISTANCE:
                            dist = struct.unpack("<I", msg.payload_bytes)[0]
                            telemetry_log.push(
                                f"[{msg.event_clock_or_duration}ms] SENSOR_DISTANCE: {dist}mm"
                            )

                    elif msg.family == message.MessageFamilyKind.KIND_INFO:
                        if msg.kind == message.MessageInfoKind.INFO_ACKNOWLEDGE:
                            seq = struct.unpack("<I", msg.payload_bytes)[0]
                            telemetry_log.push(f"INFO_ACK: Seq {seq}")

    ui.keyboard(on_key=handle_key, repeating=False)
    ui.timer(0.05, poll_gamepad)
    ui.timer(0.05, update_robot_loop)
