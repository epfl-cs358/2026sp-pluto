<div align="center">

# Pluto

**A compact 12-DOF quadruped robot with ESP32 gait control, calibrated servo actuation, sensor reactions, controller-side speech commands, and simulation assets.**

![PlatformIO](https://img.shields.io/badge/Firmware-PlatformIO-orange)
![ESP32](https://img.shields.io/badge/Controller-ESP32-blue)
![Python](https://img.shields.io/badge/UI-Python%203.13-green)
![NiceGUI](https://img.shields.io/badge/Web%20UI-NiceGUI-purple)
![Simulation](https://img.shields.io/badge/Simulation-PyBullet%20%2B%20MuJoCo-lightgrey)
![Course](https://img.shields.io/badge/EPFL-CS--358-red)

> **Current status:** Pluto has the main robot platform in place: ESP32 firmware, gait code, servo calibration hooks, ultrasonic and microphone reactions, a Python controller, shared UDP messages, updated per-leg meshes, and a MuJoCo model. Final physical gait tuning and some behavior sequences still need validation on the real robot.

> **Live demo:** Demo media can be added here once final robot videos are available.

---

## Quick Navigation

<table>
<tr>
<td width="25%" align="center">
<a href="ASSEMBLY.md"><strong>Build Pluto</strong></a><br>
Assembly steps, leg construction, body integration, and final checks.
</td>
<td width="25%" align="center">
<a href="WIRING_ELECTRICAL.md"><strong>Wire Pluto</strong></a><br>
Power path, common ground, PCA9685, sensors, and safety checks.
</td>
<td width="25%" align="center">
<a href="src/esp/README.md"><strong>ESP32 Firmware</strong></a><br>
Gaits, IK, servos, sensors, serial commands, and optional WiFi.
</td>
<td width="25%" align="center">
<a href="src/control/README.md"><strong>Controller UI</strong></a><br>
NiceGUI, keyboard/gamepad input, speech commands, and telemetry.
</td>
</tr>
<tr>
<td width="25%" align="center">
<a href="CAD_FILES.md"><strong>Meshes</strong></a><br>
Current STL inventory and MuJoCo mesh usage.
</td>
<td width="25%" align="center">
<a href="src/comm/README.md"><strong>Protocol</strong></a><br>
Shared C++/Python UDP packet and message format.
</td>
<td width="25%" align="center">
<a href="SIMULATION.md"><strong>Simulation</strong></a><br>
PyBullet UI path, MuJoCo bridge, and current limitations.
</td>
<td width="25%" align="center">
<a href="TROUBLESHOOTING.md"><strong>Troubleshooting</strong></a><br>
First-time setup, firmware, power, sensors, and WiFi issues.
</td>
</tr>
</table>

---

## Table of Contents

<table>
<tr>
<td width="33%">

- [Project Context](#project-context)
- [What Pluto Can Do](#what-pluto-can-do)
- [Current Status](#current-status)
- [Quick Start](#quick-start)

</td>
<td width="33%">

- [System Architecture](#system-architecture)
- [Hardware Overview](#hardware-overview)
- [Software Overview](#software-overview)
- [Simulation](#simulation)

</td>
<td width="33%">

- [Configuration and Tuning](#configuration-and-tuning)
- [Repository Structure](#repository-structure)
- [Known Limitations](#known-limitations)
- [Detailed Technical Reference](#detailed-technical-reference)

</td>
</tr>
</table>

---

## Project Context

<table>
<tr>
<td width="50%">

### Vision

Pluto is designed as a reusable quadruped robotics platform rather than a single-purpose demo. The goal is to make a small robot that can stand, walk, trot, gallop, react to simple sensor events, and accept commands through serial, WiFi, UI controls, and speech.

The same concepts appear across the firmware, controller, and simulation code: leg geometry, foot targets, inverse kinematics, gait phases, calibrated servo limits, and compact command messages.

</td>
<td width="50%">

### Course Context

Pluto was developed as part of **CS-358 Making Intelligent Things** at **EPFL**.

The repository documents both the final implementation and the current work-in-progress state so future development can continue from the same hardware, firmware, controller, protocol, and simulation foundation.

</td>
</tr>
</table>

### Main Implementation Layers

| Layer | Role |
| --- | --- |
| ESP32 firmware | Servo control, gait execution, ultrasonic sensing, microphone sensing, serial testing, and optional WiFi/UDP command handling |
| Python tooling | NiceGUI control hub, keyboard/gamepad input, speech command handling, telemetry display, and simulation control |
| Shared communication | C++ and Python packet/message definitions kept aligned |
| CAD and simulation assets | Per-leg STL meshes, `pluto.xml`, PyBullet-facing code, and MuJoCo bridge |

---

## What Pluto Can Do

<table>
<tr>
<td width="50%">

### Robot Side

- Drive 12 servo joints through a PCA9685 PWM driver.
- Use per-joint calibration for coxa, femur, and tibia angles.
- Run stand, stop, walk, trot, gallop, bow, and paw motion logic.
- Update gait motion every 20 ms.
- Stop forward motion when the ultrasonic sensor detects a close wall.
- Toggle walking on microphone clap-energy detection.
- Accept serial monitor commands for gait testing and servo trimming.
- Optionally receive UDP commands when WiFi support is enabled.

</td>
<td width="50%">

### Computer Side

- Launch a NiceGUI control hub.
- Use keyboard and browser gamepad input.
- Send repeated `MOVE_BY` messages while movement input is active.
- Trigger sit, give paw, and stop commands from the UI.
- Run Vosk speech recognition on the controller computer.
- Display acknowledgement and distance telemetry.
- Open the Python PyBullet simulation path.
- Maintain shared message definitions with the firmware.

</td>
</tr>
</table>

---

## Current Status

| Area | Status | Notes |
| --- | --- | --- |
| Physical robot | In progress | Main build and wiring docs exist; final gait validation still needed |
| Servo control | Implemented | Calibration lives in `src/esp/legs/leg_data.h` |
| Gaits | Implemented, tuning needed | Walk, trot, gallop, bow, paw, stand, and stop logic exist |
| Sensors | Implemented | Ultrasonic wall stop and microphone clap toggle are enabled by default |
| Controller UI | Implemented | Connect button, movement input, quick actions, telemetry, speech worker |
| WiFi/UDP | Implemented, disabled by default | Enable `PLUTO_ENABLE_WIFI` and set `IP_OF_ESP` before use |
| Meshes | Updated | Current assets are per-leg STL files plus `pluto.xml` |
| PyBullet | Partial | UI path exists, but visual meshes still reference old generic filenames |
| MuJoCo | Partial | Model and C++ bridge exist, but not yet a validated physical twin |

---

## Quick Start

### 1. Install Requirements

For the Python controller:

- Python 3.13
- Desktop environment for the browser UI and simulation windows
- Audio permission if using speech commands

For the ESP32 firmware:

- PlatformIO Core or the PlatformIO VS Code extension
- ESP32 board connected over USB
- Pluto hardware assembled or a safe bench setup

For the physical robot:

- Common ground between ESP32, PCA9685, servo power, sensors, and battery system
- Buck converter output checked with a multimeter
- Servo calibration checked before full gait tests

### 2. Run the Controller

macOS/Linux:

```bash
bash run.sh
```

Windows:

```cmd
run.bat
```

Open:

```text
http://localhost:8090
```

Useful options:

```bash
bash run.sh --reinstall
bash run.sh --port 8081
```

Manual setup, if needed:

```bash
python3.13 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python src/control/main.py --port 8090
```

### 3. Build and Flash the ESP32

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

If the upload port is not detected automatically:

```bash
pio device list
pio run -t upload --upload-port /dev/cu.usbserial-XXXX
```

Expected startup behavior:

- PCA9685 initializes at 50 Hz.
- Enabled sensors initialize.
- Pluto enters a stand pose.
- The serial monitor prints available commands.

### 4. Enable WiFi Control

WiFi support exists but is disabled by default in `src/esp/main.cpp`.

Enable:

```cpp
#define PLUTO_ENABLE_WIFI
```

Then:

1. Configure access points with `PLUTO_SERVER.addAP(...)`.
2. Set `IP_OF_ESP` in `src/control/main.py`.
3. Make sure the computer and ESP32 are on the same network.
4. Use the controller page to connect and send commands.

See [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md).

---

## System Architecture

```text
+---------------------------------------------------------+
| Python Control Hub                                      |
| NiceGUI UI, keyboard/gamepad, speech, telemetry, UDP     |
+---------------------------+-----------------------------+
                            |
                            | WiFi / UDP port 4242
                            v
+---------------------------------------------------------+
| ESP32 Firmware                                          |
| 20 ms gait loop, IK, PCA9685 output, sensors, UDP server |
+---------------------------+-----------------------------+
                            |
                            v
+---------------------------------------------------------+
| Hardware and Simulation Assets                          |
| 12 servos, 4 legs, ultrasonic, microphone, STL/MuJoCo    |
+---------------------------------------------------------+
```

<table>
<tr>
<td width="33%">

### Control Layer

Runs on the user's computer.

- NiceGUI app
- keyboard/gamepad input
- Vosk speech worker
- UDP controller client
- telemetry display
- simulation controls

</td>
<td width="33%">

### Robot Layer

Runs on the ESP32.

- PCA9685 servo output
- leg calibration
- inverse kinematics
- gait controller
- ultrasonic checks
- microphone energy checks
- optional UDP server

</td>
<td width="33%">

### Shared Layer

Keeps both sides aligned.

- C++ message definitions
- Python message mirror
- packet CRC checks
- session tokens
- sequence numbers
- mesh and simulation assets

</td>
</tr>
</table>

### Why The System Is Split This Way

| Design choice | Reason |
| --- | --- |
| Keep gait and servo output on ESP32 | Servo updates and stop/stand behavior stay close to the hardware |
| Keep UI, speech, and simulation on the computer | These are heavier and easier to iterate in Python |
| Mirror message definitions in C++ and Python | The controller and robot share one compact command language |
| Keep simulation assets in the repo | Gait timing and actuator mapping can be inspected before hardware tests |

---

## Hardware Overview

| Component | Qty | Role |
| --- | ---: | --- |
| 3D-printed body and leg parts | 1 set | Robot structure |
| DMS15-style 270-degree servos | 12 | Coxa, femur, and tibia actuation |
| PCA9685 PWM driver | 1 | 16-channel servo PWM over I2C |
| ESP32 development board | 1 | Main embedded controller |
| HC-SR04-style ultrasonic sensor | 1 | Front distance sensing |
| INMP441 I2S microphone | 1 | Audio energy sensing |
| 7.4V 2S LiPo battery | 1 | Main power source |
| LM2596 buck converter | 1 | Regulated electronics power |
| KCD1 rocker switch | 1 | Main power switching |
| Screws, bearings, servo horns, wiring | As needed | Mechanical and electrical assembly |

### Additional Materials

- PLA/PETG filament for rigid printed parts.
- TPU filament or rubber pads for feet.
- M3 and M2.5 screws for the body, legs, and servo mounting.
- Ball bearings for tibia joints.
- Jumper wires for logic signals and thicker wires for servo current.
- Soldering equipment, heat-shrink tubing, electrical tape, and cable ties.
- Multimeter for voltage, polarity, and continuity checks.
- LiPo-safe charger and LiPo-safe storage bag.

### Current Mesh Set

The current STL files live in `src/mesh`:

```text
body.stl
tl_coxa.stl   tl_femur.stl   tl_tibia.stl
tr_coxa.stl   tr_femur.stl   tr_tibia.stl
bl_coxa.stl   bl_femur.stl   bl_tibia.stl
br_coxa.stl   br_femur.stl   br_tibia.stl
pluto.xml
```

The old generic `coxa`, `femur`, and `tibia` meshes were replaced by per-leg meshes. See [CAD_FILES.md](CAD_FILES.md).

### Build Flow

<table>
<tr>
<td align="center"><strong>1</strong><br>Print parts</td>
<td align="center"><strong>2</strong><br>Assemble legs</td>
<td align="center"><strong>3</strong><br>Mount electronics</td>
<td align="center"><strong>4</strong><br>Wire power and sensors</td>
<td align="center"><strong>5</strong><br>Flash firmware</td>
<td align="center"><strong>6</strong><br>Calibrate servos</td>
</tr>
</table>

Detailed hardware docs:

- [ASSEMBLY.md](ASSEMBLY.md)
- [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md)
- [HARDWARE_OVERVIEW.md](HARDWARE_OVERVIEW.md)
- [CAD_FILES.md](CAD_FILES.md)

### Electrical Safety Notes

| Topic | Note |
| --- | --- |
| Common ground | LiPo/servo ground, PCA9685 ground, ESP32 ground, buck converter ground, and sensor grounds must be connected together |
| Servo power | Servos draw much higher current than the ESP32 and sensors; plan the power paths separately |
| Buck converter | Adjust and measure the LM2596 output before connecting the ESP32 or sensors |
| PCA9685 | Provide both logic power and servo power, and connect SDA/SCL to the ESP32 |
| Ultrasonic ECHO | If ECHO outputs 5V, use a voltage divider or level shifter before the ESP32 GPIO |
| High-current wiring | Use appropriate wire thickness for LiPo, XT60, rocker switch, servo power rail, and PCA9685 servo power |

---

## Software Overview

### Firmware

PlatformIO environment:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
build_unflags = -std=gnu++11
build_flags = -std=gnu++17
```

PlatformIO project settings:

| Setting | Value |
| --- | --- |
| `src_dir` | `src/esp` |
| `include_dir` | `src/` |
| `build_dir` | `.build/` |
| `build_src_filter` | Excludes only paths under the firmware source root; `src/sim` is outside the ESP32 firmware build |

Important firmware files:

| File | Purpose |
| --- | --- |
| `src/esp/main.cpp` | setup, loop, feature flags, sensors, serial commands |
| `src/esp/legs/leg_data.h` | servo calibration values |
| `src/esp/legs/leg_joint.h` | calibrated servo abstraction |
| `src/esp/motion/gait.cpp` | gait and behavior motion logic |
| `src/esp/motion/ik_solver.cpp` | inverse kinematics |
| `src/esp/sensors/ultrasonic.h` | ultrasonic sensor abstraction |
| `src/esp/sensors/microphone.h` | I2S microphone abstraction |
| `src/esp/server/server.cpp` | optional UDP server |

### Controller

Python dependencies:

```text
nicegui
numpy
debugpy
pybullet
mujoco
glfw
vosk
sounddevice
```

Important controller files:

| File | Purpose |
| --- | --- |
| `src/control/main.py` | NiceGUI entry point |
| `src/control/pluto_menu/controller.py` | controller page, movement, behavior buttons, telemetry |
| `src/control/pluto_menu/simulation.py` | simulation page |
| `src/control/pluto_input/input_manager.py` | keyboard/gamepad movement vector |
| `src/control/pluto_server/message.py` | Python message definitions |
| `src/control/pluto_server/server.py` | UDP client, session, heartbeat, listener |
| `src/control/pluto_speech/speech.py` | Vosk speech worker |

Controller routes:

| Route | Purpose |
| --- | --- |
| `/` | Home page with simulation and controller choices |
| `/sim` | Simulation controls |
| `/controller` | Robot command center |

Current speech grammar:

- `pluto sit`
- `pluto stop`
- `pluto give paw`

### Communication

The shared UDP packet contains:

| Field | Size | Purpose |
| --- | ---: | --- |
| `crc32` | 4 bytes | Packet integrity |
| `session_token` | 4 bytes | Session lock generated by ESP32 |
| `sequence_number` | 4 bytes | Stale packet rejection and acknowledgements |
| `timestamp` | 4 bytes | Sender-side local time |
| `message_count` | 1 byte | Number of valid messages |
| `messages` | 8 bytes each | Up to 64 packed messages |

Message families:

- `KIND_INFO`
- `KIND_MOVE`
- `KIND_SENSOR`
- `KIND_BEHAVIOR`

Implemented message examples:

| Message | Direction | Meaning |
| --- | --- | --- |
| `MOVE_CONTROL_BEGIN_FOR` | Python -> ESP32 | Starts a control lease |
| `MOVE_BY` | Python -> ESP32 | Carries forward/back and left/right signed 16-bit directions |
| `MOVE_STOP_FOR` | Python -> ESP32 | Stop/stand request |
| `BEHAVIOR_SIT` | Python -> ESP32 | High-level sit request |
| `BEHAVIOR_GIVE_PAW` | Python -> ESP32 | High-level paw request |
| `BEHAVIOR_LIE_DOWN` | Python -> ESP32 | High-level lie-down request |
| `INFO_ACKNOWLEDGE` | ESP32 -> Python | Acknowledges a packet sequence number |
| `SENSOR_DISTANCE` | ESP32 -> Python | Sends ultrasonic distance in millimeters |

Useful references:

- [SOFTWARE_OVERVIEW.md](SOFTWARE_OVERVIEW.md)
- [src/comm/README.md](src/comm/README.md)
- [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md)

---

## Simulation

| Path | Files | Current state |
| --- | --- | --- |
| Python PyBullet UI | `src/control/pluto_menu/simulation.py`, `src/control/sim_motion.py` | Integrated in the UI, but visual mesh references need updating |
| MuJoCo C++ bridge | `src/sim/sim.cpp`, `src/sim/MockPWMServoDriver.h`, `src/mesh/pluto.xml` | Uses current per-leg meshes and reuses ESP gait code |

The MuJoCo path is more aligned with the current mesh set. The PyBullet path is still useful for UI/control experiments, but it needs a mesh update before it fully represents the current CAD assets.

See [SIMULATION.md](SIMULATION.md).

---

## Configuration and Tuning

### Firmware Feature Flags

Current defaults in `src/esp/main.cpp`:

```cpp
// #define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

| Feature | Default | Notes |
| --- | --- | --- |
| WiFi/UDP | Off | Enable `PLUTO_ENABLE_WIFI` before using controller networking |
| Ultrasonic | On | Stops forward motion when wall distance is below threshold |
| Microphone | On | Toggles walking on clap-energy detection |

### Sensor Pins and Thresholds

| Item | Current value |
| --- | --- |
| Ultrasonic template | `SensorUltraSonic<5, 18>` |
| Microphone template | `SensorMicrophone<26, 25, 33>` |
| Wall stop distance | 20 cm |
| Ultrasonic period | 150 ms |
| Microphone clap threshold | `2000000` |
| Clap cooldown | 800 ms |

Operating timing:

| Loop / event | Current timing |
| --- | --- |
| Gait update | 20 ms / 50 Hz |
| Ultrasonic read cycle | 150 ms |
| Ultrasonic wait after trigger | 10 ms |
| Microphone energy print cycle | 150 ms |
| Serial monitor baud | 115200 |

### Servo Calibration

Servo calibration lives in:

```text
src/esp/legs/leg_data.h
```

Each joint has:

- `raw_min`
- `raw_max`
- `raw_start`
- `angle_min_md`
- `angle_max_md`
- `inverted`

Tune these values on the physical robot before running full gaits.

### Gait and IK Parameters

Embedded gait code lives in:

```text
src/esp/motion/gait.cpp
src/esp/motion/gait.h
```

Embedded IK code lives in:

```text
src/esp/motion/ik_solver.cpp
src/esp/motion/ik_solver.h
```

Tune these areas carefully:

- Stand pose.
- Foot target geometry.
- Stride length.
- Lift height.
- Gait period.
- Walk/trot/gallop phase offsets.
- Per-leg symmetry.
- Bow and paw target poses.

### Serial Commands

Use:

```bash
pio device monitor -b 115200
```

| Key | Action |
| --- | --- |
| `f` | Move forward |
| `b` | Move backward |
| `o` | Bow |
| `k` | Give paw |
| `s` | Stop and stand |
| `1` | Select walk gait |
| `2` | Select trot gait |
| `3` | Select gallop gait |
| `0` | Set speed to 0% |
| `5` | Set speed to 50% |
| `9` | Set speed to 100% |
| `u` | Toggle manual walk phase mode |
| `m` | Advance manual walk stage |
| `j` | Advance manual walk leg |
| `l` | Select next leg for trimming |
| `n` | Select next joint for trimming |
| `+` | Increase selected joint raw PWM by 5 |
| `-` | Decrease selected joint raw PWM by 5 |
| `r` | Reset selected joint to starting pulse |
| `R` | Reset all joints on selected leg |
| `p` | Print selected leg, joint, and pulse |

---

## Repository Structure

```text
2026sp-pluto/
|-- README.md
|-- ARCHIVES.md
|-- ASSEMBLY.md
|-- CAD_FILES.md
|-- HARDWARE_OVERVIEW.md
|-- ONGOING_WORK.md
|-- SIMULATION.md
|-- SOFTWARE_OVERVIEW.md
|-- SOFTWARE_SENSORS.md
|-- SOFTWARE_WIFI.md
|-- TROUBLESHOOTING.md
|-- WIRING_ELECTRICAL.md
|-- platformio.ini
|-- requirements.txt
|-- run.sh / run.bat
|-- images/
|-- scripts/
`-- src/
    |-- comm/       # Shared protocol definitions
    |-- control/    # Python UI, input, speech, and controller client
    |-- esp/        # ESP32 firmware
    |-- mesh/       # STL meshes and MuJoCo XML model
    `-- sim/        # C++ MuJoCo simulation bridge
```

---

## Known Limitations

<table>
<tr>
<td width="50%">

### Hardware and Motion

- Physical gait validation is still needed.
- Servo calibration needs final tuning.
- Gait constants need real-world measurement.
- Behavior sequences need calibration.

</td>
<td width="50%">

### Software and Simulation

- WiFi is disabled by default.
- Controller IP is currently hard-coded.
- PyBullet visual meshes need updating.
- MuJoCo is not yet a validated physical twin.

</td>
</tr>
</table>

See [ONGOING_WORK.md](ONGOING_WORK.md).

---

## Detailed Technical Reference

<details>
<summary><strong>Technical vocabulary</strong></summary>

- **ESP32**: Microcontroller running the robot firmware.
- **PCA9685**: I2C PWM driver used to control the 12 servo channels.
- **12-DOF**: Twelve degrees of freedom, with three actuated joints per leg.
- **Coxa, femur, tibia**: The hip-yaw, upper-leg, and lower-leg joints.
- **IK**: Inverse kinematics, converting foot targets into joint angles.
- **Gait**: Timed leg movement pattern such as walk, trot, or gallop.
- **UDP**: Lightweight network protocol used for controller-to-robot messages.
- **CRC**: Packet integrity check used by the shared communication protocol.
- **NiceGUI**: Python web UI framework used for the control hub.
- **Vosk**: Offline speech recognition engine.
- **MuJoCo**: Physics simulator used by `src/mesh/pluto.xml` and `src/sim/sim.cpp`.
- **PyBullet**: Python simulation dependency used by the control stack.

</details>

<details>
<summary><strong>Project objectives</strong></summary>

- Reliable servo control: drive 12 calibrated joints safely through the PCA9685.
- Legged locomotion: support stand, stop, walk, trot, gallop, bow, and paw motion primitives.
- IK-based movement: generate joint angles from foot targets instead of fixed pulse sequences.
- Remote operation: use a shared UDP protocol for movement, behavior, info, acknowledgement, and sensor messages.
- Sensor reactions: stop near obstacles and use microphone energy for clap-triggered start/stop behavior.
- Simulation before hardware: keep mesh and simulation assets available for gait development.
- Clear documentation: keep hardware, wiring, firmware, controller, protocol, and troubleshooting notes separated.

</details>

<details>
<summary><strong>Core technologies</strong></summary>

- **Hardware**: ESP32, PCA9685, 12 DMS15-style servos, HC-SR04-style ultrasonic sensor, INMP441 I2S microphone, 2S LiPo, LM2596 buck converter.
- **Firmware**: C++17, Arduino framework, FreeRTOS, PlatformIO, Adafruit PWM Servo Driver, Adafruit BusIO.
- **Control UI**: Python 3.13, NiceGUI, keyboard/gamepad input, Vosk, sounddevice.
- **Simulation**: PyBullet, MuJoCo, GLFW, STL mesh assets.
- **Communication**: Custom C++/Python UDP packet format with session token, sequence number, timestamp, CRC, and packed messages.

</details>

<details>
<summary><strong>ESP32 firmware responsibilities</strong></summary>

- Initialize the PCA9685 servo driver.
- Drive all 12 joints through calibrated `LegJoint` objects.
- Convert IK outputs into constrained PWM pulses.
- Update gait motion every 20 ms.
- Handle serial commands for gait selection, speed changes, manual walk staging, and servo trimming.
- Read ultrasonic distance and stop when a wall is too close.
- Read microphone energy and toggle walking on clap detection.
- Optionally accept UDP commands through the Pluto server.

</details>

<details>
<summary><strong>Python controller responsibilities</strong></summary>

- Serve the NiceGUI app from `src/control/main.py`.
- Provide navigation between home, simulation, and controller pages.
- Read keyboard and gamepad input through `InputManager`.
- Send repeated `MOVE_BY` messages while movement input is non-zero.
- Send quick behavior commands for sit, give paw, and stop.
- Display telemetry for acknowledgements and distance readings.
- Start a Vosk speech worker on app startup.
- Start and stop simulation support from the UI.

</details>

<details>
<summary><strong>Documentation index</strong></summary>

Main system:

- [Software Overview](SOFTWARE_OVERVIEW.md)
- [Ongoing Works & Next Steps](ONGOING_WORK.md)
- [Troubleshooting](TROUBLESHOOTING.md)

Hardware:

- [Hardware Overview](HARDWARE_OVERVIEW.md)
- [Assembly Guide](ASSEMBLY.md)
- [Wiring & Electrical](WIRING_ELECTRICAL.md)
- [CAD Files](CAD_FILES.md)

Software:

- [ESP32 Firmware](src/esp/README.md)
- [Python Controller](src/control/README.md)
- [Communication Protocol](src/comm/README.md)
- [WiFi Protocol](SOFTWARE_WIFI.md)
- [Software Sensors](SOFTWARE_SENSORS.md)
- [Simulation Notes](SIMULATION.md)

Key source files:

- [Python UI entry point](src/control/main.py)
- [Controller page](src/control/pluto_menu/controller.py)
- [Simulation page](src/control/pluto_menu/simulation.py)
- [Python message definitions](src/control/pluto_server/message.py)
- [ESP firmware entry point](src/esp/main.cpp)
- [ESP gait controller](src/esp/motion/gait.cpp)
- [ESP IK solver](src/esp/motion/ik_solver.cpp)
- [Shared C++ message definitions](src/comm/message.h)
- [MuJoCo model](src/mesh/pluto.xml)
- [C++ simulation bridge](src/sim/sim.cpp)

</details>

<details>
<summary><strong>Archives and historical work</strong></summary>

The repository currently keeps most history through Git, active source files, and [ONGOING_WORK.md](ONGOING_WORK.md). If older experiments are added back for reference, they should go under an `archives/` directory with a short README explaining what was tested, why it was replaced, and whether it still runs.

Recommended archive categories:

- ESP32 experiments: old gait tests, servo calibration sketches, sensor bring-up code, and behavior prototypes.
- Control UI experiments: early NiceGUI pages, keyboard/gamepad tests, and speech command prototypes.
- Simulation experiments: PyBullet and MuJoCo model tests, gait playback experiments, and old mesh-loading attempts.
- Communication experiments: older UDP packet formats, handshake tests, and protocol debugging scripts.

See [ARCHIVES.md](ARCHIVES.md).

</details>

<details>
<summary><strong>Ongoing work and next steps</strong></summary>

Current focus:

- Hardware gait validation: test walk, trot, gallop, bow, paw, and stop on the physical robot.
- Servo calibration: refine PWM limits, starting pulses, inversion flags, and angle ranges.
- WiFi control: enable and validate live UDP movement and behavior commands.
- Behavior implementation: replace placeholder behavior handlers with calibrated motion sequences.
- Sensor-driven reactions: tune ultrasonic wall stopping and microphone clap detection.
- Simulation fidelity: improve physical accuracy for mass, friction, joint limits, and servo response.

Known issues:

- WiFi support is present but disabled by default in `src/esp/main.cpp`.
- Some firmware behavior handlers are still placeholders.
- The ESP32 IP address is still a placeholder in `src/control/main.py`.
- Gait constants need final physical measurement and tuning.
- MuJoCo and PyBullet support are useful for development, but not yet perfect models of the real robot.

</details>

---

## Project Team

Pluto was developed as part of EPFL's **CS-358 Making Intelligent Things** course by:

<table>
<tr>
<td align="center">Serhat Botan</td>
<td align="center">Alexis Cazal</td>
<td align="center">Neha Chakraborty</td>
<td align="center">Myriam Lahoud</td>
</tr>
<tr>
<td align="center">Sam Lee</td>
<td align="center">Raphael Dib Nehme</td>
<td align="center">Mariya Rakytyanska</td>
<td align="center">EPFL CS-358</td>
</tr>
</table>

## Acknowledgments

- EPFL course staff, TAs, and coaches.
- Open-source quadruped projects, including SpotMicro-style robots.
- Arduino, PlatformIO, FreeRTOS, Adafruit, NiceGUI, PyBullet, MuJoCo, GLFW, Vosk, and sounddevice documentation and examples.

## Use of AI Tools

AI tools were used to support:

- Documentation structure and technical writing.
- Debugging guidance and code review suggestions.
- Edge-case review for setup, communication, and hardware integration notes.
- Summaries of implementation details from the codebase.

AI assistance was not used to replace:

- Project goals or engineering decision-making.
- Hardware assembly, wiring, or physical testing.
- Experimental validation on the real robot.
- Team ownership of the system design and implementation.

---

## Closing Notes

Pluto is a working foundation for a small quadruped robotics platform. The repository already contains low-level actuation, IK, gait generation, sensing hooks, controller-side interaction, command transport, and simulation assets. The next major step is careful physical tuning: servo calibration, gait stability, behavior sequences, and validation on the real robot.
