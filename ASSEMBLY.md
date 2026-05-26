# Assembly Instructions

Follow these steps to assemble Pluto. Read [Wiring & Electrical](WIRING_ELECTRICAL.md) before powering anything.

## 1. Print Parts

Print the required body and leg parts from the current mesh set in [src/mesh](src/mesh). The active mesh inventory is documented in [CAD_FILES.md](CAD_FILES.md).

Current part groups:

- Body: `body.stl`
- Top-left leg: `tl_coxa.stl`, `tl_femur.stl`, `tl_tibia.stl`
- Top-right leg: `tr_coxa.stl`, `tr_femur.stl`, `tr_tibia.stl`
- Bottom-left leg: `bl_coxa.stl`, `bl_femur.stl`, `bl_tibia.stl`
- Bottom-right leg: `br_coxa.stl`, `br_femur.stl`, `br_tibia.stl`

Print rigid body and leg parts in PLA or PETG. Use TPU or rubber pads for the feet if available.

## 2. Prepare Electronics

Before installing electronics in the body:

1. Adjust the LM2596 buck converter output with a multimeter.
2. Confirm LiPo polarity and XT60 wiring.
3. Confirm the PCA9685 logic and servo power paths.
4. Confirm all grounds will be connected together.
5. Check the ultrasonic ECHO voltage level before connecting it to an ESP32 GPIO.

Full wiring guide: [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md)

## 3. Assemble Legs

Each leg has three main sections:

- Coxa: hip joint.
- Femur: upper leg.
- Tibia: lower leg.

Each leg requires:

- 3 servos.
- 3 servo horns or servo arms.
- M3 screws for most mechanical connections.
- Bearings and linkage hardware for the tibia mechanism.
- TPU foot or rubber pad.

### Step 1: Assemble the Tibia

- Take the two tibia printed parts.
- Insert a ball bearing between both parts.
- Align the pieces and screw them together.
- Glue or fasten the TPU/rubber foot to the lower end.

<img width="1200" height="1600" alt="Tibia assembly photo 1" src="https://github.com/user-attachments/assets/5c1d1c69-fc43-4d7c-b345-83be95ee043b" />
<img width="1200" height="1600" alt="Tibia assembly photo 2" src="https://github.com/user-attachments/assets/d6e00f19-be81-4896-a018-c532c6961204" />
<img width="1600" height="1200" alt="Tibia assembly photo 3" src="https://github.com/user-attachments/assets/1ceb050d-0b98-4518-9f59-d70ab6b6df12" />

### Step 2: Assemble the Femur

- Take the femur inner part, spacer, and outer part.
- Place the servo between the femur sides.
- Stack the three parts around the servo.
- Add adjacent screws to reinforce the structure and improve stability.

<img width="1200" height="1600" alt="Femur assembly photo 1" src="https://github.com/user-attachments/assets/275714d3-8c09-4059-b844-c13d53d6eaf3" />
<img width="1600" height="1200" alt="Femur assembly photo 2" src="https://github.com/user-attachments/assets/2ba8f411-c47c-49eb-b998-4faf3ca9ae12" />

### Step 3: Assemble the Coxa

- Take the coxa printed part for the selected leg.
- Insert the screw from the correct side for that leg's geometry.
- Attach the coxa to the servo horn mounted on the body.
- Mount the coxa servo and check that the open sections are aligned.

### Step 4: Attach the Femur to the Coxa

- Take the assembled femur.
- Align the highest hole on the femur outer part with the coxa servo horn.
- Secure the connection using an M3x35 screw.

<img width="1200" height="1600" alt="Femur-to-coxa assembly photo" src="https://github.com/user-attachments/assets/7dc58764-0051-4da9-8161-94ccf5f3cd5d" />

### Step 5: Attach the Tibia to the Femur

- Attach the printed linkage bar to the femur servo horn using the side with the single hole.
- Connect the opposite side of the linkage bar to the upper tibia part.
- Choose the linkage hole position based on the desired bend:
  - Holes closer to the center create a smaller bending angle.
  - Holes farther away create a larger bending angle.

<img width="1200" height="1600" alt="Tibia-to-femur linkage photo" src="https://github.com/user-attachments/assets/0e9bcfb1-e1ee-40d8-9137-1f8b7ccbd079" />

### Step 6: Repeat for All Legs

Repeat the previous steps for each leg. Check:

- Servo orientation.
- Screw tightness.
- Smooth joint movement.
- Correct linkage alignment.
- No mechanical blocking across the expected range.

<img width="1200" height="1600" alt="Leg assembly photo 1" src="https://github.com/user-attachments/assets/d05dc0d8-0eac-4b16-8236-bd71493f70c2" />
<img width="1200" height="1600" alt="Leg assembly photo 2" src="https://github.com/user-attachments/assets/0a05987a-258b-4da1-b0e4-094438c27b35" />

## 4. Assemble the Body

Use M3 screws unless a part requires a different size. Coxa servos use M2.5 screws.

1. Screw the coxa servo motors onto the body.
2. Mount the PCA9685 servo driver.
3. Mount the buck converter.
4. Slide the ultrasonic sensor into its holder and secure it.
5. Slide the protection board into its holder and secure it.
6. Attach the LiPo with Velcro or another removable mount.
7. Mount the microphone if used.
8. Slide or mount the ESP32 into the body.

Reference images:

![Servo](images/servo.jpg)
![Buck Converter](images/buckConverter.jpg)
![LiPo Protection](images/lipoProt.jpg)
![ESP32](images/esp.jpg)

## 5. Wire and Test

1. Plug all cables according to [WIRING_ELECTRICAL.md](WIRING_ELECTRICAL.md).
2. Check polarity and continuity with a multimeter.
3. Flash the ESP32 firmware.
4. Open the serial monitor at `115200`.
5. Test stand, stop, and small trimming commands before walking.
6. Tune servo calibration in `src/esp/legs/leg_data.h`.

Software instructions: [SOFTWARE_OVERVIEW.md](SOFTWARE_OVERVIEW.md)

Troubleshooting: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
