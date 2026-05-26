#pragma once

#include <algorithm>
#include <cstdint>

#include <esp/legs/leg_data.h>

namespace pluto::sim
{
  /// @brief Simulation-friendly abstraction of a servo joint (no hardware dependencies)
  class SimLegJoint
  {
  public:
    SimLegJoint(SimLegJoint&&) noexcept                  = default;
    SimLegJoint(const SimLegJoint&) noexcept             = default;
    SimLegJoint& operator=(SimLegJoint&&) noexcept       = default;
    SimLegJoint& operator=(const SimLegJoint&) noexcept  = default;

    /// @brief Constructs a SimLegJoint
    SimLegJoint(uint8_t channel, const JointConfig& config) noexcept
        : _channel(channel)
        , _config(config)
        , _current(config.raw_start)
    {
    }

    /// @brief Sets raw PWM value (simulation only)
    void write_raw(uint16_t pulse) noexcept
    {
      _current = std::clamp(pulse, _config.raw_min, _config.raw_max);
    }

    /// @brief Sets joint angle in millidegrees
    void write_angle(int32_t millidegrees) noexcept
    {
      const int32_t calibrated_md =
          std::clamp(millidegrees, _config.angle_min_md, _config.angle_max_md);

      write_raw(angle_to_raw(calibrated_md));
    }

    /// @brief Reset to starting position
    void write_starting() noexcept
    {
      write_raw(_config.raw_start);
    }

    const JointConfig& config() const noexcept { return _config; }
    uint8_t channel() const noexcept { return _channel; }
    uint16_t current_raw() const noexcept { return _current; }

    int32_t current_angle() const noexcept
    {
      return raw_to_angle(_current);
    }

  private:
    uint16_t angle_to_raw(int32_t millidegrees) const noexcept
    {
      uint16_t pulse =
          _config.raw_min +
          static_cast<uint16_t>(
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
        normalized_raw =
            _config.raw_max - (raw - _config.raw_min);
      }

      return _config.angle_min_md +
             static_cast<int32_t>(
                 static_cast<int64_t>(normalized_raw - _config.raw_min)
                 * static_cast<int64_t>(_config.angle_max_md - _config.angle_min_md)
                 / static_cast<int64_t>(_config.raw_max - _config.raw_min));
    }

  private:
    uint16_t _current;
    uint8_t _channel;
    JointConfig _config;
  };
} // namespace pluto