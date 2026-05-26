# 🔌 Wiring & Electrical

This page summarizes Pluto's wiring and electrical setup. For the physical assembly sequence, see [Assembly Instructions](ASSEMBLY.md).

## 🔌 Circuit Diagram

![Circuit Diagram](images/circuit.png)

Use this diagram as the reference for the current wiring layout, then verify every connection against the actual hardware before powering the robot.

## 🔋 Power Path

Pluto uses a 7.4V 2S LiPo battery as the main power source.

Typical power path:

```text
7.4V 2S LiPo
  -> XT60 connector
  -> KCD1 rocker switch
  -> servo power rail / PCA9685 servo power
  -> LM2596 buck converter
  -> ESP32 and low-voltage electronics
```

Important checks:

- Use thick wires for high-current servo power.
- Confirm LiPo polarity before plugging in the battery.
- Adjust and verify the LM2596 output voltage with a multimeter before connecting the ESP32.
- Connect all grounds together: LiPo/servo ground, PCA9685 ground, ESP32 ground, buck converter ground, and sensor ground.
- Do not test full gaits until standing poses and single-leg motion are stable.

## 🧰 Wiring Order From Scratch

Wire the robot in this order. Each step should be verified before continuing:

1. Battery connector and rocker switch.
2. Servo power rail and PCA9685 servo power input.
3. Buck converter input from the switched battery side.
4. Buck converter output, measured before connecting electronics.
5. ESP32 regulated power and ground.
6. PCA9685 logic power, ground, SDA, and SCL.
7. One servo channel for initial testing.
8. Remaining servo channels after the first channel works.
9. Ultrasonic sensor power, ground, trigger, and echo.
10. Microphone I2S power, ground, and signal pins.

If a later step fails, disconnect power and return to the last verified step.

## 📶 ESP32 Connections

The ESP32 is the main controller. It handles firmware execution, sensor reads, serial debugging, optional WiFi, and communication with the PCA9685 servo driver.

Check these before powering the full robot:

- ESP32 is connected over USB for flashing and serial monitoring.
- ESP32 receives a safe regulated voltage from the buck converter when running from battery.
- ESP32 and PCA9685 share I2C SDA/SCL and common ground.
- Sensor pins match the templates in `src/esp/main.cpp`.
- WiFi credentials are configured in `src/esp/main.cpp` only if `PLUTO_ENABLE_WIFI` is enabled.

## 🔌 PCA9685 and Servo Wiring

The PCA9685 drives all 12 servos.

- Connect PCA9685 logic power to the ESP32-compatible logic supply.
- Connect PCA9685 servo power to the servo power rail.
- Keep servo power wiring short and thick enough for current spikes.
- Connect each servo to the expected PCA9685 channel before calibration.
- Check servo orientation mechanically before sending large movements.

## 🎚️ Servo Channel Mapping

The firmware assigns channels from `src/esp/legs/leg.h`. Each leg uses four channel slots, but only the first three are used for coxa, femur, and tibia.

| Leg | Coxa channel | Femur channel | Tibia channel | Unused slot |
| --- | ---: | ---: | ---: | ---: |
| `TOP_LEFT` | 0 | 1 | 2 | 3 |
| `TOP_RIGHT` | 4 | 5 | 6 | 7 |
| `BOTTOM_LEFT` | 8 | 9 | 10 | 11 |
| `BOTTOM_RIGHT` | 12 | 13 | 14 | 15 |

Before running gait commands, verify that each physical servo is plugged into the channel expected by this table. If a servo is on the wrong channel, the IK and gait code will move the wrong joint.

## 📡 Sensors

Current sensor notes:

- Ultrasonic sensor: HC-SR04-style sensor, mounted at the front of the robot.
- Microphone: INMP441 I2S microphone, mounted near the top-middle of the body.

Current firmware pin templates:

- Ultrasonic sensor: `SensorUltraSonic<5, 18>`
- Microphone: `SensorMicrophone<26, 25, 33>`

If the ultrasonic ECHO line outputs 5V, use a voltage divider or level shifter before connecting it to an ESP32 GPIO.

See [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) for software-side sensor behavior.

## 🧪 Minimum Electrical Tests

Before full robot testing:

1. With battery disconnected, check continuity for ground paths.
2. With servos disconnected, power the logic electronics and verify ESP32 startup.
3. Confirm the PCA9685 is detected by firmware and receives logic power.
4. Connect one servo and test small raw changes from the serial monitor.
5. Connect the remaining servos only after the first servo behaves correctly.
6. Test the ultrasonic sensor separately before relying on wall-stop behavior.
7. Test microphone energy prints before relying on clap-toggle behavior.

## ✅ First Power-Up Checklist

1. Inspect solder joints and connector polarity.
2. Check for shorts with a multimeter.
3. Verify the buck converter output before connecting low-voltage electronics.
4. Connect the ESP32 over USB and confirm firmware upload/serial monitor works.
5. Connect the LiPo through XT60.
6. Turn on the rocker switch.
7. Confirm ESP32, PCA9685, sensors, and servos power up normally.
8. Test simple serial commands before running full locomotion.

If something behaves unexpectedly, turn the robot off and see [First-Time Troubleshooting](TROUBLESHOOTING.md).
