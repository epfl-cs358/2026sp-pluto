#include "leg_joint.h"

namespace pluto
{
  LegJoint::LegJoint(Adafruit_PWMServoDriver* driver, uint8_t channel)
  {
    _pwm     = driver;
    _channel = channel;
    _pMin    = 150;
    _pMax    = 600;
    _aMin    = 0;
    _aMax    = 180;
    _trim    = 0;
  }

  void LegJoint::writeRaw(uint16_t pulse)
  {
    _pwm->setPWM(_channel, 0, pulse + _trim);
  }

  void LegJoint::writeAngle(float degrees)
  {
    degrees = constrain(degrees, _aMin, _aMax);
    uint16_t pulse =
        (uint16_t)(_pMin + (degrees - _aMin) * (_pMax - _pMin) / (_aMax - _aMin));
    writeRaw(pulse);
  }

  void LegJoint::setCalibration(uint16_t pMin, uint16_t pMax, float aMin, float aMax)
  {
    _pMin = pMin;
    _pMax = pMax;
    _aMin = aMin;
    _aMax = aMax;
  }

  void LegJoint::setTrim(int16_t offset)
  {
    _trim = offset;
  }
} // namespace pluto