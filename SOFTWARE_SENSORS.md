## Sensors:
Pluto is currently equipped with 2 sensors:
- ultrasonic sensor (HC-SR04, on the front of the robot), measuring the distances to the obstacles,
- microphone (INMP441, top middle of the body), for speech recognition.

## Ultrasonic Sensor:
An abstraction for the ultrasonic sensor exists in `src/esp/sensors/ultrasonic.h`. The abstaction is non-blocking (done by using hardware interrupts): `read_begin()` issues the pulse to trigger the sensor, `read_end()` collects the data when ready.

The maximum effective range is limited by the maximum wait time (ULTRASONIC_WAIT_TIME), and is by default `85.75 cm`.

## Microphone:
An abstraction for the microphone sensor exists in `src/esp/sensors/microphone.h`. The microphone communicates with the ESP using [I2S](https://en.wikipedia.org/wiki/I2S). The abstraction has `current_energy()` that returns a dimensionless relative indicator of signal amplitude: this was to be used to enable the speech recognition engine when sounds of sufficient amplitudes are detected.

The current ESP code does not have a speech recognition engine. When enabling the WiFi server (TODO: insert link to section!), the ESP heats up quite a lot. To avoid damaging it, we decided to replace the on-chip speech recognition with on-controller speech recognition (thus using the microphone of the controller computer), sending to the ESP the matching command to execute. Using a more powerfull ESP, and one that better supports machine-learning workflow could circumvent this limitation.