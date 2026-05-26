# First-Time Troubleshooting

Use this guide when Pluto does not start cleanly. Start with the section that matches the symptom.

## Debugging Rule For A New Build

When building from scratch, debug in this order:

1. Mechanical binding.
2. Power and ground.
3. ESP32 flashing and serial output.
4. PCA9685 and one servo.
5. All servo channel mapping.
6. Servo calibration.
7. Sensors.
8. WiFi.
9. Controller UI.
10. Gait behavior.

Do not debug gait behavior before power, wiring, and calibration are known to be correct.

## Controller UI Does Not Start

### Python 3.13 Not Found

The run scripts require Python 3.13.

Check your version:

```bash
python3 --version
```

On Windows:

```cmd
py -3.13 --version
```

If Python 3.13 is missing, install it from <https://www.python.org/downloads/> and run the script again.

### Dependencies Did Not Install Correctly

Force a clean reinstall:

```bash
bash run.sh --reinstall
```

On Windows:

```cmd
run.bat --reinstall
```

This recreates `.venv` and reinstalls `requirements.txt`.

### Port 8090 Is Already In Use

Run the UI on another port:

```bash
bash run.sh --port 8081
```

On Windows:

```cmd
run.bat --port 8081
```

Then open `http://localhost:8081`.

## Simulation Does Not Open

### PyBullet UI

- Make sure you are running on a machine with a desktop display.
- If running over SSH or in a headless environment, PyBullet GUI mode may not open.
- Check that `pybullet` installed successfully in `.venv`.
- Try `bash run.sh --reinstall` if the package installation looks incomplete.
- Current limitation: `src/control/sim_motion.py` still references old generic visual mesh files. See [SIMULATION.md](SIMULATION.md).

### MuJoCo Bridge

- Check that `mujoco` and `glfw` installed successfully.
- Confirm that [src/mesh/pluto.xml](src/mesh/pluto.xml) can find the STL files in `src/mesh`.
- The C++ MuJoCo bridge under `src/sim` is separate from the ESP32 PlatformIO firmware build.

See [SIMULATION.md](SIMULATION.md).

## ESP32 Firmware Does Not Build Or Upload

### PlatformIO Is Missing

Install PlatformIO Core or use the PlatformIO VS Code extension.

Check:

```bash
pio --version
```

### Upload Port Is Not Detected

List connected devices:

```bash
pio device list
```

Upload with an explicit port:

```bash
pio run -t upload --upload-port /dev/cu.usbserial-XXXX
```

On Windows, the port usually looks like `COM3`, `COM4`, etc.

```cmd
pio run -t upload --upload-port COM3
```

### Serial Monitor Shows Nothing

Use the configured baud rate:

```bash
pio device monitor -b 115200
```

If the monitor still shows nothing, check the USB cable, board selection, and whether the ESP32 is powered.

## Robot Does Not Connect Over WiFi

WiFi is implemented but disabled by default in `src/esp/main.cpp`.

Check:

- `#define PLUTO_ENABLE_WIFI` is enabled.
- The WiFi network is added in `setup()` with `PLUTO_SERVER.addAP(<WIFI_NAME>, <WIFI_PASSWORD>)`.
- The computer and ESP32 are on the same network.
- The ESP32 serial monitor shows WiFi startup activity.
- `IP_OF_ESP` is set in `src/control/main.py`.
- UDP port `4242` is not blocked by the network or firewall.

For packet details, see [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md).

## Hardware Powers On But Servos Do Not Move

- Verify the PCA9685 is powered and connected to the ESP32 I2C lines.
- Check that servo power and logic ground share a common ground.
- Confirm the LiPo is charged and the rocker switch is on.
- Check that the servos are connected to the expected PCA9685 channels.
- Use the serial monitor to test simple firmware commands before trying full walking.
- Check `src/esp/legs/leg_data.h` for calibration limits and starting pulses.

Do not continue testing if a servo stalls, overheats, chatters heavily, or pulls the robot into a mechanically blocked position.

## Robot Resets Or Behaves Unstable

This is often a power issue.

- Check the LiPo charge level.
- Verify the buck converter output before connecting sensitive electronics.
- Use thick enough wires for servo power.
- Confirm XT60 and switch connections are secure.
- Confirm all grounds are connected together.
- Avoid testing full gaits until single-leg and standing poses are stable.

## Sensors Do Not Respond

- Check the sensor wiring against [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md).
- Verify the firmware pin templates:
  - Ultrasonic sensor: `SensorUltraSonic<5, 18>`
  - Microphone: `SensorMicrophone<26, 25, 33>`
- Confirm the sensors share ground with the ESP32.
- Use serial output to inspect raw readings before relying on reactive behavior.
- If the ultrasonic ECHO line is 5V, use a voltage divider or level shifter.

## Speech Commands Do Not Work

Speech recognition runs on the controller computer, not on the ESP32.

- Make sure a microphone is connected and allowed by the operating system.
- Check that `sounddevice` and `vosk` installed correctly.
- Run the controller from a normal desktop session, not a restricted terminal.
- Connect to the robot before expecting speech commands to be sent.
- Speak one of the supported commands clearly, such as `pluto stop`, `pluto sit`, or `pluto give paw`.

## Still Stuck

Check these docs next:

- [README.md](README.md)
- [ASSEMBLY.md](ASSEMBLY.md)
- [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md)
- [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md)
- [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md)
- [SIMULATION.md](SIMULATION.md)
- [ONGOING_WORK.md](ONGOING_WORK.md)
