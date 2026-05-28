# 💻 Software Sensors

Pluto currently has two firmware sensor abstractions: an ultrasonic distance sensor and an INMP441 I2S microphone.

## 🧠 Current Firmware Defaults

Current sensor defaults in [src/esp/main.cpp](src/esp/main.cpp):

```cpp
#define PLUTO_ENABLE_ULTRASONIC
// #define PLUTO_ENABLE_MICROPHONE
```

Current pin templates:

- Ultrasonic sensor: `SensorUltraSonic<5, 18>`
- Microphone, when re-enabled: `SensorMicrophone<26, 25, 33>`

Verify these pins against the physical wiring before flashing or powering the robot.

## 🧰 Sensor Bring-Up From Scratch

Bring up sensors after the ESP32, PCA9685, and basic servo tests work.

1. Confirm both sensors share ground with the ESP32.
2. Confirm the ultrasonic ECHO voltage is safe for the ESP32. Use a level shifter or voltage divider if needed.
3. Flash firmware with `PLUTO_ENABLE_ULTRASONIC` enabled first.
4. Open the serial monitor at `115200`.
5. Move an object in front of the ultrasonic sensor and confirm wall-stop behavior.
6. If microphone behavior is needed, re-enable `PLUTO_ENABLE_MICROPHONE` and `update_microphone_control(now)` in `main.cpp`, then tune the clap threshold before using it near powered servos.
7. If sensor behavior is unstable, test one sensor at a time by disabling the other feature flag.

## 📏 Ultrasonic Sensor

The ultrasonic abstraction is implemented in:

```text
src/esp/sensors/ultrasonic.h
```

It is designed around a non-blocking read pattern:

- `read_begin()` sends the trigger pulse.
- `read_end()` collects the measured pulse duration and converts it to distance.

The current firmware starts a read every 150 ms and checks the result roughly 10 ms later. If the measured distance is positive and below 35 cm, `stop_robot("wall too close")` is called.

The sensor is intended for simple front obstacle detection.

### 📝 Ultrasonic Integration Notes

- The firmware does not block while waiting for the ultrasonic echo.
- `read_begin()` starts the measurement.
- The main loop waits briefly before `read_end()`.
- The current stop condition applies whenever the distance is below the threshold.
- Current wall-stop threshold is 35 cm.

This sensor is a safety and interaction feature, not a full mapping sensor.

## 🎙️ Microphone

The microphone abstraction is implemented in:

```text
src/esp/sensors/microphone.h
```

It uses I2S and exposes:

```cpp
current_energy()
```

This returns a dimensionless relative audio energy value. The clap-control block exists in `main.cpp`, but the microphone feature flag is currently commented out and `update_microphone_control(now)` is not called from the loop.

Current commented clap-control settings:

- Clap threshold: `30000000`
- Clap cooldown: `800 ms`
- Behavior if re-enabled: if the robot is walking, a clap stops it; otherwise a clap starts walking.

### 📝 Microphone Integration Notes

- The ESP32 microphone code measures energy, not full speech.
- Speech recognition is intentionally handled by the controller computer.
- The clap threshold is empirical and should be tuned in the real environment.
- If the robot starts or stops accidentally, increase the threshold or add filtering.

## 🗣️ Speech Recognition

The ESP32 firmware does not run speech recognition. Spoken commands are handled on the controller computer by Vosk in:

```text
src/control/pluto_speech/speech.py
```

Currently supported phrases include:

- `pluto sit`
- `pluto stop`
- `pluto give paw`
- `pluto flip`
- `pluto bow`

The controller converts recognized phrases into shared protocol messages and sends them to the ESP32 when connected.

## 📚 Related Documentation

- [Wiring & Electrical](WIRING_ELECTRICAL.md)
- [ESP32 Firmware](src/esp/README.md)
- [Python Controller](src/control/README.md)
- [Troubleshooting](TROUBLESHOOTING.md)
