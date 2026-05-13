# 2026sp-pluto

## Running Pluto's Controller UI:
For Windows, in the root project directory, type:
```cmd
./run.bat
```

For MacOS and Linux, in the root project directory, type:
```cmd
bash run.sh
```

These scripts will automatically install all the dependencies needed and run the web app.

By default, the application runs on port 8080. If another process is already using that port, you may specify a `--port <VALUE>` after the `./run.[bat|sh]` to use another port.


## Commands

```bash
pio run # compile
pio run -t upload

# in case it doesn't detect the port
pio device list
pio run -t upload --upload-port /dev/cu.usbserial-XXXX

# test, serial monitor
pio device monitor # or
pio device monitor -b 115200
```

f = forward
b = backward
s = stop/stand
1 = walk
2 = trot
3 = gallop
0/5/9 = speed

- 지금 servo 각도 범위는 기존 raw pulse 값에 맞춘 임시 mapping이라, 실제 다리가 반대로 꺾이거나 범위가 과하면 leg_data.h (line 15)의 angle range/raw 값을 캘리브레이션해야

- logic
Serial command
  -> GaitController 상태 변경
  -> loop에서 20ms마다 gait.update()
  -> 각 다리의 phase 계산
  -> phase로 발 위치 x,y,z 계산
  -> IK로 coxa/femur/tibia angle 계산
  -> millidegree로 변환
  -> Leg::write_angles()
  -> LegJoint::write_angle()
  -> PCA9685 PWM pulse
  -> servo 움직임


## Things to Test

Build the ESP firmware with PlatformIO.
Upload the firmware to the ESP32.
Verify that the ESP32 is detected on the correct serial port.
Open the serial monitor at 115200 baud.
Test the stand command and confirm all legs move to a stable neutral pose.

Test each leg calibration range and confirm no servo hits a mechanical limit.
Test joint direction for each servo: top/coxa, middle/femur, bottom/tibia.
Test one leg at a time before moving all four legs together.
Test forward, backward, and stop serial commands.
Test walk, trot, and gallop gait selection.
Start with small stride and lift values before increasing them.

Confirm diagonal legs move together during trot.
Confirm lifted legs move up and forward while support legs move backward.

Check whether left/right legs are mirrored correctly.
Check whether front/back legs are assigned to the correct servo channels.

Tune leg-specific calibration values in leg_data.h.
Verify the robot does not tip over when a leg or diagonal pair is lifted.

Test with the robot lifted off the ground before testing on the floor.
Monitor servo heating, jitter, and power draw.
Confirm ultrasonic and microphone sensor readings still work while gait updates run.