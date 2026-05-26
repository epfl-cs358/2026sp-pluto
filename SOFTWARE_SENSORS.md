# Software Sensors

Pluto currently has two firmware sensor abstractions: an ultrasonic distance sensor and an INMP441 I2S microphone.

## Current Firmware Defaults

Sensor support is enabled in [src/esp/main.cpp](src/esp/main.cpp):

```cpp
#define PLUTO_ENABLE_ULTRASONIC
#define PLUTO_ENABLE_MICROPHONE
```

Current pin templates:

- Ultrasonic sensor: `SensorUltraSonic<5, 18>`
- Microphone: `SensorMicrophone<26, 25, 33>`

Verify these pins against the physical wiring before flashing or powering the robot.

## Ultrasonic Sensor

The ultrasonic abstraction is implemented in:

```text
src/esp/sensors/ultrasonic.h
```

It is designed around a non-blocking read pattern:

- `read_begin()` sends the trigger pulse.
- `read_end()` collects the measured pulse duration and converts it to distance.

The current firmware starts a read every 150 ms and checks the result roughly 10 ms later. If the robot is moving forward and the measured distance is positive and below 20 cm, `stop_robot("wall too close")` is called.

The sensor is intended for simple front obstacle detection, not full mapping or SLAM.

## Microphone

The microphone abstraction is implemented in:

```text
src/esp/sensors/microphone.h
```

It uses I2S and exposes:

```cpp
current_energy()
```

This returns a dimensionless relative audio energy value. In the current firmware, that energy is printed periodically and used as a clap detector:

- Clap threshold: `2000000`
- Clap cooldown: `800 ms`
- Behavior: if the robot is walking, a clap stops it; otherwise a clap starts walking.

## Speech Recognition

The ESP32 firmware does not run speech recognition. Spoken commands are handled on the controller computer by Vosk in:

```text
src/control/pluto_speech/speech.py
```

Currently supported phrases include:

- `pluto sit`
- `pluto stop`
- `pluto give paw`

The controller converts recognized phrases into shared protocol messages and sends them to the ESP32 when connected.

## Related Documentation

- [Wiring & Electrical](WIRING_ELECTRICAL.md)
- [ESP32 Firmware](src/esp/README.md)
- [Python Controller](src/control/README.md)
- [Troubleshooting](TROUBLESHOOTING.md)
