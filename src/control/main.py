"""
Entry point for the controller UI.
"""

import argparse
import logging

from nicegui import app, ui
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


@ui.page("/")
def index():
    ui.add_head_html('''
        <style type="text/tailwindcss">
            @layer components {
                .btn {
                    @apply bg-white hover:bg-gray-50 active:bg-gray-100
                           text-gray-800 font-medium py-2 px-4
                           border border-gray-300 rounded-lg shadow-sm
                           transition-colors duration-100 select-none cursor-pointer;
                }
                .pad-btn {
                    @apply bg-white hover:bg-gray-50 active:bg-gray-100
                           text-gray-700 font-semibold
                           border border-gray-300 rounded-lg shadow-sm
                           transition-colors duration-100 select-none cursor-pointer
                           w-20 h-14 flex items-center justify-center;
                }
            }
        </style>
    ''')

    with ui.column().classes("items-center justify-center gap-12 min-h-screen bg-gray-50 px-4"):
        ui.label("pluto").classes("text-2xl font-bold text-gray-900 tracking-tight")

        with ui.row().classes("gap-2 items-center"):
            ui.button("open", on_click=start_sim).props("flat").classes("btn")
            ui.button("close", on_click=lambda: stop_sim()).props("flat").classes("btn")

            gait_btn = ui.dropdown_button("walk", auto_close=True).props("flat").classes("btn")
            with gait_btn:
                for gait in ["walk", "trot", "gallop"]:
                    ui.item(gait, on_click=lambda _, g=gait: (gait_btn.set_text(g), SIM.set_gait(g)))

        with ui.column().classes("items-center gap-1"):
            forward_btn = ui.button("↑").props("flat").classes("pad-btn")
            bind_press_events(forward_btn, "forward", 1.0)

            with ui.row().classes("gap-1"):
                left_btn = ui.button("←").props("flat").classes("pad-btn")
                bind_press_events(left_btn, "turn_left", 1.0)

                stop_btn = ui.button("■", on_click=release_motion).props("flat").classes("pad-btn")

                right_btn = ui.button("→").props("flat").classes("pad-btn")
                bind_press_events(right_btn, "turn_right", 1.0)

            backward_btn = ui.button("↓").props("flat").classes("pad-btn")
            bind_press_events(backward_btn, "backward", 1.0)


if __name__ in {"__main__", "__mp_main__"}:
    args = parse_args()
    app.on_shutdown(lambda: stop_sim(show_notice=False))
    ui.run(title="Robot Controller", port=args.port, reload=True)