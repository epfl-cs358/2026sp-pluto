# Pluto

> A four-legged canine robot with ESP32-based gait control, calibrated 12-servo actuation, WiFi command transport, voice-triggered behaviors, and a PyBullet simulation/control hub.

**Live Demo**

_Demo media can be added here once the final robot videos are available._

## Project Context

**Abstract**

Pluto is a 12-degree-of-freedom quadruped robot developed around the idea of making a compact canine-style robot that can move, react, and be controlled interactively. Each of Pluto's four legs has three joints: coxa, femur, and tibia. The project combines mechanical design, embedded motor control, inverse kinematics, gait generation, wireless communication, sensor feedback, and a Python control interface.

The original project proposal set out a modular robot capable of autonomous locomotion, interactive behavior, obstacle sensing, voice commands, and manual WiFi control. The current repository implements the core platform needed for that goal: ESP32 firmware for servo control and gait execution, a shared UDP packet protocol, ultrasonic and microphone sensor drivers, a NiceGUI controller UI, Vosk-based speech command handling, and a PyBullet simulator using Pluto's STL meshes.

**Course Context**

This project was developed as part of the **Making Intelligent Things** course at **EPFL**.

**Acknowledgments**

- Course staff, TAs, and coaches for guidance throughout the design and development process
- Existing open-source quadruped projects, including SpotMicro and related quadruped robotics tutorials, for inspiration on legged locomotion and mechanical structure
- The PyBullet robotics community and reference robot dog implementations that helped shape the simulation approach

**Note on Continued Development**

Pluto is designed as a modular platform, so the current codebase is a foundation rather than a final endpoint. The implemented system already covers servo calibration, inverse kinematics, gait generation, UDP control, speech commands, sensing hooks, and simulation. Future work can build on this by improving gait stability, adding IMU or camera feedback, expanding autonomous obstacle avoidance, implementing more expressive behaviors, and using the platform for higher-level robotics experiments. For current work-in-progress items, known limitations, and future directions, see [Ongoing Works & Next Steps](ONGOING_WORK.md).

---

## Quick Jump To Detailed Documentation

<table>
<tr>
<td width="25%" align="center">

### [Controller UI](src/control/README.md)

Python control hub, NiceGUI pages, PyBullet simulation, speech command worker

</td>
<td width="25%" align="center">

### [ESP32 Firmware](src/esp/README.md)

Robot-side motion control, servo output, sensors, WiFi UDP server

</td>
<td width="25%" align="center">

### [Communication](src/comm/README.md)

Shared packet/message format used by Python and C++

</td>
<td width="25%" align="center">

### [Next Steps](ONGOING_WORK.md)

Known limitations, active work, and future extensions

</td>
</tr>
</table>

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Quick Start](#quick-start)
3. [Hardware](#hardware)
4. [System Architecture](#system-architecture)
5. [Software Setup](#software-setup)
6. [Configuration & Tuning](#configuration--tuning)
7. [Documentation Index](#documentation-index)
8. [Credits](#credits)
9. [Conclusion](#conclusion)

---

## Project Overview

### Vision

Pluto is designed as a reusable quadruped robotics platform rather than a single-purpose demo. The main goal is to make four-legged locomotion understandable and testable across both real hardware and simulation: the same concepts of leg geometry, foot targets, inverse kinematics, gait phase offsets, and command messages appear throughout the ESP32 firmware and Python tools.

The robot-side code focuses on real-time actuation: it controls 12 servo channels through a PCA9685 driver, applies per-joint calibration limits, computes IK for each leg, and runs walk, trot, gallop, turn, stop, and stand behaviors. The computer-side code provides the operator interface: it can open a PyBullet simulation, read keyboard/gamepad inputs, recognize a small grammar of voice commands, and communicate with the ESP32 using the same packed message format as the firmware.

### How Pluto "Thinks"

Pluto is organized as three cooperating layers:

**Command Layer (Python control hub):**

1. **Select mode** - The NiceGUI home page opens either the PyBullet simulation or the controller page.
2. **Collect input** - Keyboard and gamepad input are normalized into a movement vector.
3. **Recognize speech** - Vosk listens for commands such as "pluto sit", "pluto stop", and "pluto give paw".
4. **Package commands** - Python packs movement, behavior, and sensor messages into the shared UDP protocol.
5. **Maintain connection** - The controller performs a session-token handshake, sends heartbeats, and stores received messages.

**Motion Layer (ESP32 firmware):**

1. **Receive** - A FreeRTOS network task accepts UDP packets, validates CRCs, checks sequence numbers, and queues messages.
2. **Interpret** - Movement messages, behavior messages, and sensor requests are dispatched in the main control loop.
3. **Generate gait** - The gait controller produces phase-shifted foot trajectories for walk, trot, gallop, and turn.
4. **Solve IK** - Desired foot targets are converted into coxa, femur, and tibia angles in millidegrees.
5. **Actuate** - Calibrated joint objects map logical angles to constrained PCA9685 PWM pulses.
6. **Sense** - Ultrasonic readings and I2S microphone energy are available for feedback and future reactive behavior.

**Simulation Layer (PyBullet):**

1. **Build model** - The simulator creates a quadruped body with coxa, femur, tibia, and foot links from STL meshes.
2. **Replay gaits** - Python gait definitions drive simulated leg joints using the same walk, trot, and gallop concepts.
3. **Test controls** - The UI can start/stop simulation and send directional motion commands before hardware tests.

### Technical Vocabulary

- **12-DOF**: Twelve degrees of freedom, with three actuated joints on each of four legs
- **Coxa, femur, tibia**: The hip-yaw, upper-leg, and lower-leg joints of each leg
- **DMS 15 270**: Servo model selected in the proposal for Pluto's leg actuation
- **PCA9685**: I2C PWM driver used to control the servo channels
- **Inverse kinematics (IK)**: Math that converts desired foot positions into joint angles
- **Gait**: A timed footstep pattern such as walk, trot, gallop, or turn
- **ESP32**: Microcontroller running the embedded firmware and WiFi server
- **FreeRTOS**: Task scheduler used by the ESP32 networking and sensor code
- **UDP packet**: Lightweight command envelope with CRC, session token, sequence number, timestamp, and up to 64 messages
- **PyBullet**: Physics simulator used for desktop motion testing
- **NiceGUI**: Python web UI framework used for the controller hub
- **Vosk**: Offline speech recognition engine used for voice commands

### Key Objectives

- **Reliable Servo Control**: Drive 12 joints through calibrated angle limits and safe PWM ranges
- **Legged Locomotion**: Implement stand, walk, trot, gallop, turn, and stop motion primitives
- **IK-Based Motion**: Generate joint commands from foot targets instead of hard-coded pulse sequences
- **Remote Operation**: Support WiFi commands from a Python controller using a compact shared protocol
- **Voice Interaction**: Map simple spoken commands to high-level robot behaviors
- **Simulation Before Hardware**: Use PyBullet to test gait timing and directional movement
- **Sensor Integration**: Provide ultrasonic distance and microphone support for reactive behavior

### Core Technologies

- **Embedded firmware**: C++17, Arduino framework, FreeRTOS, PlatformIO, ESP32 WiFi
- **Actuation**: Adafruit PCA9685 PWM servo driver with per-joint calibration
- **Motion algorithms**: Foot trajectory generation, phase offsets, inverse kinematics, millidegree angle mapping
- **Communication**: Shared C++/Python UDP protocol with CRC32, acknowledgements, session tokens, and sequence checks
- **Sensors**: HC-SR04-style ultrasonic distance sensing and INMP441-style I2S microphone input
- **Control UI**: Python, NiceGUI, keyboard/gamepad input handling, and Vosk speech recognition
- **Simulation**: PyBullet model using Pluto's body, coxa, femur, and tibia STL meshes

### System at a Glance

```text
+---------------------------------------------------------+
|  Python Control Hub (src/control)                       |
|  - NiceGUI home, controller, and simulation pages        |
|  - Keyboard/gamepad movement vector handling            |
|  - Vosk speech commands                                 |
|  - UDP packet packing and session management            |
+---------------------------------------------------------+
                         <-> WiFi / UDP
+---------------------------------------------------------+
|  ESP32 Robot Controller (src/esp)                       |
|  - FreeRTOS UDP server task                             |
|  - Walk/trot/gallop/turn gait controller                |
|  - IK solver and calibrated servo abstractions          |
|  - Ultrasonic and I2S microphone drivers                |
+---------------------------------------------------------+

+---------------------------------------------------------+
|  Shared Assets and Protocol                             |
|  - src/mesh STL body and leg parts                      |
|  - src/comm/message.h and message.py kept in sync       |
|  - PlatformIO firmware build and Python run scripts     |
+---------------------------------------------------------+
```

---

## Quick Start

### Prerequisites Checklist

**Control UI and simulation:**

- Python 3.13, as required by the install scripts
- A microphone if using speech commands
- A desktop environment that can open a PyBullet GUI window

**Firmware:**

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/index.html) or the PlatformIO VS Code extension
- ESP32 development board connected over USB
- Pluto hardware assembled with the PCA9685 and servos connected

### Run Pluto's Controller UI

For Windows, in the root project directory, type:

```cmd
run.bat
```

For macOS and Linux, in the root project directory, type:

```bash
bash run.sh
```

These scripts create a virtual environment, install dependencies from `requirements.txt`, and launch the NiceGUI app.

When launched through `run.sh` or `run.bat`, the application runs on port `8090` by default. If another process is already using that port, provide another one:

```bash
bash run.sh --port 8081
```

### Flash the ESP32 Firmware

```bash
pio run
pio run -t upload
```

If PlatformIO does not detect the upload port automatically:

```bash
pio device list
pio run -t upload --upload-port /dev/cu.usbserial-XXXX
```

For serial output and command testing:

```bash
pio device monitor
pio device monitor -b 115200
```

### What To Expect

- The web UI opens a home page with simulation and controller modes.
- Simulation mode can launch a PyBullet window and send forward/backward/turn commands.
- Controller mode reads keyboard/gamepad input and displays the movement vector.
- The ESP32 firmware can run serial commands for forward, backward, stop, walk, trot, gallop, speed changes, and servo trimming.
- Speech commands are recognized on the computer and mapped to behavior or stop messages when the controller is connected.

---

## Hardware

### Component Overview

The proposal and current codebase are built around a compact 12-servo quadruped architecture:

| Component | Role | Status |
|-----------|------|--------|
| 3D-printed body and legs | Mechanical structure for the quadruped | Meshes are included in `src/mesh` |
| DMS 15 270 servos x12 | Three actuated joints per leg | Supported by servo calibration code |
| PCA9685 PWM driver | Generates stable PWM for the servos over I2C | Used by ESP32 firmware |
| ESP32-DEVKITM-1 / ESP32 dev board | Main embedded controller | PlatformIO target is `esp32dev` |
| HC-SR04 ultrasonic sensor | Distance sensing for obstacle feedback | Driver implemented |
| INMP441-style I2S microphone | Audio input / energy measurement | Driver implemented |
| 7.4V 2S LiPo battery | Servo and electronics power source | Proposal hardware |
| LM2596 buck converter | Regulated low-voltage supply | Proposal hardware |
| ESP32-CAM / IMU | Future perception and stability extensions | Proposal/future work |

### Mechanical Concept

Pluto uses four symmetric legs attached to a central body. Each leg is modeled as a three-joint chain:

1. **Coxa**: hip yaw / lateral leg placement
2. **Femur**: upper-leg pitch
3. **Tibia**: lower-leg pitch

The proposal emphasizes keeping mass near the body, maintaining a centered center of mass, and using modular 3D-printed parts so legs and mounts can be modified without redesigning the whole robot.

### Power and Safety Notes

The proposal estimates a robot mass around 1.5-2.3 kg and highlights that multiple loaded servos can draw high current. The power system therefore needs careful wiring, secure connectors, and a regulator sized for the electronics. Battery safety is part of the ongoing documentation work in [ONGOING_WORK.md](ONGOING_WORK.md).

---

## System Architecture

### Repository Structure

```text
2026sp-pluto/
|-- README.md
|-- ONGOING_WORK.md
|-- platformio.ini
|-- requirements.txt
|-- run.sh / run.bat
|-- scripts/
|   |-- install.sh
|   `-- install.bat
`-- src/
    |-- comm/
    |   |-- message.h
    |   `-- README.md
    |-- control/
    |   |-- main.py
    |   |-- gait.py
    |   |-- ik_solver.py
    |   |-- sim_motion.py
    |   |-- pluto_input/
    |   |-- pluto_menu/
    |   |-- pluto_server/
    |   `-- pluto_speech/
    |-- esp/
    |   |-- main.cpp
    |   |-- legs/
    |   |-- motion/
    |   |-- sensors/
    |   `-- server/
    `-- mesh/
        |-- body.stl
        |-- coxa.stl
        |-- Femur.stl
        `-- tibia.stl
```

### Python Control Hub

The Python side is responsible for user interaction and desktop simulation.

**Main responsibilities:**

- Serve the NiceGUI app from `src/control/main.py`
- Provide navigation between home, simulation, and controller pages
- Read keyboard and gamepad input through `InputManager`
- Start and stop the PyBullet process through `PyBulletMotionController`
- Recognize a small Vosk grammar: `pluto sit`, `pluto stop`, and `pluto give paw`
- Pack and unpack UDP messages in sync with the C++ firmware protocol

**Important modules:**

- `src/control/main.py`: app entry point
- `src/control/pluto_menu/simulation.py`: PyBullet UI controls
- `src/control/pluto_menu/controller.py`: keyboard/gamepad controller page
- `src/control/pluto_input/input_manager.py`: normalized movement vectors
- `src/control/pluto_speech/speech.py`: Vosk speech worker
- `src/control/pluto_server/message.py`: Python packet/message definitions
- `src/control/pluto_server/server.py`: UDP controller client

### ESP32 Firmware

The ESP32 side is responsible for real-time robot control.

**Main responsibilities:**

- Initialize WiFi, PCA9685, ultrasonic sensing, microphone sensing, and gait state
- Drive all 12 joints through calibrated `LegJoint` objects
- Convert IK outputs into constrained PWM pulses
- Update gait motion at a regular loop interval
- Accept UDP commands through a FreeRTOS network task
- Queue inbound and outbound messages without blocking the motion loop

**Important modules:**

- `src/esp/main.cpp`: firmware entry point and top-level message dispatch
- `src/esp/legs/leg_data.h`: per-joint raw PWM and angle calibration
- `src/esp/legs/leg_joint.h`: calibrated servo output abstraction
- `src/esp/legs/leg.h`: 3-joint leg abstraction
- `src/esp/motion/ik_solver.cpp`: embedded inverse kinematics
- `src/esp/motion/gait.cpp`: walk, trot, gallop, and turn gait generation
- `src/esp/server/server.cpp`: UDP session, CRC, acknowledgement, and queues
- `src/esp/sensors/ultrasonic.h`: interrupt-based ultrasonic distance readings
- `src/esp/sensors/microphone.h`: I2S microphone sampling and energy measurement

### Shared Communication Protocol

The protocol is intentionally small so it can run on an embedded target and be mirrored in Python.

**Packet envelope:**

- `crc32`: validates the packet contents
- `session_token`: distinguishes active controller sessions
- `sequence_number`: rejects old or repeated packets
- `timestamp`: sender clock in milliseconds
- `message_count`: number of valid messages
- `messages`: up to 64 fixed-size messages

**Message families:**

| Family | Purpose |
|--------|---------|
| `KIND_INFO` | Heartbeats, sensor requests, acknowledgements |
| `KIND_MOVE` | Control leases, stop commands, movement vectors |
| `KIND_SENSOR` | Distance and microphone-related feedback |
| `KIND_BEHAVIOR` | High-level behaviors such as sit, give paw, and lie down |

The C++ definition is in `src/comm/message.h`; the Python mirror is in `src/control/pluto_server/message.py`. These files should stay synchronized.

---

## Software Setup

### Python Environment

The run scripts manage the Python environment automatically:

```bash
bash run.sh
```

Manual setup, if needed:

```bash
python3.13 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python src/control/main.py --port 8090
```

Python dependencies:

- `nicegui`
- `numpy`
- `debugpy`
- `pybullet`
- `vosk`
- `sounddevice`

### PlatformIO Firmware Environment

The project uses PlatformIO with this environment:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
build_flags = -std=gnu++17
```

Firmware dependencies:

- Adafruit PWM Servo Driver Library
- Adafruit BusIO

### Simulation Mode

Simulation mode starts a separate PyBullet process. The simulated quadruped is built from the STL meshes in `src/mesh` and uses the Python gait definitions in `src/control/gait.py`. The simulation is useful for checking gait timing and directional behavior before testing on hardware, but it is not yet a full-fidelity model of the physical robot.

### Controller Mode

Controller mode currently reads keyboard and gamepad inputs and exposes the normalized movement vector. Full live wiring from the UI movement vector to physical `MOVE_BY` UDP commands is listed as ongoing work.

---

## Configuration & Tuning

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

These values define how an IK angle maps to a safe PCA9685 PWM pulse. They must be tuned on the real robot before aggressive gait testing.

### Gait Parameters

Embedded gait constants live in:

```text
src/esp/motion/gait.cpp
```

Important values include:

- `COXA_LENGTH`, `FEMUR_LENGTH`, `TIBIA_LENGTH`
- `FOOT_Z_STAND`
- `SWING_RATIO`
- `STRIDE`
- `LIFT`
- `FOOT_Y_STANCE`
- `WALK_BALANCE_SHIFT_Y`
- `WALK_SUPPORT_PUSH_DOWN`

Python simulation gait parameters live in:

```text
src/control/gait.py
src/control/robot_config.py
```

These values should be kept conceptually aligned with the physical robot, even though simulation and firmware use different units and tuning contexts.

### WiFi and UDP

The ESP32 UDP server listens on port `4242`:

```cpp
auto PLUTO_SERVER = pluto::PlutoServer{4242};
```

WiFi access points are configured in:

```text
src/esp/main.cpp
```

The Python controller targets an ESP32 IP address through `PlutoController`. The current `main.py` leaves `IP_OF_ESP` empty, so final hardware operation requires setting the robot IP or exposing it through the UI.

### Sensor Pins

Current firmware pin templates:

- Ultrasonic sensor: `SensorUltraSonic<21, 22>`
- Microphone: `SensorMicrophone<26, 25, 33>`

Verify these against the actual wiring before flashing.

---

## Documentation Index

### Main Documents

- [Main README](README.md)
- [Ongoing Works & Next Steps](ONGOING_WORK.md)
- Project proposal: `../Project Proposal.pdf`

### Subsystem Documents

- [Control README](src/control/README.md)
- [ESP README](src/esp/README.md)
- [Communication README](src/comm/README.md)

### Key Source Files

- [Python UI entry point](src/control/main.py)
- [Simulation controller](src/control/sim_motion.py)
- [Python gait definitions](src/control/gait.py)
- [Python communication protocol](src/control/pluto_server/message.py)
- [ESP firmware entry point](src/esp/main.cpp)
- [ESP gait controller](src/esp/motion/gait.cpp)
- [ESP IK solver](src/esp/motion/ik_solver.cpp)
- [Shared C++ message format](src/comm/message.h)

---

## Credits

### Project Team

Pluto was developed by Serhat Botan, Alexis Cazal, Neha Chakraborty, Myriam Lahoud, Sam Lee, Raphael Dib Nehme, and Mariya Rakytyanska as part of EPFL's Making Intelligent Things course.

### Technical Inspiration

- SpotMicro and other open-source quadruped robot projects
- Quadruped robotics tutorials and demonstrations used as design references
- PyBullet robot simulation examples and community projects

### Technologies and Libraries

- Arduino and PlatformIO for embedded development
- FreeRTOS for task scheduling on the ESP32
- Adafruit PCA9685 library for servo output
- NiceGUI for the Python control interface
- PyBullet for simulation
- Vosk and sounddevice for speech input

---

## Conclusion

Pluto demonstrates a practical path from a 3D-printed quadruped concept to a working robotics software platform. The project already includes the key layers needed for legged robotics development: calibrated low-level actuation, inverse kinematics, gait generation, simulation, command transport, and early sensing/interaction hooks.

The current system is not the final version of the robot. It is a foundation for continued tuning and experimentation, especially around gait stability, behavior execution, sensor-driven reactions, and higher-level autonomy. Its modular structure is intended to make those next steps approachable for future development.
