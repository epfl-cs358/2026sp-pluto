# Ongoing Works & Next Steps

Pluto is still under active development. The current codebase already provides the main building blocks for locomotion, simulation, sensing, and communication, but several parts need tuning and integration before the robot behaves as a robust autonomous platform.

## Current Focus

- Hardware gait validation: test walk, trot, gallop, bow, paw, and stop on the physical robot.
- Servo calibration: refine each joint's PWM limits, starting pulse, inversion flag, and angle range so IK commands map safely to physical motion.
- WiFi control validation: enable `PLUTO_ENABLE_WIFI`, set the ESP32 IP in the Python controller, and test live UDP messages on hardware.
- Behavior implementation: replace placeholder sit, give paw, and lie-down handlers with calibrated motion sequences.
- Sensor-driven reactions: tune ultrasonic wall stopping and microphone clap detection on the actual robot.
- Simulation cleanup: update the Python PyBullet visual mesh path to use the current per-leg STL files and continue refining the MuJoCo model.

## Improvements

- Add a formal robot state machine for idle, standing, walking, behavior execution, and emergency stop.
- Smooth gait transitions so switching between stand, walk, trot, gallop, and stop does not create abrupt servo jumps.
- Add interpolation and trajectory limits around IK outputs to reduce mechanical stress.
- Expose ESP32 IP, connection state, and WiFi status in the controller UI.
- Add optional IMU feedback for body orientation and gait stability.
- Extend simulation so it better matches physical mass, friction, joint limits, and servo behavior.
- Add a telemetry view for connection state, latest distance reading, gait mode, speed, and acknowledgements.
- Document final servo channel mapping after hardware validation.

## Known Issues

- WiFi support is implemented but disabled by default in `src/esp/main.cpp`.
- Some ESP32 behavior handlers currently print placeholder messages.
- The ESP32 IP address in `src/control/main.py` is still a placeholder.
- Gait constants in ESP firmware still need final physical measurements and tuning.
- The Python PyBullet visual path still references old generic mesh filenames.
- MuJoCo support is useful for development, but it is not yet a validated physical twin.
- Camera, IMU-based stabilization, SLAM, and higher-level autonomy are future-work items, not complete features in the current repository.

## Future Directions

- Improve autonomous obstacle avoidance using ultrasonic sensing and later camera-based perception.
- Add expressive behaviors such as crouching, paw gestures, recovery motions, and more reliable sitting/lying poses.
- Explore AprilTags, camera streaming, or simple visual navigation once the base locomotion is stable.
- Use Pluto as a reusable teaching platform for legged locomotion, robot control, and embedded sensing.

## Related Documentation

- [README.md](README.md)
- [SOFTWARE_OVERVIEW.md](SOFTWARE_OVERVIEW.md)
- [SIMULATION.md](SIMULATION.md)
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
