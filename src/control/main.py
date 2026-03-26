"""
Entry point for the controller UI.
"""

from nicegui import ui
import argparse


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Robot Controller")
    parser.add_argument(
        "--port",
        type=int,
        default=8080,
        help="Port to run the web UI on (default: 8080)",
    )
    return parser.parse_args()


@ui.page("/")
def index():
    with ui.column():
        ui.label("Hello, World!").style(
            "font-size: 1.2rem; color: grey; font-family: monospace"
        )
        ui.separator()
        ui.label("NiceGUI is running correctly.")


if __name__ in {"__main__", "__mp_main__"}:
    args = parse_args()
    ui.run(title="Robot Controller", port=args.port, reload=False)
