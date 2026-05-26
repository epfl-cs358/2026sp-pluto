# Simulation Notes

Pluto currently has two simulation-facing paths. They are useful for development, but they are not yet a single polished, high-fidelity simulator.

## Current Simulation Paths

| Path | Files | Status |
| --- | --- | --- |
| Python PyBullet UI | `src/control/pluto_menu/simulation.py`, `src/control/sim_motion.py`, `src/control/gait.py` | Integrated into the NiceGUI control hub, but the visual mesh loading still expects older generic mesh filenames |
| MuJoCo C++ bridge | `src/sim/sim.cpp`, `src/sim/MockPWMServoDriver.h`, `src/mesh/pluto.xml` | Uses the current per-leg mesh set and reuses the ESP gait controller through a mock PWM driver |

## Python PyBullet Path

The `/sim` page in the NiceGUI app can open a PyBullet window and send basic motion commands through `PyBulletMotionController`.

Important files:

- `src/control/pluto_menu/simulation.py`: NiceGUI page and motion buttons.
- `src/control/sim_motion.py`: PyBullet process, robot construction, and command queue.
- `src/control/gait.py`: Python gait definitions.
- `src/control/robot_config.py`: Python simulation geometry constants.

Current limitation:

`src/control/sim_motion.py` still references generic visual mesh files:

- `coxa.stl`
- `femur.stl`
- `tibia.stl`

Those files are not part of the current mesh set. The active repository now stores per-leg files such as `tl_coxa.stl`, `tr_femur.stl`, and `br_tibia.stl`. Until `sim_motion.py` is updated, the PyBullet visual mesh path may fail or render without the intended current parts.

## MuJoCo Path

The MuJoCo model is stored in:

```text
src/mesh/pluto.xml
```

It references the current mesh files:

- `body.stl`
- `tl_coxa.stl`, `tl_femur.stl`, `tl_tibia.stl`
- `tr_coxa.stl`, `tr_femur.stl`, `tr_tibia.stl`
- `bl_coxa.stl`, `bl_femur.stl`, `bl_tibia.stl`
- `br_coxa.stl`, `br_femur.stl`, `br_tibia.stl`

The C++ bridge is stored in:

```text
src/sim/sim.cpp
src/sim/MockPWMServoDriver.h
```

It creates a mock PWM driver, constructs the same ESP `Leg` objects used by firmware, runs the `GaitController`, converts current joint angles to radians, and writes them to 12 MuJoCo actuators.

The bridge currently supports keyboard commands:

| Key | Action |
| --- | --- |
| `F` | Move forward |
| `B` | Move backward |
| `S` | Stop |
| `1` | Walk gait |
| `2` | Trot gait |
| `3` | Gallop gait |

## Known Simulation Limitations

- The simulation is not yet a validated physical twin of Pluto.
- Mass, friction, joint constraints, servo torque, cable effects, and battery placement still need refinement.
- Python PyBullet and C++ MuJoCo paths are separate and not yet unified.
- The PyBullet visual mesh path needs to be updated to use the current per-leg mesh files.
- The MuJoCo bridge is not part of the PlatformIO ESP32 firmware build.

## Related Documentation

- [Software Overview](SOFTWARE_OVERVIEW.md)
- [CAD Files](CAD_FILES.md)
- [Python Controller](src/control/README.md)
- [ESP32 Firmware](src/esp/README.md)
