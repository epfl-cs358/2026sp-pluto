from nicegui import ui, events
from pluto_menu import navigation_bar
from pluto_input.input_manager import InputManager


@ui.page("/controller")
def controller_page():
    navigation_bar()

    input_manager = InputManager()
    movement_state = {"w": False, "a": False, "s": False, "d": False}

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

    with ui.column().classes("items-center q-gutter-md").style(
        "padding: 24px; margin: 0 auto;"
    ):
        ui.label("Controller").style("font-size: 1.4rem; font-weight: 600;")
        ui.label("Hardware Inputs Active (WASD or Gamepad)").classes(
            "text-green-600 font-bold"
        )
        ui.separator().classes("w-full")

        vector_label = ui.label("Vector: (0.00, 0.00)").classes("text-2xl font-mono")

        def update_robot_loop():
            vx, vy = input_manager.get_movement_vector()
            vector_label.set_text(f"Vector: ({vx:.2f}, {vy:.2f})")

            # TODO: send (vx, vy) to pluto

    ui.keyboard(on_key=handle_key, repeating=False)
    ui.timer(0.05, poll_gamepad)
    ui.timer(0.05, update_robot_loop)
