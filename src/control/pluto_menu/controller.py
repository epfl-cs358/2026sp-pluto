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

    # Track states for movement and buttons to prevent spamming
    last_sent_command = {"direction": None}
    previous_buttons = []

    def handle_key(e: events.KeyEventArguments):
        key = e.key.name.lower()
        if key in movement_state:
            movement_state[key] = e.action.keydown
            input_manager.update_from_keyboard(**movement_state)

    async def poll_gamepad():
        nonlocal previous_buttons

        js_code = """
            (() => {
                const gp = navigator.getGamepads()[0];
                if (!gp) return null;
                return gp.buttons.map(b => b.pressed);
            })()
        """
        try:
            data = await ui.run_javascript(js_code, timeout=0.5)
            if data:
                current_buttons = data

                if not previous_buttons:
                    previous_buttons = current_buttons
                    return

                # D-pad (buttons 12-15) drives movement
                input_manager.update_from_keyboard(
                    w=current_buttons[15],
                    s=current_buttons[13],
                    a=current_buttons[14],
                    d=current_buttons[12],
                )

                # Either trigger (buttons 6-7) sends stop on press
                trigger_pressed = current_buttons[6] or current_buttons[7]
                prev_trigger_pressed = previous_buttons[6] or previous_buttons[7]
                if trigger_pressed and not prev_trigger_pressed:
                    trigger_stop()

                # Edge detection: trigger only when transitioning from False to True
                if current_buttons[0] and not previous_buttons[0]:
                    trigger_behavior(message.MessageBehaviorKind.BEHAVIOR_SIT)

                if current_buttons[1] and not previous_buttons[1]:
                    trigger_behavior(message.MessageBehaviorKind.BEHAVIOR_FLIP)

                if current_buttons[2] and not previous_buttons[2]:
                    trigger_behavior(message.MessageBehaviorKind.BEHAVIOR_BOW)

                if current_buttons[3] and not previous_buttons[3]:
                    trigger_behavior(message.MessageBehaviorKind.BEHAVIOR_GIVE_PAW)

                previous_buttons = current_buttons
        except Exception:
            pass

    async def connect_to_robot():
        ui.notify("Scanning network for Pluto...", type="info")

        # disable button so user can't spam it while scanning
        connect_button.disable()
        connect_button.text = "Scanning..."

        # run the blocking scan in a background thread
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

    def trigger_behavior(behavior_kind: message.MessageBehaviorKind, notify: bool = True):
        if pluto_controller.is_connected:
            msg = message.create_behavior(behavior_kind)
            pluto_controller.send_messages([msg])
            if notify:
                ui.notify(f"Sent: {behavior_kind.name}")

    def trigger_stop(notify: bool = True):
        if pluto_controller.is_connected:
            msg = message.Message(
                message.MessageFamilyKind.KIND_MOVE,
                message.MessageMoveKind.MOVE_STOP_FOR,
                0,
            )
            pluto_controller.send_messages([msg])
            if notify:
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
                ui.button(
                    "Give Paw",
                    on_click=lambda: trigger_behavior(
                        message.MessageBehaviorKind.BEHAVIOR_GIVE_PAW
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

            DEADZONE = 0.25

            if abs(vx) < DEADZONE:
                vx = 0.0

            if abs(vy) < DEADZONE:
                vy = 0.0

            forward_back = 0
            left_right = 0
            direction = "stop"

            if vx == 0.0 and vy == 0.0:
                direction = "stop"
            elif abs(vy) >= abs(vx):
                if vy < 0:
                    direction = "forward"
                    forward_back = MAX_SPEED
                else:
                    direction = "backward"
                    forward_back = -MAX_SPEED
            else:
                if vx < 0:
                    direction = "left"
                    left_right = -MAX_SPEED
                else:
                    direction = "right"
                    left_right = MAX_SPEED

            if pluto_controller.is_connected:
                direction_changed = direction != last_sent_command["direction"]
                if direction_changed:
                    last_sent_command["direction"] = direction  # type: ignore

                    if direction == "stop":
                        trigger_stop(False)
                    else:
                        msg = message.create_move_by(
                            forward_back, left_right, duration_ms=0
                        )
                        pluto_controller.send_messages([msg])

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
