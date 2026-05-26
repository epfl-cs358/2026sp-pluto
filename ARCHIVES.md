# 🗄️ Archives

This repository does not currently contain a dedicated `archives/` directory.

The purpose of a future archive directory would be to preserve previous prototypes, tests, and implementations that are no longer part of the active Pluto codebase, while keeping them available for reference.

## 📌 What Should Be Archived

Use an archive folder for work that is useful historically but should not be part of the active runtime path:

- early gait experiments that were replaced by the current gait controller
- old inverse-kinematics prototypes
- temporary servo calibration sketches
- sensor bring-up tests
- older PyBullet or MuJoCo experiments
- communication protocol experiments that were replaced by the current UDP packet format
- hardware test scripts that require outdated wiring or assumptions

## 🗂️ Suggested Structure

If archives are added later, use a structure like:

```text
archives/
|-- esp/
|   |-- gait_tests/
|   |-- servo_calibration/
|   `-- sensor_tests/
|-- control/
|   |-- simulation_tests/
|   `-- ui_prototypes/
`-- comm/
    `-- protocol_experiments/
```

Each archived subdirectory should include a short `README.md` explaining:

- what was tested
- why it is no longer active
- whether it still runs
- which current implementation replaced it

## 🧪 Current Active Test References

At the moment, first-time validation is documented through:

- [README.md](README.md): quick start, firmware flashing, and setup flow
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md): first-time setup issues
- [ONGOING_WORK.md](ONGOING_WORK.md): known limitations and planned improvements
- [SIMULATION.md](SIMULATION.md): current simulation split and known limitations
- [src/esp/main.cpp](src/esp/main.cpp): serial commands for early gait and servo testing
