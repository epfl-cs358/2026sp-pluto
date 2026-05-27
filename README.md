<div align="center">

![](images/pluto.png)

# 🐶 Pluto

**A compact 12-DOF quadruped robot with ESP32 gait control, calibrated servo actuation, sensor reactions, controller-side speech commands, and simulation assets.**

![PlatformIO](https://img.shields.io/badge/Firmware-PlatformIO-orange)
![ESP32](https://img.shields.io/badge/Controller-ESP32-blue)
![Python](https://img.shields.io/badge/UI-Python%203.13-green)
![NiceGUI](https://img.shields.io/badge/Web%20UI-NiceGUI-purple)
![Simulation](https://img.shields.io/badge/Simulation-PyBullet%20%2B%20MuJoCo-lightgrey)
![Course](https://img.shields.io/badge/EPFL-CS--358-red)

</div>

> **Current status:** Pluto has the main robot platform in place: ESP32 firmware, gait code, servo calibration hooks, ultrasonic wall-stopping, a compiled microphone driver, a Python controller, shared UDP messages, updated per-leg meshes, and a MuJoCo model. Final physical gait tuning, microphone behavior re-enabling, and some behavior sequences still need validation on the real robot.

> **Live demo:** Demo media can be added here once final robot videos are available.

---

## 🧭 Quick Navigation

<table>
<tr>
<td width="25%" align="center" valign="top">
<a href="ASSEMBLY.md"><strong>Build Pluto</strong></a><br>
Assembly steps, leg construction, body integration, and final checks.
</td>
<td width="25%" align="center" valign="top">
<a href="WIRING_ELECTRICAL.md"><strong>Wire Pluto</strong></a><br>
Power path, common ground, PCA9685, sensors, and safety checks.
</td>
<td width="25%" align="center" valign="top">
<a href="src/esp/README.md"><strong>ESP32 Firmware</strong></a><br>
Gaits, IK, servos, sensors, serial commands, and WiFi/UDP.
</td>
<td width="25%" align="center" valign="top">
<a href="src/control/README.md"><strong>Controller UI</strong></a><br>
NiceGUI, keyboard/gamepad input, speech commands, and telemetry.
</td>
</tr>
<tr>
<td width="25%" align="center" valign="top">
<a href="CAD_FILES.md"><strong>Meshes</strong></a><br>
Current STL inventory and MuJoCo mesh usage.
</td>
<td width="25%" align="center" valign="top">
<a href="src/comm/README.md"><strong>Protocol</strong></a><br>
Shared C++/Python UDP packet and message format.
</td>
<td width="25%" align="center" valign="top">
<a href="SIMULATION.md"><strong>Simulation</strong></a><br>
PyBullet UI path, MuJoCo bridge, and current limitations.
</td>
<td width="25%" align="center" valign="top">
<a href="TROUBLESHOOTING.md"><strong>Troubleshooting</strong></a><br>
First-time setup, firmware, power, sensors, and WiFi issues.
</td>
</tr>
</table>

---

## 📚 Table of Contents

<table>
<tr>
<td width="33%" valign="top">

- [Project Context](#project-context)
- [What Pluto Can Do](#what-pluto-can-do)
- [Current Status](#current-status)
- [Quick Start](#quick-start)

</td>
<td width="33%" valign="top">

- [System Architecture](#system-architecture)
- [Hardware Overview](#hardware-overview)
- [Software Overview](#software-overview)
- [Simulation](#simulation)

</td>
<td width="33%" valign="top">

- [Configuration and Tuning](#configuration-and-tuning)
- [Repository Structure](#repository-structure)
- [Known Limitations](#known-limitations)
- [Detailed Technical Reference](#detailed-technical-reference)

</td>
</tr>
</table>

---

## 🧠 Project Context

<table>
<tr>
<td width="50%" valign="top">

### 🎯 Vision

Pluto is designed as a reusable quadruped robotics platform rather than a single-purpose demo. The goal is to make a small robot that can stand, walk, trot, gallop, react to simple sensor events, and accept commands through serial, WiFi, UI controls, and speech.

The same concepts appear across the firmware, controller, and simulation code: leg geometry, foot targets, inverse kinematics, gait phases, calibrated servo limits, and compact command messages.

</td>
<td width="50%" valign="top">

### 🎓 Course Context

Pluto was developed as part of **CS-358 Making Intelligent Things** at **EPFL**.

The repository documents both the final implementation and the current work-in-progress state so future development can continue from the same hardware, firmware, controller, protocol, and simulation foundation.

</td>
</tr>
</table>

### 🧱 Main Implementation Layers

| Layer | Role |
| --- | --- |
| ESP32 firmware | Servo control, gait execution, ultrasonic sensing, microphone sensing, serial testing, and WiFi/UDP command handling |
| Python tooling | NiceGUI control hub, keyboard/gamepad input, speech command handling, telemetry display, and simulation control |
| Shared communication | C++ and Python packet/message definitions kept aligned |
| CAD and simulation assets | 3D-printing meshes, simulation meshes, `pluto.xml`, PyBullet-facing code, and MuJoCo bridge |

---

## 🚀 What Pluto Can Do

<table>
<tr>
<td width="50%" valign="top">

### 🤖 Robot Side

- Drive 12 servo joints through a PCA9685 PWM driver.
- Use per-joint calibration for coxa, femur, and tibia angles.
- Run stand, stop, forward, backward, left turn, right turn, walk, trot, gallop, bow, and paw motion logic.
- Update gait motion every 20 ms.
- Stop motion when the ultrasonic sensor detects a close wall.
- Compile the microphone driver; the clap-energy walking toggle code exists but is currently disabled in the main loop.
- Accept serial monitor commands for gait testing and servo trimming.
- Receive UDP commands over WiFi when configured on the same network.

</td>
<td width="50%" valign="top">

### 💻 Computer Side

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

## 📌 Current Status

| Area | Status | Notes |
| --- | --- | --- |
| Physical robot | In progress | Main build and wiring docs exist; final gait validation still needed |
| Servo control | Implemented | Calibration lives in `src/esp/legs/leg_data.h` |
| Gaits | Implemented, tuning needed | Walk, trot, gallop, left/right turn, bow, paw, stand, and stop logic exist |
| Sensors | Implemented, tuning needed | Ultrasonic wall stop is active; microphone driver is compiled, but clap-control is currently commented out |
| Controller UI | Implemented | Connect button, mDNS scan, movement input, quick actions, telemetry, speech worker |
| WiFi/UDP | Implemented, enabled in firmware | Add WiFi credentials in `setup()`; the controller discovers `_pluto._udp.local` with zeroconf |
| Meshes | Updated | 3D-printing meshes live in `src/3D printing mesh`; simulation meshes and `pluto.xml` live in `src/sim/sim_mesh` |
| PyBullet | Partial | UI path exists, but visual meshes still reference old generic filenames |
| MuJoCo | Partial | Model and C++ bridge exist, but not yet a validated physical twin |

---

## ⚡ Quick Start

### 📦 1. Install Requirements

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

### ▶️ 2. Run the Controller

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

### 🔌 3. Build and Flash the ESP32

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

### 📶 4. Configure WiFi Control

WiFi support is currently enabled in `src/esp/main.cpp`:

```cpp
#define PLUTO_ENABLE_WIFI
```

Then:

1. Configure access points with `PLUTO_SERVER.addAP(...)`.
2. Make sure the computer and ESP32 are on the same network.
3. Run the controller and open `/controller`.
4. Press `Connect & Take Control`; the Python client scans for Pluto via mDNS/zeroconf and then performs the UDP handshake.

See [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md).

---

## 🚀 Build Pluto From Scratch

This is the recommended order for a new team starting with only the repository, parts, and a 3D printer.

| Phase | Goal | Main files/docs | Exit check |
| --- | --- | --- | --- |
| 1. Understand the system | Know what each subsystem does before building | `README.md`, [SOFTWARE_OVERVIEW.md](SOFTWARE_OVERVIEW.md), [HARDWARE_OVERVIEW.md](HARDWARE_OVERVIEW.md) | Team can explain firmware, controller, communication, sensors, and simulation roles |
| 2. Print parts | Produce body and leg parts from the current 3D-printing mesh set | [CAD_FILES.md](CAD_FILES.md), `src/3D printing mesh/` | All body, coxa, femur, tibia, spacer, and linkage parts are printed and inspected |
| 3. Assemble one leg | Validate mechanical fit before repeating four times | [ASSEMBLY.md](ASSEMBLY.md) | One leg moves freely by hand without binding |
| 4. Assemble all legs and body | Build the full physical robot frame | [ASSEMBLY.md](ASSEMBLY.md) | Four legs are mounted with correct orientation |
| 5. Build power system | Prepare battery, switch, buck converter, grounds, and servo power | [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md) | Buck output is measured and all grounds are common |
| 6. Wire controller and servos | Connect ESP32, PCA9685, servos, ultrasonic sensor, and microphone | [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md), [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) | ESP32 flashes over USB and PCA9685 powers correctly |
| 7. Bring up firmware safely | Test stand, stop, and raw trim commands before walking | [src/esp/README.md](src/esp/README.md) | Serial monitor works and joints move in expected directions |
| 8. Calibrate servos | Tune per-joint raw limits, start values, angle ranges, and inversion flags | `src/esp/legs/leg_data.h` | Each joint can move through a safe range without hitting mechanical stops |
| 9. Test motion slowly | Validate stand, walk, trot, gallop, bow, and paw behavior | `src/esp/motion/gait.cpp`, [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Robot can stand reliably and execute controlled test motions |
| 10. Test controller | Configure WiFi and Python controller only after safe serial tests | [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md), [src/control/README.md](src/control/README.md) | Controller connects and sends stop/behavior/movement commands |
| 11. Use simulation for iteration | Inspect gait logic and mesh assumptions before risky physical tests | [SIMULATION.md](SIMULATION.md) | Team understands current PyBullet and MuJoCo limitations |

For a new team, the most important rule is to validate one subsystem at a time. Do not test full-body walking before power, calibration, and single-joint behavior are understood.

### 🧰 From Zero To First Motion

This section is written as a handoff guide for a team that did not build the original robot. It repeats some information from the detailed docs so the README can be used as a complete starting point.

#### Step 1: Prepare the repository and tools

1. Clone the repository.
2. Install Python 3.13.
3. Install PlatformIO Core or the PlatformIO VS Code extension.
4. Confirm that `pio --version` works.
5. Confirm that the Python controller can start with `bash run.sh` or `run.bat`.
6. Do not connect the LiPo battery yet.

The goal of this step is only to verify that the computer can build firmware and run the controller UI.

#### Step 2: Print and label the parts

Print the physical robot parts from `src/3D printing mesh/`. The folders are organized by leg:

- `Front Left/`
- `Front Right/`
- `Back Left/`
- `Back Right/`

Each folder contains that leg's coxa, femur, tibia, spacer, and linkage/bar parts. The simulation-only meshes are separate and live in `src/sim/sim_mesh/`.

Label parts as soon as they are printed. Do not mix parts between legs. The firmware also uses `TOP_LEFT`, `TOP_RIGHT`, `BOTTOM_LEFT`, and `BOTTOM_RIGHT`, so a physical mix-up can become a software calibration problem later.

Before installing any servo:

- Check that each servo fits into its printed part.
- Check that screw holes are usable.
- Check that bearings sit correctly.
- Check that the tibia linkage can move without rubbing.

#### Step 3: Assemble one leg first

Build one complete leg before building the other three.

1. Assemble the tibia with its bearing and foot.
2. Assemble the femur around the servo.
3. Attach the coxa section.
4. Connect femur and tibia with the linkage.
5. Move the leg by hand.

The leg should move smoothly without powered servos. If it binds by hand, it will bind under servo power.

#### Step 4: Assemble the body and remaining legs

After one leg is validated:

1. Assemble the other three legs.
2. Mount coxa servos into the body.
3. Mount the PCA9685.
4. Mount the buck converter.
5. Mount the ESP32.
6. Mount the ultrasonic sensor at the front.
7. Mount the microphone if used.
8. Mount the battery so it is secure but removable.

Do not power the robot from the battery yet.

#### Step 5: Wire power safely

Wire in this order:

1. LiPo connector.
2. Rocker switch.
3. Servo power rail and PCA9685 servo power input.
4. Buck converter input.
5. Buck converter output.
6. ESP32 power.
7. PCA9685 logic power and I2C.
8. Sensors.
9. Servos.

Before connecting the ESP32 or sensors to the buck converter, measure the output with a multimeter. All grounds must be connected together: battery/servo ground, PCA9685 ground, buck converter ground, ESP32 ground, and sensor ground.

#### Step 6: Verify servo channels

The firmware expects this PCA9685 channel mapping:

| Leg | Coxa | Femur | Tibia |
| --- | ---: | ---: | ---: |
| `TOP_LEFT` | 0 | 1 | 2 |
| `TOP_RIGHT` | 4 | 5 | 6 |
| `BOTTOM_LEFT` | 8 | 9 | 10 |
| `BOTTOM_RIGHT` | 12 | 13 | 14 |

Channels 3, 7, 11, and 15 are unused by the current leg abstraction.

If a servo is plugged into the wrong channel, the robot may move the wrong joint even if the code is correct.

#### Step 7: Flash firmware with the robot supported

Put the robot on a stand so the legs cannot hit the table.

Then run:

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

At this stage, use serial commands only. Do not use WiFi movement yet.

#### Step 8: Calibrate one joint at a time

Calibration is in:

```text
src/esp/legs/leg_data.h
```

Each joint has:

- `raw_min`
- `raw_max`
- `raw_stand`
- `raw_forward`
- `raw_turnleft`
- `raw_turnright`
- `raw_bow`
- `raw_sit`
- `angle_min_md`
- `angle_max_md`
- `inverted`

Use the serial monitor:

- `l` selects the next leg.
- `n` selects the next joint.
- `p` prints the selected leg, joint, and pulse.
- `+` increases raw PWM by 5.
- `-` decreases raw PWM by 5.
- `r` resets the selected joint.
- `R` resets all joints on the selected leg.

For each joint:

1. Select the leg and joint.
2. Print the current value.
3. Move slowly with `+` and `-`.
4. Stop before mechanical binding.
5. Record safe min and max raw values.
6. Choose safe standing and motion-specific raw start values.
7. Check whether direction needs `inverted = true`.
8. Rebuild and upload after editing calibration.

Only after all 12 joints are calibrated should walking be tested.

#### Step 9: Test motion in small stages

Recommended order:

1. `s`: stop and stand.
2. `1`: select walk.
3. `0`: set speed to 0%.
4. `5`: set speed to 50%.
5. `f`: test forward motion briefly.
6. `q`: test left turn briefly.
7. `e`: test right turn briefly.
8. `s`: stop.
9. `2`: select trot only after walk and turns are stable.
10. `3`: select gallop only after lower-speed gaits are stable.

If a leg moves in the wrong direction, stop and return to calibration. Do not compensate by forcing the gait constants first.

#### Step 10: Test sensors

By default, ultrasonic and microphone support are compiled:

```cpp
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

Current sensor templates:

- Ultrasonic: `SensorUltraSonic<5, 18>`
- Microphone: `SensorMicrophone<26, 25, 33>`

The ultrasonic sensor stops motion if the measured distance is below 35 cm. The microphone object is initialized, but `update_microphone_control(now)` is currently commented out in `src/esp/main.cpp`; re-enable that call only after validating the energy threshold on the real microphone.

#### Step 11: Test WiFi last

Only test WiFi control after serial movement is safe.

1. Confirm `#define PLUTO_ENABLE_WIFI` is present.
2. Add WiFi credentials with `PLUTO_SERVER.addAP(...)`.
3. Upload firmware.
4. Run the Python controller.
5. Press `Connect & Take Control`.
6. Press `Stop All` before sending movement.
7. Watch telemetry for acknowledgements.

The controller should be treated as a convenience layer, not the first debugging tool.

---

## 🏗️ System Architecture

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
<td width="33%" valign="top">

### 🎛️ Control Layer

Runs on the user's computer.

- NiceGUI app
- keyboard/gamepad input
- Vosk speech worker
- UDP controller client
- telemetry display
- simulation controls

</td>
<td width="33%" valign="top">

### 🤖 Robot Layer

Runs on the ESP32.

- PCA9685 servo output
- leg calibration
- inverse kinematics
- gait controller
- ultrasonic checks
- microphone energy checks
- UDP server with mDNS advertising

</td>
<td width="33%" valign="top">

### 🔁 Shared Layer

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

### 💡 Why The System Is Split This Way

| Design choice | Reason |
| --- | --- |
| Keep gait and servo output on ESP32 | Servo updates and stop/stand behavior stay close to the hardware |
| Keep UI, speech, and simulation on the computer | These are heavier and easier to iterate in Python |
| Mirror message definitions in C++ and Python | The controller and robot share one compact command language |
| Keep simulation assets in the repo | Gait timing and actuator mapping can be inspected before hardware tests |

---

## 🔩 Hardware Overview

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

### 🧰 Additional Materials

- PLA/PETG filament for rigid printed parts.
- TPU filament or rubber pads for feet.
- M3 and M2.5 screws for the body, legs, and servo mounting.
- Ball bearings for tibia joints.
- Jumper wires for logic signals and thicker wires for servo current.
- Soldering equipment, heat-shrink tubing, electrical tape, and cable ties.
- Multimeter for voltage, polarity, and continuity checks.
- LiPo-safe charger and LiPo-safe storage bag.

### 🧱 Current Mesh Set

The current physical 3D-printing CAD/STL files live in `src/3D printing mesh/`, grouped by leg:

```text
src/3D printing mesh/
|-- Front Left/
|-- Front Right/
|-- Back Left/
`-- Back Right/
```

The simulation meshes live separately in `src/sim/sim_mesh/` and include `body.stl`, per-leg coxa/femur/tibia STL files, and `pluto.xml`. See [CAD_FILES.md](CAD_FILES.md).

### 🔄 Build Flow

<table>
<tr>
<td align="center" valign="top"><strong>1</strong><br>Print parts</td>
<td align="center" valign="top"><strong>2</strong><br>Assemble legs</td>
<td align="center" valign="top"><strong>3</strong><br>Mount electronics</td>
<td align="center" valign="top"><strong>4</strong><br>Wire power and sensors</td>
<td align="center" valign="top"><strong>5</strong><br>Flash firmware</td>
<td align="center" valign="top"><strong>6</strong><br>Calibrate servos</td>
</tr>
</table>

Detailed hardware docs:

- [ASSEMBLY.md](ASSEMBLY.md)
- [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md)
- [HARDWARE_OVERVIEW.md](HARDWARE_OVERVIEW.md)
- [CAD_FILES.md](CAD_FILES.md)

### ⚠️ Electrical Safety Notes

| Topic | Note |
| --- | --- |
| Common ground | LiPo/servo ground, PCA9685 ground, ESP32 ground, buck converter ground, and sensor grounds must be connected together |
| Servo power | Servos draw much higher current than the ESP32 and sensors; plan the power paths separately |
| Buck converter | Adjust and measure the LM2596 output before connecting the ESP32 or sensors |
| PCA9685 | Provide both logic power and servo power, and connect SDA/SCL to the ESP32 |
| Ultrasonic ECHO | If ECHO outputs 5V, use a voltage divider or level shifter before the ESP32 GPIO |
| High-current wiring | Use appropriate wire thickness for LiPo, XT60, rocker switch, servo power rail, and PCA9685 servo power |

---

## 💻 Software Overview

For a new team, read the code in this order:

| Step | Read | Why |
| --- | --- | --- |
| 1 | `src/esp/legs/leg_data.h` | Defines the physical servo calibration assumptions |
| 2 | `src/esp/legs/leg.h` and `leg_joint.h` | Shows how servo channels and joint commands are abstracted |
| 3 | `src/esp/motion/ik_solver.cpp` | Converts foot targets into coxa/femur/tibia angles |
| 4 | `src/esp/motion/gait.cpp` | Generates stand, walk, trot, gallop, bow, and paw motion |
| 5 | `src/esp/main.cpp` | Connects setup, loop timing, sensors, serial commands, and WiFi |
| 6 | `src/comm/message.h` | Defines the shared C++ message format |
| 7 | `src/control/pluto_server/message.py` | Mirrors the same format in Python |
| 8 | `src/control/pluto_server/server.py` | Handles Python-side UDP connection, heartbeat, and receive loop |
| 9 | `src/control/pluto_menu/controller.py` | Converts UI input into movement and behavior messages |
| 10 | `src/control/sim_motion.py` and `src/sim/sim_main.cpp` | Shows the current simulation paths and their limitations |

### 🧠 Firmware

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
| `src/esp/server/server.cpp` | UDP server and mDNS advertisement |

### 🎛️ Controller

Python dependencies:

```text
nicegui
numpy
debugpy
pybullet
vosk
sounddevice
zeroconf
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

### 🔁 Communication

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
| `MOVE_BY` | Python -> ESP32 | Carries forward/back and left/right signed 16-bit directions; firmware-side mapping is still marked as a TODO |
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

## 🎮 Simulation

| Path | Files | Current state |
| --- | --- | --- |
| Python PyBullet UI | `src/control/pluto_menu/simulation.py`, `src/control/sim_motion.py` | Integrated in the UI, but visual mesh references need updating |
| MuJoCo C++ bridge | `CMakeLists.txt`, `src/sim/sim_main.cpp`, `src/sim/sim_gait.*`, `src/sim/sim_leg.*`, `src/sim/sim_mesh/pluto.xml` | Uses current simulation meshes and simulation-side copies/adapters of the ESP leg and gait abstractions |

The MuJoCo path is more aligned with the current mesh set. The PyBullet path is still useful for UI/control experiments, but it needs a mesh update before it fully represents the current CAD assets.

The C++ MuJoCo bridge uses the top-level `CMakeLists.txt`, which sets `PLUTO_MODEL_PATH` to `src/sim/sim_mesh/pluto.xml`. Its keyboard handlers are intended to mirror the firmware test controls: `F`/`B` for forward/backward, `Q`/`E` for left/right turning, `P` for paw, `O` for bow, `S` for stop, and `1`/`2`/`3` for walk/trot/gallop. Mouse drag and scroll controls are wired through MuJoCo's camera helpers. Treat this bridge as development support until it has been rebuilt and validated on the current machine.

See [SIMULATION.md](SIMULATION.md).

---

## ⚙️ Configuration and Tuning

### 🧠 Firmware Feature Flags

Current defaults in `src/esp/main.cpp`:

```cpp
#define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

| Feature | Default | Notes |
| --- | --- | --- |
| WiFi/UDP | On | Configure valid access points before using controller networking |
| Ultrasonic | On | Stops motion when wall distance is below threshold |
| Microphone | Compiled on, control inactive | `SensorMicrophone<26, 25, 33>` is initialized, but the clap-control update call is commented out |

### 📍 Sensor Pins and Thresholds

| Item | Current value |
| --- | --- |
| Ultrasonic template | `SensorUltraSonic<5, 18>` |
| Microphone template | `SensorMicrophone<26, 25, 33>` |
| Wall stop distance | 35 cm |
| Ultrasonic period | 150 ms |
| Microphone clap threshold | `30000000` in the commented clap-control block |
| Clap cooldown | 800 ms in the commented clap-control block |

Operating timing:

| Loop / event | Current timing |
| --- | --- |
| Gait update | 20 ms / 50 Hz |
| Ultrasonic read cycle | 150 ms |
| Ultrasonic wait after trigger | 10 ms |
| Microphone energy print cycle | 1000 ms in the commented clap-control block |
| Serial monitor baud | 115200 |

### 🎚️ Servo Calibration

Servo calibration lives in:

```text
src/esp/legs/leg_data.h
```

Each joint has:

- `raw_min`
- `raw_max`
- `raw_stand`
- `raw_forward`
- `raw_turnleft`
- `raw_turnright`
- `raw_bow`
- `raw_sit`
- `angle_min_md`
- `angle_max_md`
- `inverted`

Tune these values on the physical robot before running full gaits.

### 📐 Gait and IK Parameters

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

### ⌨️ Serial Commands

Use:

```bash
pio device monitor -b 115200
```

| Key | Action |
| --- | --- |
| `f` | Move forward |
| `b` | Move backward |
| `q` | Turn left |
| `e` | Turn right |
| `o` | Flip |
| `h` | Bow |
| `i` | Sit |
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

## 🗂️ Repository Structure

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
    |-- 3D printing mesh/ # Physical 3D-printing STL files
    |-- comm/             # Shared protocol definitions
    |-- control/          # Python UI, input, speech, and controller client
    |-- esp/              # ESP32 firmware
    `-- sim/              # C++ MuJoCo simulation bridge and sim_mesh assets
```

---

## ⚠️ Known Limitations

<table>
<tr>
<td width="50%" valign="top">

### 🔩 Hardware and Motion

- Physical gait validation is still needed.
- Servo calibration needs final tuning.
- Gait constants need real-world measurement.
- Behavior sequences need calibration.

</td>
<td width="50%" valign="top">

### 💻 Software and Simulation

- WiFi credentials still need to be configured for the target network.
- Controller discovery depends on mDNS/zeroconf working on the local network.
- UDP `MOVE_BY` messages are defined, but the ESP32 handler still needs to map them into gait commands.
- PyBullet visual meshes need updating.
- MuJoCo is not yet a validated physical twin.

</td>
</tr>
</table>

See [ONGOING_WORK.md](ONGOING_WORK.md).

---

## 🔬 Detailed Technical Reference

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
- **MuJoCo**: Physics simulator used by `src/sim/sim_mesh/pluto.xml` and the files under `src/sim/`.
- **PyBullet**: Python simulation dependency used by the control stack.

</details>

<details>
<summary><strong>Project objectives</strong></summary>

- Reliable servo control: drive 12 calibrated joints safely through the PCA9685.
- Legged locomotion: support stand, stop, forward/backward movement, left/right turns, walk, trot, gallop, bow, and paw motion primitives.
- IK-based movement: generate joint angles from foot targets instead of fixed pulse sequences.
- Remote operation: use a shared UDP protocol for movement, behavior, info, acknowledgement, and sensor messages.
- Sensor reactions: stop near obstacles; microphone energy code exists for clap-triggered start/stop behavior but is currently disabled in the main loop.
- Simulation before hardware: keep mesh and simulation assets available for gait development.
- Clear documentation: keep hardware, wiring, firmware, controller, protocol, and troubleshooting notes separated.

</details>

<details>
<summary><strong>Core technologies</strong></summary>

- **Hardware**: ESP32, PCA9685, 12 DMS15-style servos, HC-SR04-style ultrasonic sensor, INMP441 I2S microphone, 2S LiPo, LM2596 buck converter.
- **Firmware**: C++17, Arduino framework, FreeRTOS, PlatformIO, Adafruit PWM Servo Driver, Adafruit BusIO.
- **Control UI**: Python 3.13, NiceGUI, keyboard/gamepad input, Vosk, sounddevice.
- **Simulation**: PyBullet in Python, external MuJoCo/GLFW for the C++ bridge, STL mesh assets.
- **Communication**: Custom C++/Python UDP packet format with session token, sequence number, timestamp, CRC, and packed messages.

</details>

<details>
<summary><strong>ESP32 firmware responsibilities</strong></summary>

- Initialize the PCA9685 servo driver.
- Drive all 12 joints through calibrated `LegJoint` objects.
- Convert IK outputs into constrained PWM pulses.
- Update gait motion every 20 ms.
- Handle serial commands for movement, turning, gait selection, speed changes, manual walk staging, and servo trimming.
- Read ultrasonic distance and stop when a wall is too close.
- Initialize the microphone driver; clap detection logic is present but currently disabled in `loop()`.
- Accept UDP commands through the Pluto server when connected to WiFi.

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
- [MuJoCo model](src/sim/sim_mesh/pluto.xml)
- [C++ simulation entry point](src/sim/sim_main.cpp)

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

- Hardware gait validation: test walk, trot, gallop, left/right turns, flip, bow, sit, paw, and stop on the physical robot.
- Servo calibration: refine PWM limits, standing and motion-specific starting pulses, inversion flags, and angle ranges.
- WiFi control: validate live UDP movement and behavior commands over mDNS-discovered connections.
- Behavior implementation: replace placeholder behavior handlers with calibrated motion sequences.
- Sensor-driven reactions: tune ultrasonic wall stopping and re-enable/tune microphone clap detection if needed.
- Simulation fidelity: improve physical accuracy for mass, friction, joint limits, and servo response.

Known issues:

- Some firmware behavior handlers are still placeholders.
- Some networks may block mDNS discovery, requiring controller-side fallback work.
- Gait constants need final physical measurement and tuning.
- MuJoCo and PyBullet support are useful for development, but not yet perfect models of the real robot.

</details>

---

## 👥 Project Team

Pluto was developed as part of EPFL's **CS-358 Making Intelligent Things** course by:

<table>
<tr>
<td align="center" valign="top">Serhat Botan</td>
<td align="center" valign="top">Alexis Cazal</td>
<td align="center" valign="top">Neha Chakraborty</td>
<td align="center" valign="top">Myriam Lahoud</td>
</tr>
<tr>
<td align="center" valign="top">Sam Lee</td>
<td align="center" valign="top">Raphael Dib Nehme</td>
<td align="center" valign="top">Mariya Rakytyanska</td>
</tr>
</table>

## 🙏 Acknowledgments

- EPFL course staff, TAs, and coaches.
- Open-source quadruped projects, including SpotMicro-style robots.
- Arduino, PlatformIO, FreeRTOS, Adafruit, NiceGUI, PyBullet, MuJoCo, GLFW, Vosk, and sounddevice documentation and examples.

## 🤖 Use of AI Tools

AI tools were used to support:

- Debugging guidance and code review suggestions.
- Edge-case review for setup, communication, and hardware integration notes.
- Summaries of implementation details from the codebase.

AI assistance was not used to replace:

- Project goals or engineering decision-making.
- Hardware assembly, wiring, or physical testing.
- Experimental validation on the real robot.
- Team ownership of the system design and implementation.
- Writing the instructions and README.

---

## ✅ Closing Notes

Pluto is a working foundation for a small quadruped robotics platform. The repository already contains low-level actuation, IK, gait generation, sensing hooks, controller-side interaction, command transport, and simulation assets. The next major step is careful physical tuning: servo calibration, gait stability, behavior sequences, and validation on the real robot.
