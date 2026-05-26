# CAD Files

This page lists the CAD/mesh assets currently available in the repository and how they are used in Pluto.

## Available Mesh Files

The current STL files are stored in [src/mesh](src/mesh):

| File | Purpose |
| --- | --- |
| [body.stl](src/mesh/body.stl) | Main robot body for electronics and leg mounting |
| [coxa.stl](src/mesh/coxa.stl) | Hip yaw / coxa leg segment |
| [Femur.stl](src/mesh/Femur.stl) | Upper-leg / femur segment |
| [tibia.stl](src/mesh/tibia.stl) | Lower-leg / tibia segment |

These same meshes are also used by the PyBullet simulation.

## Printing Notes

- Print rigid structural parts in PLA or PETG.
- Use TPU or rubber pads for the feet if available.
- Print and inspect all leg parts before inserting servos.
- Check bearing fits and screw holes before final assembly.
- Keep the left/right and front/back orientation consistent during assembly.

## Assembly Links

- [Assembly Instructions](ASSEMBLY.md): physical build steps
- [Wiring & Electrical](WIRING_ELECTRICAL.md): circuit and power wiring
- [Hardware Overview](HARDWARE_OVERVIEW.md): hardware system summary
