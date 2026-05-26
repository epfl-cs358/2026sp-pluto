# ESP32 Firmware

This directory contains Pluto's robot-side firmware.

## Entry Point

```text
src/esp/main.cpp
```

PlatformIO is configured with `src_dir = src/esp`, so this directory is the firmware source root.

## Main Modules

| Module | Purpose |
| --- | --- |
| `main.cpp` | Firmware setup, loop, feature flags, sensors, gait update, serial commands, and optional UDP dispatch |
| `legs/leg_data.h` | Per-joint PWM and angle calibration |
| `legs/leg_joint.h` | Calibrated servo output abstraction |
| `legs/leg.h` | Three-joint leg abstraction |
| `motion/ik_solver.cpp` | Embedded inverse kinematics |
| `motion/gait.cpp` | Stand, walk, trot, gallop, bow, paw, and gait update logic |
| `sensors/ultrasonic.h` | Ultrasonic distance sensor abstraction |
| `sensors/microphone.h` | INMP441 I2S microphone abstraction |
| `server/server.cpp` | Optional UDP server, sessions, CRC validation, acknowledgements, and queues |

## Feature Flags

Current defaults in `main.cpp`:

```cpp
// #define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

That means ultrasonic and microphone support are enabled by default, while WiFi/UDP control must be explicitly enabled.

## Timing

- Gait update: every 20 ms.
- Ultrasonic cycle: every 150 ms.
- Ultrasonic wait after trigger: 10 ms.
- Microphone energy print period: 150 ms.
- Clap cooldown: 800 ms.
- Serial baud rate: 115200.

## Sensors

Current templates in `main.cpp`:

- Ultrasonic: `SensorUltraSonic<5, 18>`
- Microphone: `SensorMicrophone<26, 25, 33>`

Forward motion stops if ultrasonic distance is between 0 and 20 cm.

A microphone energy spike above the clap threshold toggles walking on/off.

## Serial Commands

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

## WiFi/UDP

When `PLUTO_ENABLE_WIFI` is enabled, the firmware creates:

```cpp
auto PLUTO_SERVER = pluto::PlutoServer{4242};
```

Configure access points in `setup()` with:

```cpp
PLUTO_SERVER.addAP("<WIFI_NAME>", "<WIFI_PASSWORD>");
```

See [WiFi Protocol](../../SOFTWARE_WIFI.md) and [Shared Communication Protocol](../comm/README.md).

## Build and Upload

From the repository root:

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```
