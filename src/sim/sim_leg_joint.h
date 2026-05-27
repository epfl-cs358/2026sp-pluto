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
    SimLegJoint(uint8_t channel) noexcept
        : _channel(channel)
        , _current(0)
    {
    }

    /// @brief Sets joint angle in millidegrees
    void write_angle(int32_t millidegrees) noexcept
    {
      _current = millidegrees;
    }

    /// @brief Reset to starting position
    void write_starting() noexcept
    {
      // In the simulation, starting angles are 0
      write_angle(0);
    }

    uint8_t channel() const noexcept { return _channel; }
    uint32_t current_raw() const noexcept { return _current; }

    int32_t current_angle() const noexcept
    {
      return _current;
    }

  private:
    uint32_t _current;
    uint8_t _channel;
  };
} // namespace pluto