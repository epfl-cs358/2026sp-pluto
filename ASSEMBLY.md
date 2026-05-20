# Assembly Instructions 
Follow these steps to assemble Pluto : 

## 1. 3D Print parts 
Print the parts present in the Fusion file. 

## 2. Assemble electrical circuit 
Solder the cables according to the following diagram : 

![Circuit Diagram](/images/circuit.png)

Use thick cables for the connection going from the protection board to the buck converter
Make sure the cables are long enough by putting 

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

### Step 2: Assemble the FEMUR
- Take the three FEMUR parts: Inner part, Spacer, Outer part
- Place the servo between the FEMUR sides.
- Stack (“empile”) the three parts together around the servo.
- Add two adjacent screws to reinforce the structure and improve stability.

### Step 3: Assemble the COXA
- Take the 3D-printed COXA part.
- Insert one screw into the dedicated hole:
- Front legs: insert the screw from the side of the shorter COXA section.
- Back legs: insert the screw from the side of the taller COXA section.
- Screw this into the servo driver attached to the body.
- Mount the COXA servo and ensure the open sections are properly aligned.

Final Leg Assembly :
Once the COXA, FEMUR, and TIBIA are assembled individually, connect them together as follows.

### Step 4: Attach the FEMUR to the COXA
- Take the assembled FEMUR.
- Align the highest hole on the FEMUR outer part with the COXA servo driver.
- Secure the connection using an M3x35 screw.

### Step 5: Attach the TIBIA to the FEMUR
- Attach the 3D-printed linkage bar to the FEMUR servo driver using the side with the single hole.
- Connect the opposite side of the linkage bar (the side with multiple holes) to the upper TIBIA part.
- Choose the hole position depending on how bent you want the leg to be:
- Holes closer to the center → smaller bending angle
- Holes farther away → larger bending angle

### Step 6: Repeat for All Legs
- Repeat the previous steps for each leg.
- Double-check:
    - Servo orientations
    - Screw tightness
    - Smooth joint movement
    - Correct linkage alignment

## 4. Assemble body 
(Use 3mm screws everywhere except the coxa servos)
- Screw the coxa servo motors onto the body using 2.5mm screws
- Screw the multiplexer onto the body 
- Screw the buck converter onto the body 
- Screw the protection board holder onto the body and carefully slide the protection board inside
- Slide the ultrasonic sensor into its holder and screw it into the body 
- Put velcro on the bottom of the body and the LIPO and attach it
- Slide microphone into its holder in the body
- Slide the ESP into the body 

## 5. Plug everything in and run the code 
- Plug all cables into corresponding locations according to the electronic diagram
- Connect ESP 
- Run the code according to the README.md instructions 
