#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <server/server.h>
#include <legs/leg.h>

#include <sensors/ultrasonic.h>
#include <sensors/microphone.h>

static constexpr size_t LEG_JOINTS_COUNT = 3;

/// @brief The pluto WiFi server
// auto PLUTO_SERVER = pluto::PlutoServer{4242};
/// @brief The PWM driver
Adafruit_PWMServoDriver PWM = Adafruit_PWMServoDriver();
/// @brief The leg
auto LEG = pluto::Leg{PWM, pluto::LegSide::TOP_LEFT};

auto CURRENT_JOINT = pluto::LegJointType{};

/// @brief Ultrasonic sensor
pluto::SensorUltraSonic<5, 18> SENSOR_ULTRASONIC;
/// @brief Microphone sensor
pluto::SensorMicrophone<26, 25, 33> SENSOR_MICROPHONE;

void setup()
{
  Serial.begin(115200);

  // PLUTO_SERVER.addAP("Don't Try Plz...", "jules000");
  // TODO: add DELL wifi
  // PLUTO_SERVER.addAP("", "");

  // Initialize the background task on Core 0
  // PLUTO_SERVER.begin();
  PWM.begin();
  PWM.setOscillatorFrequency(27000000);
  PWM.setPWMFreq(50);
  SENSOR_ULTRASONIC.begin();
  SENSOR_MICROPHONE.begin();
}

void loop()
{
  if (Serial.available() > 0)
  {
    uint16_t raw      = LEG[CURRENT_JOINT].current_raw();
    bool should_print = false;
    char cmd          = Serial.read();

    switch (cmd)
    {
    case '+':
      LEG[CURRENT_JOINT].write_raw(raw + 5);
      should_print = true;
      break;
    case '-':
      LEG[CURRENT_JOINT].write_raw(raw - 5);
      should_print = true;
      break;
    case 'n':
      CURRENT_JOINT = pluto::next_joint_type(CURRENT_JOINT);
      should_print  = true;
      break;
    case 'r':
      LEG[CURRENT_JOINT].write_starting();
      should_print = true;
      break;
    case 'R':
      LEG.write_starting();
      Serial.println("Reset all joints");
      break;
    case 'p':
      should_print = true;
      break;
    default:
      // Ignore invalid characters and whitespace
      break;
    }

    if (should_print)
    {
      Serial.print("Joint: ");
      Serial.print(pluto::str_joint_type(CURRENT_JOINT));
      Serial.print(" | Pulse: ");
      Serial.println(LEG[CURRENT_JOINT].current_raw());
    }
  }

  //   Message msg;

  //   // non-blocking poll of the message queue
  //   while (PLUTO_SERVER.getNextMessage(msg))
  //   {
  //     switch (static_cast<MessageFamilyKind>(msg.family))
  //     {
  //     case MessageFamilyKind::KIND_MOVE:
  //       if (msg.kind == static_cast<uint8_t>(MessageMoveKind::MOVE_BY))
  //       {
  //         int16_t fwd  = msg.payload.move_by.top_bottom_dir;
  //         int16_t side = msg.payload.move_by.left_right_dir;
  //       }
  //       break;

  //     case MessageFamilyKind::KIND_INFO:
  //       break;

  //     default:
  //       break;
  //     }
  //   }
}