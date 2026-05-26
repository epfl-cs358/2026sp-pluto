# First-Time Troubleshooting

Use this guide when Pluto does not start cleanly the first time. Start with the section that matches the symptom.

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

## PyBullet Simulation Does Not Open

- Make sure you are running on a machine with a desktop display.
- If running over SSH or a headless environment, PyBullet GUI mode may not open.
- Check that `pybullet` installed successfully in `.venv`.
- Try `--reinstall` if the package installation looks incomplete.

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

- Confirm the WiFi network is added in `src/esp/main.cpp` with `PLUTO_SERVER.addAP(<WIFI_NAME>, <WIFI_PASSWORD>)`.
- Confirm the computer and ESP32 are on the same network.
- Check the ESP32 serial monitor for connection logs.
- Set the ESP32 IP address in `src/control/main.py` where `PlutoController(IP_OF_ESP)` is created.
- Make sure UDP port `4242` is not blocked by the network or firewall.

For packet details, see [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md).

## Hardware Powers On But Servos Do Not Move

- Verify the PCA9685 is powered and connected to the ESP32 I2C lines.
- Check that servo power and logic ground share a common ground.
- Confirm the LiPo is charged and the rocker switch is on.
- Check that the servos are connected to the expected PCA9685 channels.
- Use the serial monitor to test simple firmware commands before trying full walking.

Do not continue testing if a servo stalls, overheats, chatters heavily, or pulls the robot into a mechanically blocked position.

## Robot Resets Or Behaves Unstable

This is often a power issue.

- Check the LiPo charge level.
- Verify the buck converter output before connecting sensitive electronics.
- Use thick enough wires for servo power.
- Confirm XT60 and switch connections are secure.
- Avoid testing full gaits until single-leg and standing poses are stable.

## Sensors Do Not Respond

- Check the sensor wiring against [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md).
- Verify the firmware pin templates:
  - Ultrasonic sensor: `SensorUltraSonic<21, 22>`
  - Microphone: `SensorMicrophone<26, 25, 33>`
- Confirm the sensors share ground with the ESP32.
- Use serial output to inspect raw readings before relying on reactive behavior.

## Speech Commands Do Not Work

- Make sure a microphone is connected and allowed by the operating system.
- Check that `sounddevice` and `vosk` installed correctly.
- Run the controller from a normal desktop session, not a restricted terminal.
- Speak one of the supported commands clearly, such as `pluto stop`, `pluto sit`, or `pluto give paw`.

## Still Stuck

Check these docs next:

- [README.md](README.md) for the main setup flow
- [ASSEMBLY.md](ASSEMBLY.md) for wiring and mechanical assembly
- [SOFTWARE_WIFI.md](SOFTWARE_WIFI.md) for UDP communication
- [SOFTWARE_SENSORS.md](SOFTWARE_SENSORS.md) for sensor setup
- [ONGOING_WORK.md](ONGOING_WORK.md) for known limitations and future work
