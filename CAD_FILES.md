# 🧱 CAD Files

This page documents the CAD/STL files used to physically print Pluto. The printable robot parts are stored separately from the simplified simulation meshes.

## 🖨️ Physical 3D-Printing Files

Use this folder for fabrication:

```text
src/3D printing mesh/
```

The folder is organized by physical leg position:

| Folder | Purpose |
| --- | --- |
| [Front Left](<src/3D printing mesh/Front Left>) | Printable parts for the front-left leg |
| [Front Right](<src/3D printing mesh/Front Right>) | Printable parts for the front-right leg |
| [Back Left](<src/3D printing mesh/Back Left>) | Printable parts for the back-left leg |
| [Back Right](<src/3D printing mesh/Back Right>) | Printable parts for the back-right leg |

Other than the leg files there are files corresponding to the Main Body, the Board Holder and the Sensor Holder.\
Each leg folder contains the printable coxa, femur, tibia, spacer, and linkage/bar parts for that specific leg.

## 🖨️ Printable Parts By Leg

### 📌 Front Left

- [Front Left COXA.stl](<src/3D printing mesh/Front Left/Front Left COXA.stl>)
- [Front Left Inner FEMUR.stl](<src/3D printing mesh/Front Left/Front Left Inner FEMUR.stl>)
- [Front Left Outer FEMUR.stl](<src/3D printing mesh/Front Left/Front Left Outer FEMUR.stl>)
- [Front Left Spacer FEMUR.stl](<src/3D printing mesh/Front Left/Front Left Spacer FEMUR.stl>)
- [Front Left TIBIA.stl](<src/3D printing mesh/Front Left/Front Left TIBIA.stl>)
- [Front Left Inner Mirror TIBIA.stl](<src/3D printing mesh/Front Left/Front Left Inner Mirror TIBIA.stl>)
- [Front Left Bar.stl](<src/3D printing mesh/Front Left/Front Left Bar.stl>)

### 📌 Front Right

- [Front Right COXA.stl](<src/3D printing mesh/Front Right/Front Right COXA.stl>)
- [Front Right Inner FEMUR.stl](<src/3D printing mesh/Front Right/Front Right Inner FEMUR.stl>)
- [Front Right Outer FEMUR.stl](<src/3D printing mesh/Front Right/Front Right Outer FEMUR.stl>)
- [Front Right Spacer FEMUR.stl](<src/3D printing mesh/Front Right/Front Right Spacer FEMUR.stl>)
- [Front Right Inner TIBIA.stl](<src/3D printing mesh/Front Right/Front Right Inner TIBIA.stl>)
- [Front Right Inner Mirror TIBIA.stl](<src/3D printing mesh/Front Right/Front Right Inner Mirror TIBIA.stl>)
- [Front Right Bar.stl](<src/3D printing mesh/Front Right/Front Right Bar.stl>)

### 📌 Back Left

- [Back Left COXA.stl](<src/3D printing mesh/Back Left/Back Left COXA.stl>)
- [Back Left Inner FEMUR.stl](<src/3D printing mesh/Back Left/Back Left Inner FEMUR.stl>)
- [Back Left Outer FEMUR.stl](<src/3D printing mesh/Back Left/Back Left Outer FEMUR.stl>)
- [Back Left Spacer FEMUR.stl](<src/3D printing mesh/Back Left/Back Left Spacer FEMUR.stl>)
- [Back Left Inner TIBIA.stl](<src/3D printing mesh/Back Left/Back Left Inner TIBIA.stl>)
- [Back Left Inner Mirror TIBIA.stl](<src/3D printing mesh/Back Left/Back Left Inner Mirror TIBIA.stl>)
- [Back Left Bar.stl](<src/3D printing mesh/Back Left/Back Left Bar.stl>)

### 📌 Back Right

- [Back Right COXA.stl](<src/3D printing mesh/Back Right/Back Right COXA.stl>)
- [Back Right Inner FEMUR.stl](<src/3D printing mesh/Back Right/Back Right Inner FEMUR.stl>)
- [Back Right Outer FEMUR.stl](<src/3D printing mesh/Back Right/Back Right Outer FEMUR.stl>)
- [Back Right Spacer FEMUR.stl](<src/3D printing mesh/Back Right/Back Right Spacer FEMUR.stl>)
- [Back Right Inner TIBIA.stl](<src/3D printing mesh/Back Right/Back Right Inner TIBIA.stl>)
- [Back Right Inner Mirror TIBIA.stl](<src/3D printing mesh/Back Right/Back Right Inner Mirror TIBIA.stl>)
- [Back Right Bar.stl](<src/3D printing mesh/Back Right/Back Right Bar.stl>)

## 🧰 How To Use These Files From Scratch

1. Print one complete leg folder first, preferably the leg you will use for test fitting.
2. Assemble that one leg before printing all remaining parts.
3. Check servo fit, screw holes, bearing fit, spacer alignment, and linkage motion.
4. Label every printed part by leg position immediately after printing.
5. Do not mix front/back or left/right parts.
6. After mechanical assembly, verify that the physical leg positions match the firmware leg names in `src/esp/legs/leg_data.h`.

The printed parts and firmware calibration are connected. If a leg is mounted on the wrong side, the calibration values and servo channel mapping may no longer match the physical robot.

## 🧱 Simulation Meshes

Simulation-specific meshes are stored in:

```text
src/sim/sim_mesh/
```

These files are not the primary physical printing files. They are simplified/organized for simulation use and include:

- `body.stl`
- `tl_coxa.stl`, `tl_femur.stl`, `tl_tibia.stl`
- `tr_coxa.stl`, `tr_femur.stl`, `tr_tibia.stl`
- `bl_coxa.stl`, `bl_femur.stl`, `bl_tibia.stl`
- `br_coxa.stl`, `br_femur.stl`, `br_tibia.stl`
- `pluto.xml`

The MuJoCo model is [src/sim/sim_mesh/pluto.xml](src/sim/sim_mesh/pluto.xml).

See [SIMULATION.md](SIMULATION.md) for the current simulation split.

## 📝 Printing Notes

- Print rigid structural parts in PLA or PETG.
- Use TPU or rubber pads for the feet if available.
- Print and inspect one full leg before printing the entire set.
- Keep track of front/back and left/right orientation during assembly.
- Check bearing fits, screw holes, servo horn alignment, and linkage movement before final assembly.

## 🖨️ Recommended Print Validation

Before committing to the full robot:

- Confirm that the servo body fits without forcing the print.
- Confirm that the servo horn can be mounted without rubbing.
- Confirm that the tibia bearing sits flat.
- Confirm that screw holes can be used without cracking the part.
- Confirm that leg movement is smooth before electronics are powered.

## 📚 Related Documentation

- [Assembly Instructions](ASSEMBLY.md)
- [Wiring & Electrical](WIRING_ELECTRICAL.md)
- [Hardware Overview](HARDWARE_OVERVIEW.md)
- [Simulation Notes](SIMULATION.md)
