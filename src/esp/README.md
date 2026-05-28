# 🧠 ESP32 Firmware

This directory contains Pluto's robot-side firmware.

## 🚪 Entry Point

```text
src/esp/main.cpp
```

PlatformIO is configured with `src_dir = src/esp`, so this directory is the firmware source root.

## 🧩 Main Modules

| Module | Purpose |
| --- | --- |
| `main.cpp` | Firmware setup, loop, feature flags, sensors, gait update, serial commands, and UDP dispatch |
| `legs/leg_data.h` | Per-joint PWM and angle calibration |
| `legs/leg_joint.h` | Calibrated servo output abstraction |
| `legs/leg.h` | Three-joint leg abstraction |
| `motion/ik_solver.cpp` | Embedded inverse kinematics |
| `motion/gait.cpp` | Stand, walk, trot, gallop, turn, flip, bow, sit, paw, and gait update logic |
| `sensors/ultrasonic.h` | Ultrasonic distance sensor abstraction |
| `sensors/microphone.h` | INMP441 I2S microphone abstraction |
| `server/server.cpp` | UDP server, mDNS advertisement, sessions, CRC validation, acknowledgements, and queues |

## 🧠 Firmware Execution Flow

This is the order in which the firmware operates:

1. `setup()` starts serial output at `115200`.
2. If WiFi is enabled, `PLUTO_SERVER` is configured, advertises `_pluto._udp.local`, and starts.
3. The PCA9685 is initialized and set to 50 Hz.
4. Enabled sensors are initialized.
5. `GAIT.stand(LEGS)` moves the robot into the starting stand pose.
6. `loop()` repeatedly reads microphone and ultrasonic state.
7. Every 20 ms, `GAIT.update(LEGS, now)` writes updated joint targets.
8. Serial input is checked and converted into gait, speed, trimming, or reset commands.
9. If WiFi is enabled, queued UDP messages are processed.

Understanding this flow is important before adding new behavior. Long blocking code in `loop()` can make gait updates irregular.

## 🎚️ Leg and Servo Mapping

Each leg uses four PCA9685 channel slots. The current firmware uses the first three slots for coxa, femur, and tibia:

| Leg | Coxa | Femur | Tibia | Unused |
| --- | ---: | ---: | ---: | ---: |
| `TOP_LEFT` | 0 | 1 | 2 | 3 |
| `TOP_RIGHT` | 4 | 5 | 6 | 7 |
| `BOTTOM_LEFT` | 8 | 9 | 10 | 11 |
| `BOTTOM_RIGHT` | 12 | 13 | 14 | 15 |

This mapping comes from `Leg::CHANNEL_STEPS_PER_SIDE = 4` in `legs/leg.h`.

## 🎚️ Servo Calibration Workflow

Calibration lives in:

```text
src/esp/legs/leg_data.h
```

Each joint has:

- `raw_min`: lowest safe raw PWM value.
- `raw_max`: highest safe raw PWM value.
- `raw_stand`: standing raw PWM value.
- `raw_forward`: forward-start raw PWM value.
- `raw_turnleft`: left-turn-start raw PWM value.
- `raw_turnright`: right-turn/backward-start raw PWM value.
- `raw_bow`: bow-start raw PWM value.
- `raw_sit`: sit-start raw PWM value.
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
7. Set `raw_stand` to a physically neutral standing value, then tune the motion-specific raw start values.
8. Confirm whether the logical direction needs `inverted = true`.
9. Repeat for all 12 joints.
10. Rebuild and upload firmware after changing calibration values.

Do not run full gait commands until all 12 joints have safe limits.

## 🚩 Feature Flags

Current defaults in `main.cpp`:

```cpp
#define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

That means WiFi/UDP, ultrasonic, and microphone support are enabled at compile time. WiFi still needs valid access point credentials in `setup()`.

## 📌 Timing

- Gait update: every 20 ms.
- Ultrasonic cycle: every 150 ms.
- Ultrasonic wait after trigger: 10 ms.
- Microphone clap-control code is present but currently commented out in `main.cpp`.
- Serial baud rate: 115200.

## 📡 Sensors

Current templates in `main.cpp`:

- Ultrasonic: `SensorUltraSonic<5, 18>`
- Microphone: `SensorMicrophone<26, 25, 33>`

Motion stops if ultrasonic distance is between 0 and 35 cm.

The microphone driver initializes, but the clap-toggle update call is currently commented out in `main.cpp`.

## ⌨️ Serial Commands

Use:

```bash
pio device monitor -b 115200
```

| Key | Action |
| --- | --- |
| `f` | Move forward |
| `b` | Move backward |
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

## 📶 WiFi/UDP

When `PLUTO_ENABLE_WIFI` is enabled, the firmware creates:

```cpp
auto PLUTO_SERVER = pluto::PlutoServer{4242};
```

Configure access points in `setup()` with:

```cpp
PLUTO_SERVER.addAP("<WIFI_NAME>", "<WIFI_PASSWORD>");
```

Queued UDP messages are processed in `loop()`. `MOVE_BY` commands apply a small deadzone and then map to one of the existing movement commands:

- positive forward/back value: walk forward
- negative forward/back value: walk backward
- negative left/right value: turn left
- positive left/right value: turn right
- both axes inside the deadzone: stop and stand

Behavior messages currently map as follows:

- `BEHAVIOR_SIT`: stop/stand
- `BEHAVIOR_GIVE_PAW`: paw motion
- `BEHAVIOR_LIE_DOWN`: bow motion

See [WiFi Protocol](../../SOFTWARE_WIFI.md) and [Shared Communication Protocol](../comm/README.md).

## 📌 Build and Upload

From the repository root:

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## 🧠 Adding New Firmware Behavior

To add a new robot behavior:

1. Add or reuse a `MessageBehaviorKind` in `src/comm/message.h`.
2. Mirror it in `src/control/pluto_server/message.py`.
3. Add UI or speech trigger code in `src/control`.
4. Add firmware handling in the `KIND_BEHAVIOR` switch in `src/esp/main.cpp`.
5. Implement the motion in `src/esp/motion/gait.cpp` if it needs continuous updates.
6. Test over serial first when possible.
7. Test over UDP only after serial behavior is safe.

Keep behavior code non-blocking. Prefer stateful motion updated by the 20 ms gait loop instead of long delays inside message handling.
