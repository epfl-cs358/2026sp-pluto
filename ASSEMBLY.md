# Assembly Instructions 
Follow these steps to assemble Pluto : 

## 1. 3D Print parts 
Print the parts present in the Fusion file. 

## 2. Assemble electrical circuit 
Read the full wiring guide [here](WIRING_ELECTRICAL.md)

## 3. Assemble legs 
Before starting, make sure to 3D-print all required parts from the CAD files.
Each leg is assembled in three main sections:
- COXA (hip joint)
- FEMUR (upper leg)
- TIBIA (lower leg)

Each leg requires:
- 3 servos
- 3 servo drivers / servo horns (one per servo)
- M3x20 screws for all connections unless stated otherwise

### Step 1: Assemble the TIBIA
- Take the two 3D-printed TIBIA parts.
- Insert a ball bearing between both parts.
- Align the pieces and screw them together.
- Glue the TPU foot to the lower end of the TIBIA assembly.
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 37 (2)" src="https://github.com/user-attachments/assets/5c1d1c69-fc43-4d7c-b345-83be95ee043b" />
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 37 (3)" src="https://github.com/user-attachments/assets/d6e00f19-be81-4896-a018-c532c6961204" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-05-26 at 14 35 38" src="https://github.com/user-attachments/assets/1ceb050d-0b98-4518-9f59-d70ab6b6df12" />

### Step 2: Assemble the FEMUR
- Take the three FEMUR parts: Inner part, Spacer, Outer part
- Place the servo between the FEMUR sides.
- Stack (“empile”) the three parts together around the servo.
- Add two adjacent screws to reinforce the structure and improve stability.
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 36 (1)" src="https://github.com/user-attachments/assets/275714d3-8c09-4059-b844-c13d53d6eaf3" />
<img width="1600" height="1200" alt="WhatsApp Image 2026-05-26 at 14 35 37 (1)" src="https://github.com/user-attachments/assets/2ba8f411-c47c-49eb-b998-4faf3ca9ae12" />

### Step 3: Assemble the COXA
- Take the 3D-printed COXA part.
- Insert one screw into the dedicated hole:
- Front legs: insert the screw from the side of the shorter COXA section.
  Back legs: insert the screw from the side of the taller COXA section.
- Screw this into the servo driver attached to the body.
- Mount the COXA servo and ensure the open sections are properly aligned.


Final Leg Assembly :
Once the COXA, FEMUR, and TIBIA are assembled individually, connect them together as follows.

### Step 4: Attach the FEMUR to the COXA
- Take the assembled FEMUR.
- Align the highest hole on the FEMUR outer part with the COXA servo driver.
- Secure the connection using an M3x35 screw.
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 36 (4)" src="https://github.com/user-attachments/assets/7dc58764-0051-4da9-8161-94ccf5f3cd5d" />


### Step 5: Attach the TIBIA to the FEMUR
- Attach the 3D-printed linkage bar to the FEMUR servo driver using the side with the single hole.
- Connect the opposite side of the linkage bar (the side with multiple holes) to the upper TIBIA part.
- Choose the hole position depending on how bent you want the leg to be:
- Holes closer to the center → smaller bending angle
- Holes farther away → larger bending angle
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 36 (3)" src="https://github.com/user-attachments/assets/0e9bcfb1-e1ee-40d8-9137-1f8b7ccbd079" />

### Step 6: Repeat for All Legs
- Repeat the previous steps for each leg.
- Double-check:
    - Servo orientations
    - Screw tightness
    - Smooth joint movement
    - Correct linkage alignment
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 35 (1)" src="https://github.com/user-attachments/assets/d05dc0d8-0eac-4b16-8236-bd71493f70c2" />
<img width="1200" height="1600" alt="WhatsApp Image 2026-05-26 at 14 35 35 (2)" src="https://github.com/user-attachments/assets/0a05987a-258b-4da1-b0e4-094438c27b35" />

## 4. Assemble body 
(Use 3mm screws everywhere except the coxa servos)
- Screw the coxa servo motors onto the body using 2.5mm screws
![Coxa Servo](/images/servo.jpg)
- Screw the multiplexer onto the body 
- Screw the buck converter onto the body 
![Buck Converter](/images/buckConverter.jpg)
- Slide the ultrasonic sensor into its holder and screw it into the body 
- Slide the protection board into its holder and screw it into the body 
- Put velcro on the bottom of the body and the LIPO and attach it
![Lipo](/images/lipoProt.jpg)
- If desired, slide microphone into its holder in the body
- Slide the ESP into the body
![ESP](/images/esp.jpg) 

## 5. Plug everything in and run the code 
- Plug all cables into corresponding locations according to the electronic diagram 
- Follow the [Software instructions](SOFTWARE_OVERVIEW.md)
