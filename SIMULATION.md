# 🎮 Simulation Notes

Pluto currently has two simulation-facing paths. They are useful for development, but they are not yet a single polished, high-fidelity simulator.

## 🎮 Current Simulation Paths

| Path | Files | Status |
| --- | --- | --- |
| Python PyBullet UI | `src/control/pluto_menu/simulation.py`, `src/control/sim_motion.py`, `src/control/gait.py` | Integrated into the NiceGUI control hub, but the visual mesh loading still expects older generic mesh filenames |
| MuJoCo C++ bridge | `CMakeLists.txt`, `src/sim/sim_main.cpp`, `src/sim/sim_gait.*`, `src/sim/sim_leg.*`, `src/sim/sim_mesh/pluto.xml` | Uses the current per-leg mesh set and simulation-side leg/gait abstractions based on the ESP concepts |

## 🎮 How A New Team Should Use Simulation

Use simulation to inspect concepts, not to prove that hardware walking is safe.

Recommended order:

1. Read `src/control/gait.py` and `src/control/robot_config.py` to understand Python-side gait assumptions.
2. Read `src/esp/motion/gait.cpp` to understand firmware-side gait assumptions.
3. Inspect `src/sim/sim_mesh/pluto.xml` to see how the current mesh set is represented in MuJoCo.
4. Use the PyBullet UI path for controller and UI experiments.
5. Use the MuJoCo bridge to inspect how ESP gait output maps to simulated actuators.
6. Treat simulation success as a sign to continue testing, not as proof that the physical robot is safe.
7. Validate all physical movement slowly on the real robot with conservative calibration.

## 🎮 Python PyBullet Path

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

To update this path, replace the generic mesh references in `sim_motion.py` with the current per-leg STL files and verify the visual frame offsets for each leg.

## 🎮 MuJoCo Path

The MuJoCo model is stored in:

```text
src/sim/sim_mesh/pluto.xml
```

It references the current mesh files:

- `body.stl`
- `tl_coxa.stl`, `tl_femur.stl`, `tl_tibia.stl`
- `tr_coxa.stl`, `tr_femur.stl`, `tr_tibia.stl`
- `bl_coxa.stl`, `bl_femur.stl`, `bl_tibia.stl`
- `br_coxa.stl`, `br_femur.stl`, `br_tibia.stl`

The C++ bridge is stored in:

```text
src/sim/sim_main.cpp
src/sim/sim_gait.cpp
src/sim/sim_gait.h
src/sim/sim_leg.h
src/sim/sim_leg_joint.h
CMakeLists.txt
```

It constructs simulation-side `Leg` objects, runs a simulation-side `GaitController`, converts current joint angles to radians, and writes them to 12 MuJoCo actuators.

### ▶️ Build and Run the MuJoCo Bridge

The MuJoCo bridge is built with the top-level `CMakeLists.txt`, not PlatformIO. MuJoCo must be downloaded separately, then CMake must be pointed at that local installation.

```bash
cmake -S . -B build -DMUJOCO_DIR=/path/to/mujoco
cmake --build build
./build/bin/pluto_sim
```

On Windows, the default path is `C:/mujoco`, and the build copies MuJoCo DLLs from `MUJOCO_DIR/bin` beside the executable. On macOS or Linux, pass the actual MuJoCo install path with `-DMUJOCO_DIR=...`.

The model path is compiled into the executable as:

```text
PLUTO_MODEL_PATH="${CMAKE_SOURCE_DIR}/src/sim/sim_mesh/pluto.xml"
```

The bridge currently supports keyboard commands:

| Key | Action |
| --- | --- |
| `F` | Move forward |
| `B` | Move backward |
| `Q` | Turn left |
| `E` | Turn right |
| `S` | Stop |
| `1` | Walk gait |
| `2` | Trot gait |
| `3` | Gallop gait |

## 🎮 Known Simulation Limitations

- The simulation is not yet a validated physical twin of Pluto.
- Mass, friction, joint constraints, servo torque, cable effects, and battery placement still need refinement.
- Python PyBullet and C++ MuJoCo paths are separate and not yet unified.
- The PyBullet visual mesh path needs to be updated to use the current per-leg mesh files.
- The MuJoCo bridge is not part of the PlatformIO ESP32 firmware build.
- The top-level `CMakeLists.txt` defines the MuJoCo executable and sets `PLUTO_MODEL_PATH` to `src/sim/sim_mesh/pluto.xml`.

## 🎮 Future Simulation Work

For a future team, useful improvements would be:

- Unify the Python and MuJoCo simulation assumptions.
- Replace old PyBullet generic mesh references with current per-leg meshes.
- Add documented build/run commands for the C++ MuJoCo bridge.
- Tune mass, friction, joint limits, and servo response against physical measurements.
- Add screenshots or videos showing expected simulation output.

## 📚 Related Documentation

- [Software Overview](SOFTWARE_OVERVIEW.md)
- [CAD Files](CAD_FILES.md)
- [Python Controller](src/control/README.md)
- [ESP32 Firmware](src/esp/README.md)
