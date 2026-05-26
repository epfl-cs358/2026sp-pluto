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

## Firmware Execution Flow

This is the order in which the firmware operates:

1. `setup()` starts serial output at `115200`.
2. If WiFi is enabled, `PLUTO_SERVER` is configured and started.
3. The PCA9685 is initialized and set to 50 Hz.
4. Enabled sensors are initialized.
5. `GAIT.stand(LEGS)` moves the robot into the starting stand pose.
6. `loop()` repeatedly reads microphone and ultrasonic state.
7. Every 20 ms, `GAIT.update(LEGS, now)` writes updated joint targets.
8. Serial input is checked and converted into gait, speed, trimming, or reset commands.
9. If WiFi is enabled, queued UDP messages are processed.

Understanding this flow is important before adding new behavior. Long blocking code in `loop()` can make gait updates irregular.

## Leg and Servo Mapping

Each leg uses four PCA9685 channel slots. The current firmware uses the first three slots for coxa, femur, and tibia:

| Leg | Coxa | Femur | Tibia | Unused |
| --- | ---: | ---: | ---: | ---: |
| `TOP_LEFT` | 0 | 1 | 2 | 3 |
| `TOP_RIGHT` | 4 | 5 | 6 | 7 |
| `BOTTOM_LEFT` | 8 | 9 | 10 | 11 |
| `BOTTOM_RIGHT` | 12 | 13 | 14 | 15 |

This mapping comes from `Leg::CHANNEL_STEPS_PER_SIDE = 4` in `legs/leg.h`.

## Servo Calibration Workflow

Calibration lives in:

```text
src/esp/legs/leg_data.h
```

Each joint has:

- `raw_min`: lowest safe raw PWM value.
- `raw_max`: highest safe raw PWM value.
- `raw_start`: neutral or startup raw PWM value.
- `angle_min_md`: minimum logical angle in millidegrees.
- `angle_max_md`: maximum logical angle in millidegrees.
- `inverted`: whether logical angle direction is reversed for that joint.

Recommended calibration sequence:

1. Place the robot on a stand so legs can move freely.
2. Use serial command `l` to select a leg.
3. Use serial command `n` to select a joint.
4. Use `p` to print the current selected joint and pulse.
5. Use `+` and `-` to move in small raw PWM increments.
6. Record the safe lower and upper values before mechanical binding.
7. Set `raw_start` to a physically neutral starting value.
8. Confirm whether the logical direction needs `inverted = true`.
9. Repeat for all 12 joints.
10. Rebuild and upload firmware after changing calibration values.

Do not run full gait commands until all 12 joints have safe limits.

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

## Adding New Firmware Behavior

To add a new robot behavior:

1. Add or reuse a `MessageBehaviorKind` in `src/comm/message.h`.
2. Mirror it in `src/control/pluto_server/message.py`.
3. Add UI or speech trigger code in `src/control`.
4. Add firmware handling in the `KIND_BEHAVIOR` switch in `src/esp/main.cpp`.
5. Implement the motion in `src/esp/motion/gait.cpp` if it needs continuous updates.
6. Test over serial first when possible.
7. Test over UDP only after serial behavior is safe.

Keep behavior code non-blocking. Prefer stateful motion updated by the 20 ms gait loop instead of long delays inside message handling.
