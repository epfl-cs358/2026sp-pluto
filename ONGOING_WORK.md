# Ongoing Works & Next Steps

Pluto is still under active development. The current codebase already provides the main building blocks for locomotion, simulation, sensing, and communication, but several parts need tuning and integration before the robot behaves as a robust autonomous platform.

## Current Focus

- **Hardware gait validation**: Test the walk, trot, gallop, and turn gaits on the physical robot and tune stride length, lift height, phase offsets, support push-down, and balance shifts.
- **Servo calibration**: Refine each joint's PWM limits, starting pulse, inversion flag, and angle range so IK commands map cleanly to safe physical motion.
- **Controller-to-robot integration**: Finish wiring the NiceGUI keyboard/gamepad controller to live `MOVE_BY` UDP messages and expose ESP32 connection settings in the UI.
- **Behavior implementation**: Replace placeholder behavior handlers for sit, give paw, and lie down with calibrated motion sequences.
- **Sensor-driven reactions**: Use ultrasonic readings to stop, slow down, or recover when an obstacle is too close.

## Improvements

- Add a formal robot state machine for idle, standing, walking, turning, behavior execution, and emergency stop.
- Smooth gait transitions so switching between stand, walk, trot, and stop does not create abrupt servo jumps.
- Add interpolation and trajectory limits around IK outputs to reduce mechanical stress.
- Feed microphone data into a complete on-robot or controller-side command pipeline.
- Add optional IMU feedback for body orientation and gait stability.
- Extend the simulator so it better matches the physical robot's mass, friction, joint limits, and servo behavior.
- Add a small telemetry view in the web UI for connection state, latest distance reading, gait mode, speed, and received acknowledgements.
- Document wiring, battery safety, mechanical assembly, and servo calibration procedures.

## Known Issues to Address

- Some ESP32 behavior handlers are currently placeholders.
- Movement vectors are computed in the controller UI but still need to be fully sent to the physical robot.
- Gait constants in the ESP firmware still need final physical measurements and tuning.
- The PyBullet model is useful for development, but it is not yet a high-fidelity dynamics model of the real robot.
- Camera, IMU-based stabilization, and higher-level autonomy are proposal/future-work items, not complete features in the current repository.

## Future Directions

- Improve autonomous obstacle avoidance using ultrasonic sensing and later camera-based perception.
- Add expressive behaviors such as crouching, paw gestures, recovery motions, and more reliable sitting/lying poses.
- Explore AprilTags, camera streaming, or simple visual navigation once the base locomotion is stable.
- Use Pluto as a reusable teaching platform for legged locomotion, robot control, and embedded sensing.
