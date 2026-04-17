import logging
from nicegui import ui
from pluto_menu import navigation_bar
from sim_motion import PyBulletMotionController

LOGGER = logging.getLogger(__name__)
SIM = PyBulletMotionController()


def start_sim() -> None:
    try:
        SIM.start()
        ui.notify("PyBullet window opened.", color="positive")
    except Exception as e:
        LOGGER.exception("start command failed")
        ui.notify(f"Open failed: {e}", color="negative")


def stop_sim(show_notice: bool = True) -> None:
    try:
        SIM.shutdown()
        if show_notice:
            ui.notify("PyBullet window closed.", color="warning")
    except Exception as e:
        LOGGER.exception("shutdown failed")
        if show_notice:
            ui.notify(f"Close failed: {e}", color="negative")


def set_motion(motion: str, speed: float = 1.0) -> None:
    try:
        SIM.set_motion(motion, speed)
    except Exception:
        LOGGER.exception("motion command failed")
        ui.notify(f"{motion} failed", color="negative")


def release_motion() -> None:
    try:
        SIM.stop()
    except Exception:
        LOGGER.exception("stop command failed")
        ui.notify("Stop failed", color="negative")


def bind_press_events(button: ui.button, motion: str, speed: float = 1.0) -> None:
    button.on("mousedown", lambda _: set_motion(motion, speed))
    button.on("mouseup", lambda _: release_motion())
    button.on("mouseleave", lambda _: release_motion())
    button.on("touchstart", lambda _: set_motion(motion, speed))
    button.on("touchend", lambda _: release_motion())
    button.on("touchcancel", lambda _: release_motion())


@ui.page("/sim")
def sim_page():
    navigation_bar()
    with ui.column().classes("items-center q-gutter-md").style(
        "padding: 24px; margin: 0 auto;"
    ):
        ui.label("Simulation Controller").style("font-size: 1.4rem; font-weight: 600;")
        ui.separator().classes("w-full")

        with ui.row().classes("q-gutter-sm"):
            ui.button("Open PyBullet", on_click=start_sim)
            ui.button("Close PyBullet", on_click=lambda: stop_sim())

        ui.separator().classes("w-full")
        ui.label("Hold a button to keep moving").style("font-size: 1rem;")

        forward_btn = ui.button("Forward").props("size=lg color=primary")
        bind_press_events(forward_btn, "forward", 1.0)

        with ui.row().classes("q-gutter-sm"):
            left_btn = ui.button("Left").props("size=lg color=secondary")
            bind_press_events(left_btn, "turn_left", 1.0)

            stop_btn = ui.button("Stop", on_click=release_motion).props(
                "size=lg color=negative"
            )

            right_btn = ui.button("Right").props("size=lg color=secondary")
            bind_press_events(right_btn, "turn_right", 1.0)

        backward_btn = ui.button("Backward").props("size=lg color=primary")
        bind_press_events(backward_btn, "backward", 1.0)
