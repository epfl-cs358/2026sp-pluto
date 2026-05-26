# Software Overview

Pluto's software is split into four active areas: ESP32 firmware, Python controller, shared communication protocol, and simulation support.

## Main Components

| Component | Location | Role |
| --- | --- | --- |
| ESP32 firmware | [src/esp](src/esp/README.md) | Runs on the robot, controls servos, executes gaits, reads sensors, and optionally handles UDP packets |
| Python control hub | [src/control](src/control/README.md) | Provides the NiceGUI interface, PyBullet control page, keyboard/gamepad input, speech worker, and UDP client |
| Shared protocol | [src/comm](src/comm/README.md) | Defines the compact packet and message format shared by C++ firmware and Python control code |
| Simulation assets | [SIMULATION.md](SIMULATION.md) | Documents the Python PyBullet path, MuJoCo XML model, C++ bridge, and current limitations |
| WiFi protocol notes | [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md) | Documents UDP connection setup, packet structure, batching, CRCs, and payload layout |
| Sensor notes | [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) | Documents the ultrasonic distance sensor, INMP441 microphone, and current firmware behavior |

## Firmware Layer

The ESP32 firmware handles time-sensitive robot behavior:

- PCA9685 initialization and 12-servo PWM output.
- Per-joint calibration through `LegJoint` and `leg_data.h`.
- Inverse kinematics for coxa, femur, and tibia joints.
- Gait generation for stand, walk, trot, gallop, bow, paw, and stop.
- Serial monitor command handling for testing and trimming.
- Ultrasonic wall-stop behavior.
- Microphone clap-toggle behavior.
- Optional FreeRTOS-based UDP communication when `PLUTO_ENABLE_WIFI` is enabled.

Current default feature flags in `src/esp/main.cpp`:

```cpp
// #define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

See [ESP32 Firmware](src/esp/README.md).

## Control Layer

The Python control hub provides operator-facing tools:

- NiceGUI home, simulation, and controller pages.
- Keyboard and browser gamepad input normalization.
- `MOVE_BY` command sending while movement input is non-zero.
- Quick behavior buttons for sit, give paw, and stop.
- Telemetry display for acknowledgements and distance messages.
- Vosk-based speech commands using the controller computer microphone.
- UDP packet creation and session handling through `PlutoController`.

The ESP32 IP address is currently a placeholder in `src/control/main.py` and must be set before physical WiFi control.

See [Python Controller](src/control/README.md).

## Communication Layer

Python and C++ share a compact binary protocol. Commands are grouped into UDP packets with:

- CRC validation.
- Session tokens.
- Sequence numbers.
- Device timestamps.
- Up to 64 packed 8-byte messages per packet.

See [Communication](src/comm/README.md) and [WiFi Protocol](SOFTWARE_WIFI.md).

## Simulation Layer

Pluto currently has two simulation-facing paths:

- Python PyBullet control path under `src/control`.
- MuJoCo XML model and C++ simulation bridge under `src/mesh` and `src/sim`.

The MuJoCo model uses the current per-leg STL files. The Python PyBullet visual path is older and still expects generic leg mesh filenames, so it needs a mesh update to fully match the current CAD set.

See [Simulation Notes](SIMULATION.md).

## Current Extension Points

- Better gait stability and interpolation.
- Live WiFi control validation on hardware.
- More complete behavior sequences.
- IMU-based stabilization.
- Camera or SLAM-based perception.
- Higher-fidelity simulation.
- Autonomous obstacle avoidance.

See [Ongoing Works & Next Steps](ONGOING_WORK.md).
