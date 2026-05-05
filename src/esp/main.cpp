#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <server/server.h>

auto PLUTO_SERVER = pluto::PlutoServer{4242};

void setup()
{
  Serial.begin(115200);

  PLUTO_SERVER.addAP("Don't Try Plz...", "jules000");
  // TODO: add DELL wifi
  // PLUTO_SERVER.addAP("", "");

  // Initialize the background task on Core 0
  PLUTO_SERVER.begin();
}

void loop()
{
  Message msg;

  // non-blocking poll of the message queue
  while (PLUTO_SERVER.getNextMessage(msg))
  {
    switch (static_cast<MessageFamilyKind>(msg.family))
    {
    case MessageFamilyKind::KIND_MOVE:
      if (msg.kind == static_cast<uint8_t>(MessageMoveKind::MOVE_BY))
      {
        int16_t fwd  = msg.payload.move_by.top_bottom_dir;
        int16_t side = msg.payload.move_by.left_right_dir;
      }
      break;

    case MessageFamilyKind::KIND_INFO:
      break;

    default:
      break;
    }
  }
}