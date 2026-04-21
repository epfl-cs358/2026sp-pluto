from nicegui import ui


def navigation_bar():
    """Shared header for navigating between modes."""
    with ui.header().classes("items-center justify-between"):
        ui.label("Pluto Controller Platform").classes("text-lg font-bold")
        with ui.row():
            ui.link("Home", "/").classes("text-white")
            ui.link("Simulation", "/sim").classes("text-white")
            ui.link("Controller", "/controller").classes("text-white")
