# 🔩 Hardware Overview

Pluto is a compact 12-servo quadruped robot. The hardware is organized around a 3D-printed body, four modular 3-DOF legs, an ESP32 controller, a PCA9685 servo driver, LiPo power, ultrasonic distance sensing, and microphone input.

## 🔩 Main Hardware Areas

| Area | Purpose | Details |
| --- | --- | --- |
| Mechanical structure | Holds electronics and supports four articulated legs | Current per-leg meshes are listed in [CAD Files](CAD_FILES.md) |
| Leg actuation | Provides 3 degrees of freedom per leg | 12 DMS15-style 270-degree servos: coxa, femur, and tibia on each leg |
| Servo control | Drives all servos with stable PWM | PCA9685 16-channel PWM driver over I2C |
| Main controller | Runs firmware, sensors, gait logic, and WiFi/UDP | ESP32 development board |
| Sensing | Provides basic environment and audio input | HC-SR04-style ultrasonic sensor and INMP441 I2S microphone |
| Power | Supplies servo and logic power | 7.4V 2S LiPo, XT60 connectors, rocker switch, and LM2596 buck converter |

## 🔢 Build Order

1. Print the body, holder, leg, and TPU foot parts from the current physical 3D-printing mesh set in `src/3D printing mesh`.
2. Assemble each coxa/femur/tibia leg chain with servos, horns, bearings, and linkage hardware.
3. Mount the coxa servos, PCA9685, buck converter, ESP32, sensors, and battery in the body.
4. Wire the power system first, then I2C, servo channels, and sensor connections.
5. Verify voltage levels and polarity before connecting all servos.
6. Flash the ESP32 firmware and test basic serial commands before running full gaits.

## 🔩 From-Scratch Hardware Milestones

Use these milestones to know when it is reasonable to move to the next stage:

| Milestone | What must be true |
| --- | --- |
| Mechanical prototype | One leg can be assembled and moved by hand without binding |
| Full frame | Four legs are mounted and have consistent orientation |
| Power system | Battery, switch, buck converter, and common ground are verified with a multimeter |
| Servo wiring | Each PCA9685 channel controls the expected joint |
| Sensor wiring | Ultrasonic and microphone pins match `src/esp/main.cpp` |
| Firmware bring-up | Serial monitor commands work before walking is attempted |
| Calibration | Every joint has safe `raw_min`, `raw_max`, standing raw, and motion-specific start values |

## 🔩 Important Hardware Notes

- All electronics and servo power paths must share a common ground.
- The servos can draw large current spikes, so use appropriate wire thickness for the servo power rail.
- Check the LM2596 output with a multimeter before powering the ESP32 or sensors from it.
- Confirm the ultrasonic ECHO voltage before connecting it directly to an ESP32 pin.
- Calibrate servo limits in firmware before testing full body movements.

## 📚 Documentation

- [Assembly Instructions](ASSEMBLY.md): leg assembly, body assembly, and final integration.
- [Wiring & Electrical](WIRING_ELECTRICAL.md): power path, circuit diagram, ESP32, PCA9685, sensors, and startup checks.
- [CAD Files](CAD_FILES.md): current mesh files and 3D-printing notes.
- [Software Sensors](SOFTWARE_SENSORS.md): ultrasonic and microphone software notes.
- [Troubleshooting](TROUBLESHOOTING.md): first-time setup and failure checks.
