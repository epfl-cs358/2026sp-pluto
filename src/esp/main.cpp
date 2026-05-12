#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <server/server.h>
#include <legs/leg.h>
#include <motion/gait.h>
#include <array>

#include <sensors/ultrasonic.h>
#include <sensors/microphone.h>

static constexpr size_t LEG_JOINTS_COUNT = 3;

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
pluto::motion::GaitController GAIT;

/// @brief Ultrasonic sensor
pluto::SensorUltraSonic<21, 22> SENSOR_ULTRASONIC;
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

  GAIT.stand(LEGS);
  Serial.println("Pluto motion ready");
  Serial.println("Commands: f forward, b backward, s stop, 1 walk, 2 trot, 3 gallop, +/- trim selected joint");
}

void loop()
{
  static uint32_t last_motion_ms = 0;
  static uint32_t last_sensor_ms = 0;
  static bool ultrasonic_pending = false;
  static uint32_t ultrasonic_begin_ms = 0;

  const uint32_t now = millis();

  if (now - last_motion_ms >= 20)
  {
    GAIT.update(LEGS, now);
    last_motion_ms = now;
  }

  if (!ultrasonic_pending && now - last_sensor_ms >= 500)
  {
    SENSOR_ULTRASONIC.read_begin();
    ultrasonic_pending  = true;
    ultrasonic_begin_ms = now;
  }

  if (ultrasonic_pending && now - ultrasonic_begin_ms >= 10)
  {
    Serial.print("Current Energy: ");
    Serial.println(SENSOR_MICROPHONE.current_energy());
    Serial.print("Current Distance: ");
    Serial.println(SENSOR_ULTRASONIC.read_end());
    ultrasonic_pending = false;
    last_sensor_ms     = now;
  }

  if (Serial.available() > 0)
  {
    uint16_t raw      = LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].current_raw();
    bool should_print = false;
    char cmd          = Serial.read();

    switch (cmd)
    {
    case 'f':
      GAIT.set_motion(pluto::motion::MotionCommand::FORWARD);
      Serial.println("Motion: forward");
      break;
    case 'b':
      GAIT.set_motion(pluto::motion::MotionCommand::BACKWARD);
      Serial.println("Motion: backward");
      break;
    case 's':
      GAIT.set_motion(pluto::motion::MotionCommand::IDLE);
      GAIT.stand(LEGS);
      Serial.println("Motion: stop/stand");
      break;
    case '1':
      GAIT.set_gait(pluto::motion::GaitKind::WALK);
      Serial.println("Gait: walk");
      break;
    case '2':
      GAIT.set_gait(pluto::motion::GaitKind::TROT);
      Serial.println("Gait: trot");
      break;
    case '3':
      GAIT.set_gait(pluto::motion::GaitKind::GALLOP);
      Serial.println("Gait: gallop");
      break;
    case '0':
      GAIT.set_speed(0.0F);
      Serial.println("Speed: 0%");
      break;
    case '5':
      GAIT.set_speed(0.5F);
      Serial.println("Speed: 50%");
      break;
    case '9':
      GAIT.set_speed(1.0F);
      Serial.println("Speed: 100%");
      break;
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
