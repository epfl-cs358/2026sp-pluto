# 💻 Software Overview

Pluto's software is split into four active areas: ESP32 firmware, Python controller, shared communication protocol, and simulation support.

## 🧰 How To Read The Code From Scratch

For a new team, read the code in this order:

| Step | File or folder | What to learn |
| --- | --- | --- |
| 1 | `platformio.ini` | How PlatformIO builds the ESP32 firmware from `src/esp` |
| 2 | `src/esp/legs/leg_data.h` | Servo calibration values and leg names |
| 3 | `src/esp/legs/leg.h` | Servo channel mapping and leg abstraction |
| 4 | `src/esp/legs/leg_joint.h` | How raw PWM and logical angles are written |
| 5 | `src/esp/motion/ik_solver.cpp` | How foot targets become joint angles |
| 6 | `src/esp/motion/gait.cpp` | How stand, walk, trot, gallop, bow, and paw are generated |
| 7 | `src/esp/main.cpp` | Firmware startup, feature flags, sensors, serial commands, and UDP dispatch |
| 8 | `src/comm/message.h` | Shared C++ packet and message format |
| 9 | `src/control/pluto_server/message.py` | Python mirror of the same packet and message format |
| 10 | `src/control/pluto_server/server.py` | Python UDP client, connection handshake, heartbeat, and listener |
| 11 | `src/control/main.py` | NiceGUI app entry point and controller setup |
| 12 | `src/control/pluto_menu/controller.py` | How keyboard/gamepad input becomes robot messages |
| 13 | `src/control/pluto_speech/speech.py` | How recognized speech becomes robot messages |
| 14 | `src/control/sim_motion.py` and `src/sim/sim_main.cpp` | Current Python and MuJoCo simulation paths |

Read this before changing behavior. Most software bugs in this project come from changing one side of a shared contract without updating the matching side.

## 📌 Main Components

| Component | Location | Role |
| --- | --- | --- |
| ESP32 firmware | [src/esp](src/esp/README.md) | Runs on the robot, controls servos, executes gaits, reads sensors, and optionally handles UDP packets |
| Python control hub | [src/control](src/control/README.md) | Provides the NiceGUI interface, PyBullet control page, keyboard/gamepad input, speech worker, and UDP client |
| Shared protocol | [src/comm](src/comm/README.md) | Defines the compact packet and message format shared by C++ firmware and Python control code |
| Simulation assets | [SIMULATION.md](SIMULATION.md) | Documents the Python PyBullet path, MuJoCo XML model, C++ bridge, and current limitations |
| WiFi protocol notes | [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md) | Documents UDP connection setup, packet structure, batching, CRCs, and payload layout |
| Sensor notes | [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) | Documents the ultrasonic distance sensor, INMP441 microphone, and current firmware behavior |

## 🧠 Firmware Layer

The ESP32 firmware handles time-sensitive robot behavior:

- PCA9685 initialization and 12-servo PWM output.
- Per-joint calibration through `LegJoint` and `leg_data.h`.
- Inverse kinematics for coxa, femur, and tibia joints.
- Gait generation for stand, walk, trot, gallop, turns, flip, bow, sit, paw, and stop.
- Serial monitor command handling for testing and trimming.
- Ultrasonic wall-stop behavior.
- Microphone driver initialization; clap-toggle logic exists but is currently commented out in `main.cpp`.
- FreeRTOS-based UDP communication with mDNS service advertisement when `PLUTO_ENABLE_WIFI` is enabled.

Current default feature flags in `src/esp/main.cpp`:

```cpp
#define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

See [ESP32 Firmware](src/esp/README.md).

### 🧠 Firmware Bring-Up Order

Use this sequence when setting up a new robot:

1. Build firmware without changing feature flags.
2. Upload to ESP32 and open serial monitor.
3. Verify PCA9685 startup and stand pose.
4. Test `s`, `p`, `l`, `n`, `+`, `-`, and `r` before walking.
5. Tune `src/esp/legs/leg_data.h`.
6. Test `1`, `2`, `3` gait selection without aggressive speeds.
7. Validate WiFi control only after serial control is safe.

## 🎛️ Control Layer

The Python control hub provides operator-facing tools:

- NiceGUI home, simulation, and controller pages.
- Keyboard and browser gamepad input normalization.
- `MOVE_BY` command sending while movement input is non-zero; firmware maps the vectors to forward, backward, left turn, right turn, or stop.
- Quick behavior buttons for sit, give paw, and stop.
- Telemetry display for acknowledgements and distance messages.
- Vosk-based speech commands using the controller computer microphone.
- UDP packet creation and session handling through `PlutoController`.

The controller discovers the ESP32 with zeroconf/mDNS when `Connect & Take Control` is pressed.

See [Python Controller](src/control/README.md).

### 🎛️ Controller Bring-Up Order

Use this sequence when setting up the controller:

1. Run `bash run.sh` or `run.bat`.
2. Confirm the NiceGUI home page opens.
3. Open `/controller` and check that keyboard vectors update.
4. Confirm `PLUTO_ENABLE_WIFI` is enabled and WiFi credentials are configured in firmware.
5. Connect from the controller page.
6. Test `Stop All` before movement.
7. Test a small movement vector.
8. Check telemetry for acknowledgements.

## 🔁 Communication Layer

Python and C++ share a compact binary protocol. Commands are grouped into UDP packets with:

- CRC validation.
- Session tokens.
- Sequence numbers.
- Device timestamps.
- Up to 64 packed 8-byte messages per packet.

See [Communication](src/comm/README.md) and [WiFi Protocol](SOFTWARE_WIFI.md).

### 🔁 Keeping Protocol Files In Sync

Whenever a message family, kind, payload, or packet field changes:

1. Update `src/comm/message.h`.
2. Update `src/control/pluto_server/message.py`.
3. Update [src/comm/README.md](src/comm/README.md).
4. Test packing and unpacking on the Python side.
5. Flash firmware and test one packet type at a time.

## 🎮 Simulation Layer

Pluto currently has two simulation-facing paths:

- Python PyBullet control path under `src/control`.
- MuJoCo XML model and C++ simulation bridge under `src/sim/sim_mesh` and `src/sim`.

The MuJoCo model uses the current per-leg STL files. The Python PyBullet visual path is older and still expects generic leg mesh filenames, so it needs a mesh update to fully match the current CAD set.

See [Simulation Notes](SIMULATION.md).

## 📌 Current Extension Points

- Better gait stability and interpolation.
- Live WiFi control tuning on hardware.
- More complete behavior sequences.
- Higher-fidelity simulation.
- Autonomous obstacle avoidance.

See [Ongoing Works & Next Steps](ONGOING_WORK.md).
