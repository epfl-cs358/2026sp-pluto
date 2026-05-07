#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <server/server.h>
#include <legs/leg.h>
#include <array>

/// @brief The pluto WiFi server
// auto PLUTO_SERVER = pluto::PlutoServer{4242};
/// @brief The PWM driver
Adafruit_PWMServoDriver PWM = Adafruit_PWMServoDriver();
/// @brief The legs
std::array<pluto::Leg, 4> LEGS = {
    pluto::Leg{PWM, pluto::LegSide::TOP_LEFT},
    pluto::Leg{PWM, pluto::LegSide::TOP_RIGHT},
    pluto::Leg{PWM, pluto::LegSide::BOTTOM_LEFT},
    pluto::Leg{PWM, pluto::LegSide::BOTTOM_RIGHT}};

auto CURRENT_JOINT = pluto::LegJointType{};
auto CURRENT_SIDE  = pluto::LegSide{};

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
}

void loop()
{
  if (Serial.available() > 0)
  {
    uint16_t raw      = LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].current_raw();
    bool should_print = false;
    char cmd          = Serial.read();

    switch (cmd)
    {
    case '+':
      LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].write_raw(raw + 5);
      should_print = true;
      break;
    case '-':
      LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].write_raw(raw - 5);
      should_print = true;
      break;
    case 'l':
      CURRENT_SIDE = pluto::next_leg_side(CURRENT_SIDE);
      should_print = true;
      break;
    case 'n':
      CURRENT_JOINT = pluto::next_leg_joint_type(CURRENT_JOINT);
      should_print  = true;
      break;
    case 'r':
      LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].write_starting();
      should_print = true;
      break;
    case 'R':
      LEGS[(uint8_t)CURRENT_SIDE].write_starting();
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
      Serial.print("Leg: ");
      Serial.print(pluto::str_leg_side(CURRENT_SIDE));
      Serial.print(" | Joint: ");
      Serial.print(pluto::str_leg_joint_type(CURRENT_JOINT));
      Serial.print(" | Pulse: ");
      Serial.println(LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].current_raw());
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