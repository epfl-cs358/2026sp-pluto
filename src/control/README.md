# 🎛️ Python Controller

This directory contains Pluto's computer-side control stack.

## 🚪 Entry Point

```text
src/control/main.py
```

The app uses NiceGUI and registers pages from `pluto_menu`.

Current routes:

- `/`: home page.
- `/sim`: simulation controls.
- `/controller`: robot command center.

## 🧩 Main Modules

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

## 🎛️ Controller Execution Flow

The control application starts in `main.py`:

1. Command-line arguments are parsed, including `--port`.
2. NiceGUI route modules are imported, which registers their pages.
3. `IP_OF_ESP` is used to construct `PlutoController`.
4. The controller object is stored in `app.extra`.
5. A speech-recognition worker starts in a background thread.
6. The NiceGUI server starts.

The `/controller` page then:

1. Creates an `InputManager`.
2. Tracks WASD key state.
3. Polls the first browser gamepad.
4. Combines keyboard and gamepad input into a normalized vector.
5. Converts that vector into signed 16-bit movement values scaled by `MAX_SPEED = 255`.
6. Sends `MOVE_BY` messages while the vector is non-zero.
7. Sends behavior and stop messages from the quick-action buttons.
8. Reads acknowledgement and sensor messages into the telemetry log.

## ▶️ Running

Use the project-level scripts:

```bash
bash run.sh
```

or on Windows:

```cmd
run.bat
```

By default the scripts launch the UI on port `8090`.

## 📶 Robot Connection

The ESP32 IP address is currently set in `main.py`:

```python
IP_OF_ESP = ""
CONTROLLER = PlutoController(IP_OF_ESP)
```

Set this to the ESP32 address before using live WiFi control.

WiFi must also be enabled in the ESP32 firmware with `PLUTO_ENABLE_WIFI`.

## 👥 New-Team Setup Checklist

1. Run `bash run.sh` or `run.bat`.
2. Confirm the home page opens.
3. Open `/controller`.
4. Press WASD and confirm the displayed vector changes.
5. Connect a gamepad and confirm the vector changes.
6. Set `IP_OF_ESP` in `main.py`.
7. Enable WiFi in the ESP32 firmware.
8. Flash firmware and confirm the ESP32 is on the same network.
9. Press `Connect & Take Control`.
10. Send `Stop All` before sending movement.
11. Watch the telemetry log for `INFO_ACK` messages.

## 🎛️ Controller Page

`pluto_menu/controller.py` supports:

- Connect and take-control button.
- WASD keyboard input.
- First browser gamepad input.
- Repeated `MOVE_BY` messages while the movement vector is non-zero.
- Quick action buttons for sit, give paw, and stop.
- Telemetry log for acknowledgements and distance messages.

## 🗣️ Speech Commands

Speech recognition runs on the controller computer with Vosk and `sounddevice`.

Current grammar:

- `pluto sit`
- `pluto stop`
- `pluto give paw`

Recognized phrases are converted into shared protocol messages.

## ➕ Adding A New UI Command

To add a new button or command:

1. Define the message in `src/control/pluto_server/message.py`.
2. Make sure the same message exists in `src/comm/message.h`.
3. Add a button or input handler in `pluto_menu/controller.py`.
4. Send the message through `pluto_controller.send_messages(...)`.
5. Add firmware handling in `src/esp/main.cpp`.
6. Test that the UI sends the packet before connecting servos.

## 🎮 Simulation Page

The `/sim` page opens the Python PyBullet path. It is useful for UI-level simulation testing, but the visual mesh references still need to be updated to match the current per-leg STL files.

Camera controls in PyBullet:

- Rotate: hold Alt/Option and left-click drag.
- Zoom: scroll wheel.
- Pan: hold middle-click drag.

See [Simulation Notes](../../SIMULATION.md).
