import argparse
import logging
from nicegui import app, ui

# Importing the modules registers their @ui.page routes
from pluto_menu import simulation
from pluto_menu import controller
from pluto_menu import navigation_bar
from pluto_server.server import PlutoController
from pluto_speech.speech import start_speech_engine

import threading

logging.basicConfig(level=logging.INFO)


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
    navigation_bar()

    with ui.column().classes("items-center justify-center w-full q-mt-xl"):
        ui.label("Pluto Control Hub").style("font-size: 2.5rem; font-weight: bold;")
        ui.label("Select an operating mode below to continue.").classes("text-lg q-mb-lg")

        with ui.row().classes("q-gutter-xl"):
            with ui.card().classes("items-center p-6 cursor-pointer").on(
                "click", lambda: ui.navigate.to("/sim")
            ):
                ui.icon("smart_toy", size="4rem").classes("text-blue-500")
                ui.label("Simulation Mode").classes("text-xl font-bold q-mt-md")

            with ui.card().classes("items-center p-6 cursor-pointer").on(
                "click", lambda: ui.navigate.to("/controller")
            ):
                ui.icon("gamepad", size="4rem").classes("text-green-500")
                ui.label("Controller").classes("text-xl font-bold q-mt-md")


if __name__ in {"__main__", "__mp_main__"}:
    args = parse_args()

    IP_OF_ESP = ""
    CONTROLLER = PlutoController(IP_OF_ESP)
    app.extra["PLUTO_CONTROLLER"] = CONTROLLER
    app.on_startup(
        lambda: threading.Thread(
            target=start_speech_engine, args=(CONTROLLER,), daemon=True
        ).start()
    )
    app.on_shutdown(lambda: simulation.stop_sim(show_notice=False))
    ui.run(title="Pluto Controller", port=args.port, reload=True)
