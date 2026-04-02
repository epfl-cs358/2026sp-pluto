"""
Entry point for the controller UI.
"""

import logging
import argparse
from nicegui import ui, app

from sim_motion import PyBulletMotionController


logging.basicConfig(level=logging.INFO)
LOGGER = logging.getLogger(__name__)

SIM = PyBulletMotionController()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Robot Controller")
    parser.add_argument(
        "--port",
        type=int,
        default=8080,
        help="Port to run the web UI on (default: 8080)",
    )
    return parser.parse_args()


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


def run_forward() -> None:
    try:
        SIM.forward(duration=1.0, speed=1.0)
    except Exception:
        LOGGER.exception("forward command failed")
        ui.notify("Forward failed", color="negative")


def run_backward() -> None:
    try:
        SIM.backward(duration=1.0, speed=1.0)
    except Exception:
        LOGGER.exception("backward command failed")
        ui.notify("Backward failed", color="negative")


def run_turn_left() -> None:
    try:
        SIM.turn(angle=-45, speed=1.0)
    except Exception:
        LOGGER.exception("left turn command failed")
        ui.notify("Left turn failed", color="negative")


def run_turn_right() -> None:
    try:
        SIM.turn(angle=45, speed=1.0)
    except Exception:
        LOGGER.exception("right turn command failed")
        ui.notify("Right turn failed", color="negative")


@ui.page("/")
def index():
    with ui.column():
        ui.label("Pluto Controller").style("font-size: 1.1rem;")
        ui.separator()

        with ui.row():
            ui.button("Open PyBullet", on_click=start_sim)
            ui.button("Close PyBullet", on_click=stop_sim)

        ui.separator()
        with ui.row():
            ui.button("Forward 1s", on_click=run_forward)
            ui.button("Backward 1s", on_click=run_backward)
            ui.button("Turn Left 45", on_click=run_turn_left)
            ui.button("Turn Right 45", on_click=run_turn_right)

if __name__ in {"__main__", "__mp_main__"}:
    args = parse_args()
    app.on_shutdown(lambda: stop_sim(show_notice=False))
    ui.run(title="Robot Controller", port=args.port, reload=True)
