# Hardware Overview

Pluto is a compact 12-servo quadruped robot. The hardware is organized around a 3D-printed body, four modular legs, an ESP32 controller, a PCA9685 servo driver, LiPo power, ultrasonic distance sensing, and microphone input.

## Main Hardware Areas

| Area | Purpose | Details |
| --- | --- | --- |
| Mechanical structure | Holds the electronics and supports four articulated legs | See [CAD Files](CAD_FILES.md) and [Assembly Instructions](ASSEMBLY.md) |
| Leg actuation | Provides 3 degrees of freedom per leg | 12 DMS15 270-degree servos, one coxa, femur, and tibia joint per leg |
| Servo control | Drives all servos with stable PWM | PCA9685 16-channel PWM driver over I2C |
| Main controller | Runs firmware, WiFi, sensors, and motion logic | ESP32 development board |
| Sensing | Provides basic environment and audio input | HC-SR04 ultrasonic sensor and INMP441 I2S microphone |
| Power | Supplies servo and logic power | 7.4V 2S LiPo, XT60 connectors, rocker switch, and LM2596 buck converter |

## Documentation

- [Assembly Instructions](ASSEMBLY.md): leg assembly, body assembly, and final integration
- [Wiring & Electrical](WIRING_ELECTRICAL.md): power path, circuit diagram, ESP32, PCA9685, sensors, and startup checks
- [CAD Files](CAD_FILES.md): available mesh files and 3D-printing notes
- [Software Sensors](SOFTWARE_SENSORS.md): ultrasonic and microphone software notes

## Build Order

1. Print the body and leg parts from the CAD/mesh files.
2. Assemble each leg from coxa, femur, tibia, servos, bearings, and linkage hardware.
3. Mount the coxa servos, PCA9685, buck converter, ESP32, sensors, and battery inside the body.
4. Wire the power system first, then the I2C/PWM/sensor connections.
5. Verify voltage levels and polarity before connecting all servos.
6. Flash the ESP32 firmware and test basic commands before running full gaits.
