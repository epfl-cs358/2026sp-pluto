#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <array>
#include <utility>
#include <cstdint>

#include <legs/leg_data.h>
namespace pluto
{
  /// @brief A class to represent and control a single robotic leg joint using a PCA9685 PWM driver.
  /// @tparam MIN The minimum raw pulse of that joint
  /// @tparam MAX The maximum raw pulse of that joint
  /// @tparam STARTING The starting raw pulse of that joint
  class LegJoint
  {
  public:
    LegJoint(LegJoint&&) noexcept                 = default;
    LegJoint(const LegJoint&) noexcept            = default;
    LegJoint& operator=(LegJoint&&) noexcept      = default;
    LegJoint& operator=(const LegJoint&) noexcept = default;
    /// @brief Constructs a LegJoint and initializes the servo to the starting pulse.
    /// @param driver Pointer to the Adafruit_PWMServoDriver instance.
    /// @param channel The PCA9685 channel index (0-15).
    LegJoint(
        Adafruit_PWMServoDriver& driver, uint8_t channel, const JointConfig& config) noexcept
        : _pwm(&driver)
        , _channel(channel)
        , _config(config)
        , _current(config.raw_start)
    {
    }

    /// @brief Directly writes a raw PWM pulse width to the servo.
    /// @param pulse The 12-bit pulse width value, constrained by MIN and MAX.
    void write_raw(uint16_t pulse) noexcept
    {
      _current = constrain(pulse, _config.raw_min, _config.raw_max);
      _pwm->setPWM(_channel, 0, _current);
    }
    /// @brief Sets the joint position using a logical angle in millidegrees.
    /// @param millidegrees The target angle in millidegrees, mapped to the PWM range.
    void write_angle(int32_t millidegrees) noexcept
    {
      const int32_t calibrated_millidegrees =
          constrain(millidegrees + zero_angle_offset_md(), _config.angle_min_md, _config.angle_max_md);
      write_raw(angle_to_raw(calibrated_millidegrees));
    }
    /// @brief Sets the joint to its starting position
    void write_starting() noexcept { write_raw(_config.raw_start); }
    /// @brief Gets the joint configuration parameters.
    /// @return The JointConfig struct containing calibration data for this joint.
    const JointConfig& config() const noexcept { return _config; }
    /// @brief Gets the PCA9685 channel assigned to this joint.
    /// @return The channel index.
    uint8_t channel() const noexcept { return _channel; }
    /// @brief Gets the last written raw PWM pulse width.
    /// @return The 12-bit PWM value.
    uint16_t current_raw() const noexcept { return _current; }
    /// @brief Calculates the current logical angle based on the last written pulse.
    /// @return The current angle in millidegrees.
    int32_t current_angle() const noexcept
    {
      return raw_to_angle(_current) - zero_angle_offset_md();
    }
  
  private:
    uint16_t angle_to_raw(int32_t millidegrees) const noexcept
    {
      uint16_t pulse = _config.raw_min
                       + static_cast<uint16_t>(
                             static_cast<int64_t>(millidegrees - _config.angle_min_md)
                             * static_cast<int64_t>(_config.raw_max - _config.raw_min)
                             / static_cast<int64_t>(_config.angle_max_md - _config.angle_min_md));

      if (_config.inverted)
      {
        pulse = _config.raw_max - (pulse - _config.raw_min);
      }

      return pulse;
    }

    int32_t raw_to_angle(uint16_t raw) const noexcept
    {
      uint16_t normalized_raw = raw;
      if (_config.inverted)
      {
        normalized_raw = _config.raw_max - (raw - _config.raw_min);
      }

      return _config.angle_min_md
             + static_cast<int32_t>(
                   static_cast<int64_t>(normalized_raw - _config.raw_min)
                   * static_cast<int64_t>(_config.angle_max_md - _config.angle_min_md)
                   / static_cast<int64_t>(_config.raw_max - _config.raw_min));
    }

    int32_t zero_angle_offset_md() const noexcept
    {
      return raw_to_angle(_config.raw_start);
    }

    /// @brief Pointer to the servo driver.
    /// @warning Must outlive the current class.
    Adafruit_PWMServoDriver* _pwm;
    /// @brief The current raw PWM
    uint16_t _current;
    /// @brief The channel
    uint8_t _channel;
    JointConfig _config;
  };
} // namespace pluto
