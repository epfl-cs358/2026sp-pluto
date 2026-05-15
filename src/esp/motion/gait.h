#pragma once

#include <array>
#include <cstdint>

#include <legs/leg.h>

namespace pluto::motion
{
  enum class GaitKind : uint8_t
  {
    WALK = 0,
    TROT,
    GALLOP,
    TURN, // Move forward on this gait to turn right, backward to turn left
  };

  enum class MotionCommand : uint8_t
  {
    IDLE = 0,

    FORWARD = 1,
    RIGHT = FORWARD,

    BACKWARD = 2,
    LEFT = BACKWARD,

    BOW = 3,
    PAW = 4,
  };

  class GaitController
  {
  public:
    void set_gait(GaitKind gait) noexcept;
    void set_motion(MotionCommand motion) noexcept;
    void set_speed(float speed) noexcept;

    GaitKind gait() const noexcept { return _gait; }
    MotionCommand motion() const noexcept { return _motion; }
    float speed() const noexcept { return _speed; }

    void stand(std::array<Leg, 4>& legs) const noexcept;
    void update(std::array<Leg, 4>& legs, uint32_t now_ms) const noexcept;

  private:
    float period_seconds() const noexcept;
    float offset_for(LegSide side) const noexcept;
    void write_leg(std::array<Leg, 4>& legs, LegSide side, float time_s) const noexcept;
    void write_bow(std::array<Leg, 4>& legs, float time_s) const noexcept;
    void write_paw(std::array<Leg, 4>& legs, float time_s) const noexcept;

    GaitKind _gait        = GaitKind::TROT;
    MotionCommand _motion = MotionCommand::IDLE;
    float _speed          = 0.65F;
  };
} // namespace pluto::motion
