# CAD Files

This page lists the mesh assets currently available in the repository and how they are used by Pluto.

## Current Mesh Directory

All current mesh and simulation model files are stored in [src/mesh](src/mesh).

| File | Purpose |
| --- | --- |
| [body.stl](src/mesh/body.stl) | Main robot body for electronics and leg mounting |
| [tl_coxa.stl](src/mesh/tl_coxa.stl) | Top-left coxa segment |
| [tl_femur.stl](src/mesh/tl_femur.stl) | Top-left femur segment |
| [tl_tibia.stl](src/mesh/tl_tibia.stl) | Top-left tibia segment |
| [tr_coxa.stl](src/mesh/tr_coxa.stl) | Top-right coxa segment |
| [tr_femur.stl](src/mesh/tr_femur.stl) | Top-right femur segment |
| [tr_tibia.stl](src/mesh/tr_tibia.stl) | Top-right tibia segment |
| [bl_coxa.stl](src/mesh/bl_coxa.stl) | Bottom-left coxa segment |
| [bl_femur.stl](src/mesh/bl_femur.stl) | Bottom-left femur segment |
| [bl_tibia.stl](src/mesh/bl_tibia.stl) | Bottom-left tibia segment |
| [br_coxa.stl](src/mesh/br_coxa.stl) | Bottom-right coxa segment |
| [br_femur.stl](src/mesh/br_femur.stl) | Bottom-right femur segment |
| [br_tibia.stl](src/mesh/br_tibia.stl) | Bottom-right tibia segment |
| [pluto.xml](src/mesh/pluto.xml) | MuJoCo model that references the current STL files |

The previous generic files `coxa.stl`, `Femur.stl`, and `tibia.stl` are no longer part of the active mesh set.

## Naming Convention

- `tl`: top left
- `tr`: top right
- `bl`: bottom left
- `br`: bottom right

The repository uses top/bottom naming in several mesh and firmware files. In the physical robot documentation, this corresponds to front/back placement depending on how the robot is oriented during assembly.

## Simulation Usage

The MuJoCo model at [src/mesh/pluto.xml](src/mesh/pluto.xml) references the current per-leg STL files.

The older Python PyBullet path in `src/control/sim_motion.py` still expects generic `coxa.stl`, `femur.stl`, and `tibia.stl` visual meshes. That path needs to be updated before it fully matches the current CAD files.

See [SIMULATION.md](SIMULATION.md) for the current simulation split.

## Printing Notes

- Print rigid structural parts in PLA or PETG.
- Use TPU or rubber pads for the feet if available.
- Print and inspect all leg parts before inserting servos.
- Keep track of left/right and top/bottom orientation during assembly.
- Check bearing fits, screw holes, and servo horn alignment before final assembly.

## Related Documentation

- [Assembly Instructions](ASSEMBLY.md)
- [Wiring & Electrical](WIRING_ELECTRICAL.md)
- [Hardware Overview](HARDWARE_OVERVIEW.md)
- [Simulation Notes](SIMULATION.md)
