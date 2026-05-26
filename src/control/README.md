# Python Controller

This directory contains Pluto's computer-side control stack.

## Entry Point

```text
src/control/main.py
```

The app uses NiceGUI and registers pages from `pluto_menu`.

Current routes:

- `/`: home page.
- `/sim`: simulation controls.
- `/controller`: robot command center.

## Main Modules

| Module | Purpose |
| --- | --- |
| `main.py` | Starts the NiceGUI app, creates `PlutoController`, and starts the speech worker |
| `pluto_menu/controller.py` | Controller UI, connect button, quick actions, movement input, and telemetry log |
| `pluto_menu/simulation.py` | PyBullet simulation UI controls |
| `pluto_input/input_manager.py` | Normalizes WASD and gamepad input into movement vectors |
| `pluto_server/message.py` | Python mirror of the shared packet/message format |
| `pluto_server/server.py` | UDP controller client, handshake, listener, and heartbeat |
| `pluto_speech/speech.py` | Vosk speech recognition worker |
| `gait.py`, `robot_config.py`, `sim_motion.py` | Python simulation and gait support |

## Running

Use the project-level scripts:

```bash
bash run.sh
```

or on Windows:

```cmd
run.bat
```

By default the scripts launch the UI on port `8090`.

## Robot Connection

The ESP32 IP address is currently set in `main.py`:

```python
IP_OF_ESP = ""
CONTROLLER = PlutoController(IP_OF_ESP)
```

Set this to the ESP32 address before using live WiFi control.

WiFi must also be enabled in the ESP32 firmware with `PLUTO_ENABLE_WIFI`.

## Controller Page

`pluto_menu/controller.py` supports:

- Connect and take-control button.
- WASD keyboard input.
- First browser gamepad input.
- Repeated `MOVE_BY` messages while the movement vector is non-zero.
- Quick action buttons for sit, give paw, and stop.
- Telemetry log for acknowledgements and distance messages.

## Speech Commands

Speech recognition runs on the controller computer with Vosk and `sounddevice`.

Current grammar:

- `pluto sit`
- `pluto stop`
- `pluto give paw`

Recognized phrases are converted into shared protocol messages.

## Simulation Page

The `/sim` page opens the Python PyBullet path. It is useful for UI-level simulation testing, but the visual mesh references still need to be updated to match the current per-leg STL files.

Camera controls in PyBullet:

- Rotate: hold Alt/Option and left-click drag.
- Zoom: scroll wheel.
- Pan: hold middle-click drag.

See [Simulation Notes](../../SIMULATION.md).
