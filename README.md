# Pluto

> A four-legged canine robot with ESP32-based gait control, calibrated 12-servo actuation, WiFi command transport, voice-triggered behaviors, and a PyBullet simulation/control hub.
_Photo to be added later_

**Live Demo**

_Demo media can be added here once the final robot videos are available._

## Project Context

**Abstract**

Pluto is a 12-degree-of-freedom quadruped robot developed around the idea of making a compact canine-style robot that can move, react, and be controlled interactively. Each of Pluto's four legs has three joints: coxa, femur, and tibia. The project combines mechanical design, embedded motor control, inverse kinematics, gait generation, wireless communication, sensor feedback, and a Python control interface.

The current repository implements the core platform for that goal: ESP32 firmware for servo control and gait execution, a shared UDP packet protocol, ultrasonic and microphone sensor drivers, a NiceGUI controller UI, Vosk-based speech command handling, and a PyBullet simulator using Pluto's STL meshes.

**Course Context**

This project was developed as part of the **Making Intelligent Things** course at **EPFL**.

**Acknowledgments**

- Course staff, TAs, and coaches for guidance throughout the design and development process
- Existing open-source quadruped projects, including SpotMicro and related quadruped robotics tutorials, for inspiration on legged locomotion and mechanical structure

**Note on Continued Development**

Although the CS-358 course has concluded, SLAMaleykoum remains under active development. For current work-in-progress features, planned enhancements, and future research directions, see [Ongoing Works & Next Steps](ONGOING_WORK.md).

---

## Quick Jump To Detailed Documentation

<table>
<tr>
<td width="25%" align="center">

### [Hardware & Assembly](ASSEMBLY.md)

3D-printed body and leg assembly, circuit wiring, and final hardware integration

</td>
<td width="25%" align="center">

### [Software Overview](SOFTWARE_OVERVIEW.md)

Main implemented software layers and links to each component

</td>
<td width="25%" align="center">

### [Controller UI](src/control/README.md)

Python control hub, NiceGUI pages, PyBullet simulation, speech command worker

</td>
<td width="25%" align="center">

### [Next Steps](ONGOING_WORK.md)

Active work, planned enhancements, and future research directions

</td>
</tr>
<tr>
<td width="25%" align="center">

### [ESP32 Firmware](src/esp/README.md)

Robot-side motion control, servo output, sensors, WiFi UDP server

</td>
<td width="25%" align="center">

### [Communication](src/comm/README.md)

Shared packet/message format used by Python and C++

</td>
<td width="25%" align="center">

### [Sensors](SOFTWARE_SENSORS.md)

Ultrasonic distance sensing and INMP441 microphone notes

</td>
<td width="25%" align="center">

### [WiFi Protocol](SOFTWARE_WIFI.md)

UDP connection setup, packet structure, batching, and payload layout

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
8. [Archives](#archives)
9. [Development & Debugging Tools](#development--debugging-tools)
10. [Ongoing Works & Next Steps](#ongoing-works--next-steps)
11. [Credits](#credits)
12. [Conclusion](#conclusion)

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
- **DMS 15 270**: Servo model used for Pluto's leg actuation
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

### 1. Check Prerequisites

**Control UI and simulation:**

- Python 3.13, as required by the install scripts
- A desktop environment that can open a PyBullet GUI window

**Firmware:**

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/index.html) or the PlatformIO VS Code extension
- ESP32 development board connected over USB
- Pluto hardware assembled with the PCA9685 and servos connected

For hardware assembly and wiring, see [Hardware & Assembly](ASSEMBLY.md). For sensor notes, see [Sensors](SOFTWARE_SENSORS.md).

### 2. Run Pluto's Controller UI

If you are using the physical robot, power it up before opening the controller:

1. Check the wiring against [Hardware & Assembly](ASSEMBLY.md), especially the LiPo, buck converter, PCA9685, ESP32, and servo power lines.
2. Connect the ESP32 over USB so it can be flashed, monitored, or powered during setup.
3. Connect the charged 7.4V LiPo battery through the XT60 connector.
4. Turn on the KCD1 rocker switch and confirm that the ESP32, PCA9685, sensors, and servos power up normally.
5. Make sure the computer and ESP32 are on the same WiFi network configured in `src/esp/main.cpp`.

For software-only use, you can run the controller UI and PyBullet simulation without powering the robot hardware.

For Windows, in the root project directory, type:

```cmd
run.bat
```

For macOS and Linux, in the root project directory, type:

```bash
bash run.sh
```

The run scripts call the installer, create or reuse `.venv`, install `requirements.txt`, and launch the NiceGUI app. Installation is skipped automatically when `requirements.txt` has not changed.

To force a clean dependency reinstall:

```bash
bash run.sh --reinstall
```

```cmd
run.bat --reinstall
```

When launched through `run.sh` or `run.bat`, the application runs on port `8090` by default. If another process is already using that port, provide another one:

```bash
bash run.sh --port 8081
```

```cmd
run.bat --port 8081
```

Then open:

```text
http://localhost:8090
```

### 3. Flash the ESP32 Firmware

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

### 4. Configure WiFi Control

The ESP32 firmware and Python controller communicate over UDP on port `4242`.

1. Add one or more WiFi networks in `src/esp/main.cpp` using `PLUTO_SERVER.addAP(<WIFI_NAME>, <WIFI_PASSWORD>)`.
2. Set the ESP32 IP address in `src/control/main.py` where `PlutoController(IP_OF_ESP)` is constructed.
3. Keep the Python and C++ message definitions in sync through [Communication](src/comm/README.md).

For packet format and binary message details, see [WiFi Protocol](SOFTWARE_WIFI.md).

### 5. What To Expect

- The web UI opens a home page with simulation and controller modes.
- Simulation mode can launch a PyBullet window and send forward/backward/turn commands.
- Controller mode reads keyboard/gamepad input and displays the movement vector.
- The ESP32 firmware can run serial commands for forward, backward, stop, walk, trot, gallop, speed changes, and servo trimming.
- Speech commands are recognized on the computer and mapped to behavior or stop messages when the controller is connected.

First-time setup problems are collected in [First-Time Troubleshooting](TROUBLESHOOTING.md).

---

## Hardware

### Component Overview

Pluto is built around a compact 12-servo quadruped architecture. Reference links are included for the main parts or equivalent modules.

| Component | Qty | Role | Status | Reference |
|-----------|----:|------|--------|-----------|
| 3D-printed body, coxa, femur, and tibia parts | 1 set | Mechanical structure for the quadruped | Meshes are included in `src/mesh` | [Mesh files](src/mesh), [Assembly](ASSEMBLY.md) |
| DMS15 270-degree servos | 12 | Three actuated joints per leg | Supported by servo calibration code | [DMS15 servo reference](https://www.aideepen.com/products/dms15-15kg-digital-servo-180-angel-degrees-270-angel-degrees-rotation-servo-for-robot-flight-control-rc-helicopter) |
| Servo horns / servo drivers | 12 | Mechanical connection between servos and printed links | Required for leg assembly | Usually included with servos; see [Assembly](ASSEMBLY.md) |
| PCA9685 16-channel PWM driver | 1 | Generates stable PWM for all servos over I2C | Used by ESP32 firmware | [Adafruit PCA9685 guide](https://learn.adafruit.com/16-channel-pwm-servo-driver?view=all) |
| ESP32-DEVKITM-1 / ESP32 dev board | 1 | Main embedded controller for firmware, WiFi, sensors, and motion control | PlatformIO target is `esp32dev` | [Espressif ESP32-DevKitM-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/index.html) |
| HC-SR04 ultrasonic sensor | 1-2 | Front obstacle distance sensing | Driver implemented | [HC-SR04 reference](https://www.iemrobotics.com/products/ultrasonic-distance-sensor-module-hc-sr04) |
| INMP441 I2S microphone module | 1 | Audio input for speech/energy sensing | Driver implemented | [INMP441 datasheet](https://product.tdk.com/system/files/dam/doc/product/sw_piezo/mic/mems-mic/data_sheet/inmp441.pdf) |
| 7.4V 2S LiPo battery with XT60 | 1 | Servo and electronics power source | Hardware design | [Swaytronic 2S LiPo XT60 reference](https://www.swaytronic.ch/en/SWAYTRONIC-LiPo-2S-7.4V-2200mAh-60C-120C-XT60) |
| LM2596 DC-DC buck converter | 1 | Steps battery voltage down for ESP32 and low-voltage electronics | Hardware design | [LM2596 module reference](https://www.beemong.com/product/lm2596-lm2576-dc-dc-buck-converter-module-12v-24v-to-5v-step-down-power-supply-3a-max-output-voltage-regulator-board) |
| XT60 connectors | 2 | High-current battery connection | Required for power wiring | [XT60 connector reference](https://www.sparkfun.com/products/10474) |
| KCD1 rocker switch | 1 | Main power switch | Required for safe power control | [KCD1 switch reference](https://www.phaseda.com/switch/KCD1.htm) |
| M3 screws, M2.5 screws, bearings, linkage hardware, wiring, and heat-shrink | As needed | Mechanical fastening and electrical assembly | Required for final build | See [Assembly](ASSEMBLY.md) |
| TPU feet / rubber pads | 4 | Foot traction and impact reduction | Recommended | See [Assembly](ASSEMBLY.md) |
| ESP32-CAM | 1 optional | Future camera streaming and vision-based features | Future work | [ESP32-CAM reference](https://www.olimex.com/Products/IoT/ESP32/ESP32-CAM/) |
| MPU6050 IMU | 1 optional | Future orientation and acceleration feedback | Future work | [MPU6050 product reference](https://www.adafruit.com/product/3886) |

### Additional Materials Needed

These are not part of the main electronics list, but they are needed for assembly, wiring, and testing:

- PLA/PETG filament for rigid printed body and leg parts
- TPU filament or rubber pads for feet
- M3 screws for most printed-part connections
- M2.5 screws for coxa servo mounting
- M3x20 and M3x35 screws for leg assembly
- Ball bearings for tibia joints
- Servo horns / servo arms compatible with the selected servos
- Jumper wires, thicker power wires for servo current, and I2C wiring
- Solder, heat-shrink tubing, electrical tape, and cable ties
- Velcro or mounting tape for the LiPo battery
- Multimeter for checking polarity, buck-converter output, and continuity
- LiPo-safe charger and LiPo-safe storage bag
- USB cable for flashing and monitoring the ESP32

### Assembly Overview

[Full step-by-step assembly guide here](ASSEMBLY.md)

Pluto is built around a custom 3D-printed quadruped body with four modular legs. Each leg has three servo-driven joints: coxa, femur, and tibia. Assembly is done by printing the CAD parts, building each leg, mounting the servos and electronics in the body, wiring the power and sensor systems, and then testing basic firmware commands before attempting full gaits.

Assembly phases:

- **3D-printed parts** - Print the body, coxa, femur, tibia, linkage, and sensor/battery mounting parts
- **Leg assembly** - Build each coxa/femur/tibia chain with servos, horns, screws, bearings, and TPU/rubber feet
- **Body assembly** - Mount coxa servos, PCA9685, buck converter, ESP32, ultrasonic sensor, microphone, and LiPo
- **Wiring** - Follow the circuit diagram, connect power rails, I2C, servo channels, and sensor lines
- **Final checks** - Route cables, verify polarity and voltage, flash firmware, and test simple movements

[Complete Assembly Instructions](ASSEMBLY.md)

### Wiring & Electrical

[Full wiring explanation here](WIRING_ELECTRICAL.md)

Electrical diagram:

![Electrical Circuit](images/circuit.png)

**Important Wiring Notes**

- **Common ground**: The LiPo/servo ground, PCA9685 ground, ESP32 ground, buck converter ground, and sensor grounds must be connected together.
- **Servo power vs logic power**: The servos draw much higher current than the ESP32 and sensors. Use the LiPo/power rail for servo power and the buck converter for low-voltage electronics.
- **Buck converter check**: Adjust and measure the LM2596 output with a multimeter before connecting it to the ESP32 or sensors.
- **PCA9685 wiring**: Connect SDA/SCL between the ESP32 and PCA9685, and make sure the PCA9685 has both logic power and servo power.
- **Ultrasonic sensor**: Check the HC-SR04 voltage level before connecting ECHO to an ESP32 GPIO. If the module outputs 5V, use a voltage divider or level shifter.
- **High-current wiring**: Use thick wires for the LiPo, XT60, rocker switch, servo power rail, and PCA9685 servo power input.

[Detailed Wiring & Soldering Guide](WIRING_ELECTRICAL.md)

### CAD Files

You can explore the available STL files directly in [src/mesh](src/mesh).

Current CAD/mesh files:

- [body.stl](src/mesh/body.stl)
- [coxa.stl](src/mesh/coxa.stl)
- [Femur.stl](src/mesh/Femur.stl)
- [tibia.stl](src/mesh/tibia.stl)

These files are used for both 3D printing and the PyBullet simulation model.

[CAD files overview](CAD_FILES.md)

Before implementing hardware changes, check [Ongoing Works & Next Steps](ONGOING_WORK.md) for known hardware limitations and recommended improvements.

### Mechanical Concept

Pluto uses four symmetric legs attached to a central body. Each leg is modeled as a three-joint chain:

1. **Coxa**: hip yaw / lateral leg placement
2. **Femur**: upper-leg pitch
3. **Tibia**: lower-leg pitch

The mechanical design keeps mass near the body, maintains a centered center of mass, and uses modular 3D-printed parts so legs and mounts can be modified without redesigning the whole robot.

### Power and Safety Notes

The robot mass is estimated around 1.5-2.3 kg, and multiple loaded servos can draw high current. The power system therefore needs careful wiring, secure connectors, and a regulator sized for the electronics. 

---

## System Architecture

The computational load is divided between the physical ESP32 robot controller and the Python control hub running on the user's computer.

- **ESP32 Robot Controller**: Low-level motion control, servo actuation, sensor reads, UDP packet handling, and behavior execution
- **Python Control Hub**: User interface, keyboard/gamepad input, speech commands, PyBullet simulation, and UDP command generation
- **Shared Communication Layer**: Compact packet/message protocol mirrored in C++ and Python

The ESP32 uses FreeRTOS for the UDP network task while the main loop keeps motion updates, serial commands, sensor polling, and message dispatch moving. The Python side uses NiceGUI for the web UI, background threads for speech and UDP receive/heartbeat work, and a separate PyBullet process for simulation.

**Rationale:**

- **Real-time control**: Servo updates and gait generation stay on the ESP32 close to the hardware.
- **Usability**: The computer handles heavier UI, speech, and simulation work.
- **Modularity**: Motion, communication, sensing, input, and simulation are split into focused modules.
- **Safety**: The firmware can stop or stand the robot even if the desktop controller disconnects.
- **Reuse**: The shared packet format keeps Python and C++ behavior aligned.

Detailed component documentation:

- [Controller UI documentation](src/control/README.md)
- [ESP32 firmware documentation](src/esp/README.md)
- [Communication protocol documentation](src/comm/README.md)
- [Software overview](SOFTWARE_OVERVIEW.md)

### Architecture Diagram

```text
+----------------------------------------------------------+
| Python Control Hub (computer)                            |
| src/control/main.py                                      |
|                                                          |
| - NiceGUI home, simulation, and controller pages          |
| - Keyboard/gamepad movement input                        |
| - Vosk speech commands                                   |
| - PyBullet simulation process                            |
| - UDP session, heartbeat, packet packing/unpacking        |
+-----------------------------+----------------------------+
                              |
                              | WiFi / UDP port 4242
                              v
+----------------------------------------------------------+
| ESP32 Robot Controller                                   |
| src/esp/main.cpp                                         |
|                                                          |
| - 20 ms gait/motion update loop                          |
| - PCA9685 12-servo output                                |
| - Inverse kinematics and gait generation                  |
| - Ultrasonic and microphone sensor hooks                  |
| - FreeRTOS UDP server task and message queues             |
+-----------------------------+----------------------------+
                              |
                              v
+----------------------------------------------------------+
| Hardware Layer                                            |
|                                                          |
| - Four 3-DOF legs: coxa, femur, tibia                     |
| - DMS15 servos, PCA9685, ESP32, LiPo power                |
| - HC-SR04 ultrasonic sensor and INMP441 microphone        |
+----------------------------------------------------------+
```

### ESP32 Robot Controller

[Full ESP32 firmware documentation](src/esp/README.md)

**Primary mission:** execute reliable low-level robot control on the physical quadruped.

**Core responsibilities:**

- Initialize WiFi, PCA9685, ultrasonic sensing, microphone sensing, and gait state
- Drive all 12 joints through calibrated `LegJoint` objects
- Convert IK outputs into constrained PWM pulses
- Update gait motion every 20 ms in the main firmware loop
- Accept UDP commands through a FreeRTOS network task
- Queue inbound and outbound messages without blocking motion control
- Handle serial commands for early testing, gait selection, speed changes, and servo trimming

**Key components:**

- `src/esp/main.cpp`: firmware entry point and top-level message dispatch
- `src/esp/legs/leg_data.h`: per-joint raw PWM and angle calibration
- `src/esp/legs/leg_joint.h`: calibrated servo output abstraction
- `src/esp/legs/leg.h`: 3-joint leg abstraction
- `src/esp/motion/ik_solver.cpp`: embedded inverse kinematics
- `src/esp/motion/gait.cpp`: walk, trot, gallop, and turn gait generation
- `src/esp/server/server.cpp`: UDP session, CRC, acknowledgement, and queues
- `src/esp/sensors/ultrasonic.h`: interrupt-based ultrasonic distance readings
- `src/esp/sensors/microphone.h`: I2S microphone sampling and energy measurement

**Operating frequency:**

- Motion/gait update: 50 Hz, every 20 ms
- Ultrasonic polling template: every 500 ms
- UDP server task delay: 5 ms between network iterations
- Serial monitor baud rate: 115200

### Python Control Hub

[Full Controller UI documentation](src/control/README.md)

**Primary mission:** provide the operator-facing interface, simulation mode, speech commands, and UDP command transport.

**Core responsibilities:**

- Serve the NiceGUI app from `src/control/main.py`
- Provide navigation between home, simulation, and controller pages
- Read keyboard and gamepad input through `InputManager`
- Start and stop the PyBullet process through `PyBulletMotionController`
- Recognize supported Vosk commands such as `pluto sit`, `pluto stop`, and `pluto give paw`
- Maintain a UDP session with the ESP32 using heartbeats and background receive handling
- Pack and unpack messages in sync with the C++ firmware protocol

**Key components:**

- `src/control/main.py`: app entry point
- `src/control/pluto_menu/simulation.py`: PyBullet UI controls
- `src/control/pluto_menu/controller.py`: keyboard/gamepad controller page
- `src/control/pluto_input/input_manager.py`: normalized movement vectors
- `src/control/pluto_speech/speech.py`: Vosk speech worker
- `src/control/pluto_server/message.py`: Python packet/message definitions
- `src/control/pluto_server/server.py`: UDP controller client
- `src/control/gait.py`: Python gait definitions for simulation
- `src/control/sim_motion.py`: PyBullet simulation process control

### Simulation Layer

**Primary mission:** allow gait and movement experiments before running commands on physical hardware.

**Core responsibilities:**

- Load Pluto's STL meshes from `src/mesh`
- Build a quadruped model in PyBullet
- Replay Python gait definitions
- Test basic forward, backward, turning, and stop commands from the UI

**Current limitation:** the simulator is useful for control experiments, but it is not yet a full-fidelity model of the physical robot's mass, friction, joint limits, or servo dynamics.

### Communication Architecture

The Python controller and ESP32 firmware communicate over WiFi using UDP on port `4242`.

[Full communication protocol documentation](src/comm/README.md)

**Protocol:** custom packet-based binary protocol with CRC validation, session tokens, sequence numbers, timestamps, and fixed-size messages.

**Data flow:**

| Direction | Message Type | Content |
|-----------|--------------|---------|
| Python -> ESP32 | `MOVE_BY` | Direction vector for movement control |
| Python -> ESP32 | `MOVE_STOP_FOR` | Stop/stand command with duration semantics |
| Python -> ESP32 | `BEHAVIOR_*` | High-level commands such as sit, give paw, and lie down |
| Python -> ESP32 | `INFO_REQUEST_*` | Heartbeat and sensor requests |
| ESP32 -> Python | `INFO_ACKNOWLEDGE` | Packet acknowledgement |
| ESP32 -> Python | `SENSOR_DISTANCE` | Ultrasonic distance reading |

**Shared definitions:**

- `src/comm/message.h`: C++ packet and message definitions
- `src/control/pluto_server/message.py`: Python mirror of the same protocol
- `SOFTWARE_WIFI.md`: WiFi setup and binary packet notes

### Shared Mechanisms

- **Session management**: Python requests a session token; ESP32 rejects stale or invalid packets.
- **CRC checks**: Packets are validated before messages are queued.
- **Sequence numbers**: Repeated or old packets are ignored by the firmware.
- **Message queues**: The ESP32 network task queues inbound and outbound messages for the main control loop.
- **Heartbeats**: Python sends periodic keep-alive messages to prevent session timeout.
- **Compile-time feature flags**: `PLUTO_ENABLE_WIFI`, `PLUTO_ENABLE_ULTRASONIC`, and `PLUTO_ENABLE_MICROPHONE` enable major firmware features.

### Repository Structure

```text
2026sp-pluto/
|-- ASSEMBLY.md
|-- CAD_FILES.md
|-- HARDWARE_OVERVIEW.md
|-- README.md
|-- ONGOING_WORK.md
|-- SOFTWARE_OVERVIEW.md
|-- SOFTWARE_SENSORS.md
|-- SOFTWARE_WIFI.md
|-- TROUBLESHOOTING.md
|-- WIRING_ELECTRICAL.md
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

### Core System Documentation

**Main System:**

- [Main README](README.md) - This document
- [Software Overview](SOFTWARE_OVERVIEW.md) - High-level map of the implemented software
- [Archives](ARCHIVES.md) - Notes for preserving previous prototypes and tests
- [Ongoing Works & Next Steps](ONGOING_WORK.md) - Active work, limitations, planned enhancements, and future research directions

**Robot Subsystems:**

- [ESP32 Firmware](src/esp/README.md) - Robot-side motion control, servo output, sensors, and UDP server
- [Controller UI](src/control/README.md) - Python control hub, NiceGUI pages, simulation, and speech command worker
- [Communication Protocol](src/comm/README.md) - Shared message format used by C++ and Python

### Software Documentation

- [ESP32 Firmware](src/esp/README.md)
- [Controller UI](src/control/README.md)
- [Communication Protocol](src/comm/README.md)
- [WiFi Protocol](SOFTWARE_WIFI.md)
- [Software Sensors](SOFTWARE_SENSORS.md)

### Hardware Documentation

**Assembly & Build:**

- [Hardware Overview](HARDWARE_OVERVIEW.md)
- [Complete Assembly Guide](ASSEMBLY.md)
- [Wiring & Circuit Details](WIRING_ELECTRICAL.md)
- [CAD Files](CAD_FILES.md)

### Configuration & Setup

- [Quick Start](#quick-start)
- [Software Setup](#software-setup)
- [Configuration & Tuning](#configuration--tuning)
- [WiFi Protocol Setup](SOFTWARE_WIFI.md)

### Debugging & Development Tools

- [First-Time Troubleshooting Guide](TROUBLESHOOTING.md)
- [Archives](ARCHIVES.md)
- [PlatformIO Firmware Environment](#platformio-firmware-environment)
- [Servo Calibration](#servo-calibration)
- [Sensor Pins](#sensor-pins)

### Shared Utilities And Key Source Files

- [Python UI entry point](src/control/main.py)
- [Simulation controller](src/control/sim_motion.py)
- [Python gait definitions](src/control/gait.py)
- [Python communication protocol](src/control/pluto_server/message.py)
- [ESP firmware entry point](src/esp/main.cpp)
- [ESP gait controller](src/esp/motion/gait.cpp)
- [ESP IK solver](src/esp/motion/ik_solver.cpp)
- [Shared C++ message format](src/comm/message.h)

---

## Archives

The purpose of an archive area is to save previous work and keep track of tests that are no longer part of the active implementation.

This repository does not currently include a dedicated `archives/` directory. Earlier work is mainly preserved through Git history, current source files, and the notes in [Ongoing Works & Next Steps](ONGOING_WORK.md). If older prototypes are added back for reference, they should be placed under an `archives/` directory with a short README explaining what was tested, why it was replaced, and whether it still runs.

Recommended archive categories:

- **ESP32 experiments** - old gait tests, servo calibration sketches, sensor bring-up code, and behavior prototypes
- **Control UI experiments** - early NiceGUI pages, keyboard/gamepad tests, and speech command prototypes
- **Simulation experiments** - PyBullet model tests, gait playback experiments, and old mesh-loading attempts
- **Communication experiments** - older UDP packet formats, handshake tests, and protocol debugging scripts

Some archived tests may stop compiling as message definitions, include paths, or hardware assumptions evolve. Keep them for reference, but treat the active implementation in `src/esp`, `src/control`, and `src/comm` as the source of truth.

See [Archives](ARCHIVES.md) for the suggested archive structure.

---

## Development & Debugging Tools

During development, Pluto uses lightweight debugging tools rather than a separate profiling framework.

Current tools and practices:

- **Serial monitor**: Used for firmware startup logs, gait selection, speed changes, and servo trimming through `pio device monitor -b 115200`.
- **PlatformIO builds and uploads**: Used to compile and flash the ESP32 firmware.
- **NiceGUI controller UI**: Used to inspect controller mode, simulation mode, and input state.
- **PyBullet simulation**: Used to test gait timing and directional behavior before hardware trials.
- **UDP acknowledgements and heartbeats**: Used to verify controller-to-robot communication.
- **Troubleshooting guide**: Documents first-time setup failures, WiFi issues, power problems, and sensor checks.

Planned debugging improvements:

- Add a telemetry panel for connection state, latest distance reading, gait mode, speed, and acknowledgements.
- Add more structured serial logs around behavior execution and UDP message handling.
- Add repeatable hardware-in-the-loop tests for servo calibration, gait safety, and sensor readings.
- Add profiling or timing logs if gait updates or network handling become unreliable.

See [First-Time Troubleshooting](TROUBLESHOOTING.md) for setup and debugging steps.

---

## Ongoing Works & Next Steps

Pluto continues to evolve beyond the CS-358 course timeline. The current codebase provides the main building blocks for locomotion, simulation, sensing, and communication, but several parts still need physical tuning and integration.

Current focus:

- **Hardware gait validation**: Test walk, trot, gallop, and turn gaits on the physical robot.
- **Servo calibration**: Refine PWM limits, starting pulses, inversion flags, and angle ranges for each joint.
- **Controller-to-robot integration**: Finish wiring keyboard/gamepad movement vectors to live `MOVE_BY` UDP messages.
- **Behavior implementation**: Replace placeholder sit, give paw, and lie down handlers with calibrated motion sequences.
- **Sensor-driven reactions**: Use ultrasonic readings to stop, slow down, or recover near obstacles.

Known issues to address:

- Some ESP32 behavior handlers are still placeholders.
- Movement vectors are computed in the UI but still need full physical robot integration.
- Gait constants need final physical measurement and tuning.
- The PyBullet simulator is not yet a high-fidelity model of the real robot.
- Camera, IMU stabilization, and higher-level autonomy remain future work.

See [Ongoing Works & Next Steps](ONGOING_WORK.md) for the full list.

---

## Credits

### Project Team

Pluto was developed by Serhat Botan, Alexis Cazal, Neha Chakraborty, Myriam Lahoud, Sam Lee, Raphael Dib Nehme, and Mariya Rakytyanska as part of EPFL's Making Intelligent Things course.

### Use of AI Tools in Development

This project documentation and parts of the development workflow were supported by AI-powered tools in accordance with EPFL academic integrity guidelines.

AI assistance was used for:

- structuring documentation and improving technical writing
- debugging guidance and code review suggestions
- identifying edge cases in setup, communication, and hardware integration notes
- summarizing implementation details from the codebase

AI assistance was not used to replace:

- project goals or engineering decision-making
- hardware assembly, wiring, or physical testing
- experimental validation on the real robot
- team ownership of the system design and implementation

All AI-assisted content should be reviewed and validated by the project team before submission or hardware use.

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

### Hardware Credits

- SpotMicro and related open-source quadruped projects for mechanical and locomotion inspiration
- Existing quadruped robotics tutorials for reference designs and gait concepts
- Adafruit documentation for PCA9685 servo-driver integration
- PyBullet examples and community projects for simulation references

---

## Conclusion

Pluto demonstrates a practical path from a 3D-printed quadruped concept to a working robotics software platform. The project already includes the key layers needed for legged robotics development: calibrated low-level actuation, inverse kinematics, gait generation, simulation, command transport, and early sensing/interaction hooks.

The current system is not the final version of the robot. It is a foundation for continued tuning and experimentation, especially around gait stability, behavior execution, sensor-driven reactions, and higher-level autonomy. Its modular structure is intended to make those next steps approachable for future development.

### Questions, Issues, Or Feedback

For technical support or setup issues, check [First-Time Troubleshooting](TROUBLESHOOTING.md) first. For implementation details, use the [Documentation Index](#documentation-index) to find the relevant subsystem documentation.
