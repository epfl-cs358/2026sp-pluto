#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <array>
#include <utility>

namespace pluto
{
  class LegJoint
  {
  public:
    LegJoint(Adafruit_PWMServoDriver* driver, uint8_t channel);
    void writeRaw(uint16_t pulse);
    void writeAngle(float degrees);
    void setCalibration(uint16_t pMin, uint16_t pMax, float aMin, float aMax);
    void setTrim(int16_t offset);

  private:
    Adafruit_PWMServoDriver* _pwm;
    uint8_t _channel;
    uint16_t _pMin;
    uint16_t _pMax;
    float _aMin;
    float _aMax;
    int16_t _trim;
  };

  namespace details
  {
    template<size_t... INTEGERS>
    auto make_leg_joints(
        Adafruit_PWMServoDriver* pwm, std::integer_sequence<size_t, INTEGERS...>)
    {
      return std::array<LegJoint, sizeof...(INTEGERS)>{LegJoint{pwm, INTEGERS}...};
    }
  } // namespace details

  template<size_t COUNT>
  auto make_leg_joints(Adafruit_PWMServoDriver* pwm)
  {
    return details::make_leg_joints(pwm, std::make_index_sequence<COUNT>());
  }
} // namespace pluto
