# Software Overview

Pluto's software is split into a robot-side firmware stack, a computer-side control stack, and a shared communication layer: hardware interfacing, locomotion, and wireless control.

## Main Components

| Component | Location | Role |
| --- | --- | --- |
| ESP32 firmware | [src/esp](src/esp/README.md) | Runs on the robot, controls servos, executes gaits, reads sensors, and handles UDP packets |
| Python control hub | [src/control](src/control/README.md) | Provides the NiceGUI interface, PyBullet simulation, keyboard/gamepad control, and speech command worker |
| Shared protocol | [src/comm](src/comm/README.md) | Defines the compact packet and message format used by both C++ firmware and Python control code |
| WiFi protocol notes | [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md) | Documents UDP connection setup, packet structure, batching, CRCs, and message payload layout |
| Sensor notes | [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) | Documents the ultrasonic distance sensor and INMP441 microphone |

## Firmware Layer

The ESP32 firmware handles the time-critical robot behavior:

- Servo calibration and PCA9685 PWM output
- Inverse kinematics for coxa, femur, and tibia joints
- Gait generation for stand, walk, trot, gallop, turn, stop, and behavior commands
- FreeRTOS-based UDP communication
- Ultrasonic and microphone sensor hooks

See [ESP32 Firmware](src/esp/README.md).

## Control Layer

The Python control hub provides the operator-facing tools:

- NiceGUI pages for controller and simulation modes
- PyBullet simulation using Pluto's STL meshes
- Keyboard and gamepad input normalization
- Vosk-based speech commands
- UDP packet creation and session handling

See [Controller UI](src/control/README.md).

## Communication Layer

Python and C++ share a compact binary protocol. Commands are grouped into UDP packets with CRC validation, sequence numbers, timestamps, session tokens, and packed 8-byte messages.

See [Communication](src/comm/README.md) and [WiFi Protocol](SOFTWARE_WIFI.md).

## Simulation Layer

The simulation code builds a PyBullet quadruped using the mesh assets in [src/mesh](src/mesh). It allows gait and control experiments before deploying movement to the physical robot.

## Current Extension Points

The software is structured to support future work on:

- Better gait stability and interpolation
- IMU-based stabilization
- Camera or SLAM-based perception
- Autonomous obstacle avoidance
- More expressive robot behaviors

See [Ongoing Works & Next Steps](ONGOING_WORK.md).
