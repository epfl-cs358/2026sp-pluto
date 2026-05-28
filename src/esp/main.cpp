#define PLUTO_ENABLE_WIFI
#define PLUTO_ENABLE_ULTRASONIC
// #define PLUTO_ENABLE_MICROPHONE

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <legs/leg.h>
#include <motion/gait.h>
#include <array>

#ifdef PLUTO_ENABLE_WIFI
  #include <server/server.h>
#endif // PLUTO_ENABLE_WIFI

#ifdef PLUTO_ENABLE_ULTRASONIC
  #include <sensors/ultrasonic.h>
#endif // PLUTO_ENABLE_ULTRASONIC

#ifdef PLUTO_ENABLE_MICROPHONE
  #include <sensors/microphone.h>
#endif // PLUTO_ENABLE_MICROPHONE

#ifdef PLUTO_ENABLE_WIFI
/// @brief The pluto WiFi server
auto PLUTO_SERVER = pluto::PlutoServer{4242};
#endif

/// @brief The PWM driver
Adafruit_PWMServoDriver PWM = Adafruit_PWMServoDriver();
/// @brief The legs
std::array<pluto::Leg, 4> LEGS = {
    pluto::Leg{PWM, pluto::LegSide::TOP_LEFT},
    pluto::Leg{PWM, pluto::LegSide::TOP_RIGHT},
    pluto::Leg{PWM, pluto::LegSide::BOTTOM_LEFT},
    pluto::Leg{PWM, pluto::LegSide::BOTTOM_RIGHT}};

pluto::LegJointType CURRENT_JOINT = pluto::LegJointType::COXA;
pluto::LegSide CURRENT_SIDE       = pluto::LegSide::TOP_LEFT;
pluto::motion::GaitController GAIT;

#ifdef PLUTO_ENABLE_ULTRASONIC
/// @brief Ultrasonic sensor
pluto::SensorUltraSonic<5, 18> SENSOR_ULTRASONIC;
#endif

#ifdef PLUTO_ENABLE_MICROPHONE
/// @brief Microphone sensor
pluto::SensorMicrophone<26, 25, 33> SENSOR_MICROPHONE;
#endif

static bool robot_walking = false;
void stop_robot(const char* reason)
{
  GAIT.set_motion(pluto::motion::MotionCommand::IDLE);
  GAIT.stand(LEGS);
  robot_walking = false;

  Serial.print("STOP: ");
  Serial.println(reason);
}

void start_robot()
{
  GAIT.forward_start(LEGS);
  delay(300);

  GAIT.set_gait(pluto::motion::GaitKind::WALK);
  GAIT.set_speed(0.65F);
  GAIT.set_motion(pluto::motion::MotionCommand::FORWARD);
  robot_walking = true;

  Serial.println("START: robot walking");
}

/**
static constexpr uint32_t CLAP_THRESHOLD = 30000000;
static constexpr uint32_t CLAP_COOLDOWN_MS = 800;
static constexpr uint32_t MICROPHONE_CHECK_PERIOD_MS = 50;
static constexpr uint32_t MICROPHONE_PRINT_PERIOD_MS = 1000; 

void update_microphone_control(uint32_t now)
{
#ifdef PLUTO_ENABLE_MICROPHONE
  static uint32_t last_microphone_check_ms = 0; 
  static uint32_t last_microphone_print_ms = 0; 
  static uint32_t last_clap_ms = 0; 
  static bool was_above_threshold = false; 

  if (now - last_microphone_check_ms < MICROPHONE_CHECK_PERIOD_MS){
    return; 
  }

  last_microphone_check_ms = now; 

  uint32_t mic_energy = SENSOR_MICROPHONE.current_energy(); 

  if (now - last_microphone_print_ms >= MICROPHONE_PRINT_PERIOD_MS)
  {
    last_microphone_print_ms = now; 

    Serial.print("Mic energy: ");
    Serial.println(mic_energy);
  }

  bool above_threshold = mic_energy >= CLAP_THRESHOLD; 

  if (above_threshold && !was_above_threshold && now - last_clap_ms >= CLAP_COOLDOWN_MS)
  {
    last_clap_ms = now; 

    Serial.print("CLAP DETECTED | Energy: "); 
    Serial.println(mic_energy); 
    
    if (robot_walking)
    {
      Serial.println("CLAP ACTION stop robot");
      stop_robot("clap detected");
    }
    else 
    {
      Serial.println("CLAP start robot"); 
      start_robot();
    }
  }

  was_above_threshold = above_threshold; 
#endif
}
*/

static constexpr float WALL_STOP_DISTANCE_CM   = 35.0F;
static constexpr uint32_t ULTRASONIC_PERIOD_MS = 150;
static constexpr uint32_t ULTRASONIC_WAIT_MS   = 10;

void update_ultrasonic_control(uint32_t now)
{
#ifdef PLUTO_ENABLE_ULTRASONIC
  static bool ultrasonic_pending      = false;
  static uint32_t ultrasonic_begin_ms = 0;
  static uint32_t last_ultrasonic_ms  = 0;

  if (!ultrasonic_pending && now - last_ultrasonic_ms >= ULTRASONIC_PERIOD_MS)
  {
    SENSOR_ULTRASONIC.read_begin();
    ultrasonic_pending  = true;
    ultrasonic_begin_ms = now;
  }

  if (ultrasonic_pending && now - ultrasonic_begin_ms >= ULTRASONIC_WAIT_MS)
  {
    ultrasonic_pending = false;
    last_ultrasonic_ms = now;

    float distance_cm = SENSOR_ULTRASONIC.read_end();

    if (distance_cm > 0.0F && distance_cm < WALL_STOP_DISTANCE_CM)
    {
      stop_robot("wall too close");
    }
  }
#endif
}

static constexpr int16_t WIFI_MOVE_DEADZONE = 100;

void command_forward()
{
  if (GAIT.motion() != pluto::motion::MotionCommand::FORWARD)
  {
    GAIT.forward_start(LEGS);
    delay(300);
  }

  GAIT.set_gait(pluto::motion::GaitKind::WALK);
  GAIT.set_speed(0.65F);
  GAIT.set_motion(pluto::motion::MotionCommand::FORWARD);
  robot_walking = true;

  Serial.println("Motion: forward");
}

void command_backward()
{
  if (GAIT.motion() != pluto::motion::MotionCommand::BACKWARD)
  {
    GAIT.backward_start(LEGS);
    delay(300);
  }

  GAIT.set_gait(pluto::motion::GaitKind::WALK);
  GAIT.set_motion(pluto::motion::MotionCommand::BACKWARD);
  robot_walking = true;

  Serial.println("Motion: backward");
}

void command_turn_left()
{
  GAIT.set_motion(pluto::motion::MotionCommand::LEFT);
  robot_walking = true;

  Serial.println("Motion: turn left");
}

void command_turn_right()
{
  GAIT.set_motion(pluto::motion::MotionCommand::RIGHT);
  robot_walking = true;

  Serial.println("Motion: turn right");
}

void command_stop()
{
  GAIT.set_motion(pluto::motion::MotionCommand::IDLE);
  GAIT.stand(LEGS);
  robot_walking = false;

  last_wifi_motion = pluto::motion::MotionCommand::IDLE;
  Serial.println("Motion: stop/stand");
}

void command_bow()
{
  GAIT.set_motion(pluto::motion::MotionCommand::BOW);
  robot_walking = false;

  Serial.println("Motion: bow");
}

void command_paw()
{
  GAIT.set_motion(pluto::motion::MotionCommand::PAW);
  robot_walking = false;

  Serial.println("Motion: paw");
}

pluto::motion::MotionCommand last_wifi_motion = pluto::motion::MotionCommand::IDLE;

void apply_move_command(int16_t fwd, int16_t side)
{
  if (abs(fwd) < WIFI_MOVE_DEADZONE) fwd = 0;
  if (abs(side) < WIFI_MOVE_DEADZONE) side = 0;

  pluto::motion::MotionCommand new_motion = pluto::motion::MotionCommand::IDLE;

  if (fwd > 0)
    new_motion = pluto::motion::MotionCommand::FORWARD;
  else if (fwd < 0)
    new_motion = pluto::motion::MotionCommand::BACKWARD;
  else if (side < 0)
    new_motion = pluto::motion::MotionCommand::LEFT;
  else if (side > 0)
    new_motion = pluto::motion::MotionCommand::RIGHT;

  if (new_motion == last_wifi_motion)
  {
    return;
  }

  last_wifi_motion = new_motion;

  switch (new_motion)
  {
    case pluto::motion::MotionCommand::FORWARD:
      command_forward();
      break;

    case pluto::motion::MotionCommand::BACKWARD:
      command_backward();
      break;

    case pluto::motion::MotionCommand::LEFT:
      command_turn_left();
      break;

    case pluto::motion::MotionCommand::RIGHT:
      command_turn_right();
      break;

    case pluto::motion::MotionCommand::IDLE:
    default:
      command_stop();
      break;
  }
}

void setup()
{
  Serial.begin(115200);

#ifdef PLUTO_ENABLE_WIFI
  PLUTO_SERVER.addAP("Don't Try Plz...", "jules000");
  // TODO: add DELL wifi
  // PLUTO_SERVER.addAP("", "");
  PLUTO_SERVER.begin();
#endif

  PWM.begin();
  PWM.setOscillatorFrequency(27000000);
  PWM.setPWMFreq(50);

#ifdef PLUTO_ENABLE_ULTRASONIC
  SENSOR_ULTRASONIC.begin();
#endif

#ifdef PLUTO_ENABLE_MICROPHONE
  SENSOR_MICROPHONE.begin();
#endif

  GAIT.stand(LEGS);
  Serial.println("Pluto motion ready");
  Serial.println(
      "Commands: f forward, b backward, q turn-left, e turn-right, o flip, h bow, i "
      "sit, s stop, 1 walk, 2 trot, 3 gallop"
      "selected joint");
}

void loop()
{
  static uint32_t last_motion_ms = 0;

  const uint32_t now = millis();

  // update_microphone_control(now);
  update_ultrasonic_control(now);

  if (now - last_motion_ms >= 20)
  {
    GAIT.update(LEGS, now);
    last_motion_ms = now;
  }

  if (Serial.available() > 0)
  {
    uint16_t raw      = LEGS[(uint8_t)CURRENT_SIDE][CURRENT_JOINT].current_raw();
    bool should_print = false;
    char cmd          = Serial.read();

    switch (cmd)
    {
    case 'f':
      GAIT.forward_start(LEGS);
      delay(1000);

      GAIT.set_motion(pluto::motion::MotionCommand::FORWARD);
      robot_walking = true;
      Serial.println("Motion: forward");
      break;
    case 'b':
      GAIT.backward_start(LEGS);
      delay(1000);

      GAIT.set_motion(pluto::motion::MotionCommand::BACKWARD);
      robot_walking = true;
      Serial.println("Motion: backward");
      break;
    case 'q':
      GAIT.turnleft_start(LEGS);
      delay(1000);

      GAIT.set_motion(pluto::motion::MotionCommand::LEFT);
      robot_walking = true;
      Serial.println("Motion: turn left");
      break;
    case 'e':
      GAIT.turnright_start(LEGS);
      delay(1000);

      GAIT.set_motion(pluto::motion::MotionCommand::RIGHT);
      robot_walking = true;
      Serial.println("Motion: turn right");
      break;
    case 'o':
      GAIT.set_motion(pluto::motion::MotionCommand::FLIP);
      Serial.println("Motion: flip");
      break;
    case 'h':
      GAIT.bow_start(LEGS);
      delay(250);
      GAIT.set_motion(pluto::motion::MotionCommand::BOW);
      Serial.println("Motion: bow");
      break;
    case 'i':
      GAIT.sit_start(LEGS);
      delay(250);
      GAIT.set_motion(pluto::motion::MotionCommand::SIT);
      Serial.println("Motion: sit");
      break;
    case 'k':
      GAIT.set_motion(pluto::motion::MotionCommand::PAW);
      Serial.println("Motion: paw");
      break;
    case 's':
      GAIT.set_motion(pluto::motion::MotionCommand::IDLE);
      GAIT.stand(LEGS);
      robot_walking = false;
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
    case 'u':
      GAIT.set_walk_manual_phase(!GAIT.walk_manual_phase_enabled());
      Serial.print("Walk manual phase: ");
      Serial.println(GAIT.walk_manual_phase_enabled() ? "ON" : "OFF");
      break;
    case 'm':
      GAIT.next_walk_manual_stage();
      Serial.print("Walk manual stage: ");
      Serial.println(GAIT.walk_manual_stage());
      break;
    case 'j':
      GAIT.next_walk_manual_leg();
      Serial.print("Walk manual leg: ");
      Serial.println(pluto::str_leg_side(GAIT.walk_manual_leg()));
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

#ifdef PLUTO_ENABLE_WIFI
  Message msg;
  while (PLUTO_SERVER.getNextMessage(msg))
  {
    switch (static_cast<MessageFamilyKind>(msg.family))
    {
    case MessageFamilyKind::KIND_MOVE:
      switch (static_cast<MessageMoveKind>(msg.kind))
      {
      case MessageMoveKind::MOVE_BY:
      {
        int16_t fwd  = msg.payload.move_by.top_bottom_dir;
        int16_t side = msg.payload.move_by.left_right_dir;

        apply_move_command(fwd, side);
      }
      break;

      case MessageMoveKind::MOVE_STOP_FOR:
        command_stop();
        Serial.println("Command: Stop and Stand");
        break;

      default:
        break;
      }
      break;

    case MessageFamilyKind::KIND_INFO:
      if (msg.kind == static_cast<uint8_t>(MessageInfoKind::INFO_REQUEST_SENSOR))
      {
        if (msg.payload.info_request_sensor.sensor_kind
            == MessageSensorKind::SENSOR_DISTANCE)
        {
  #ifdef PLUTO_ENABLE_ULTRASONIC
          float distance_cm = SENSOR_ULTRASONIC.read_end();

          if (distance_cm > 0.0F)
          {
            uint32_t distance_mm = static_cast<uint32_t>(distance_cm * 10.0F);
            Message reply = pluto::create_sensor_distance(distance_mm, millis());
            PLUTO_SERVER.sendMessage(reply);
          }
  #endif
        }
      }
      break;

    case MessageFamilyKind::KIND_BEHAVIOR:
      switch (static_cast<MessageBehaviorKind>(msg.kind))
      {
      case MessageBehaviorKind::BEHAVIOR_SIT:
        command_stop();
        Serial.println("Behavior: sit requested");
        break;

      case MessageBehaviorKind::BEHAVIOR_GIVE_PAW:
        command_paw();
        Serial.println("Behavior: give paw");
        break;

      case MessageBehaviorKind::BEHAVIOR_FLIP:
        Serial.println("Behavior: flip");
        break;

      case MessageBehaviorKind::BEHAVIOR_BOW:
        command_bow();
        Serial.println("Behavior: lie down requested");
        break;

      default:
        break;
      }
      break;

    default:
      break;
    }
  }
#endif
}
