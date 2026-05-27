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
    BACKWARD = 2,
    LEFT = 3,
    RIGHT = 4,

    BOW = 5,
    PAW = 6,
  };

  class GaitController
  {
  public:
    void set_gait(GaitKind gait) noexcept;
    void set_motion(MotionCommand motion) noexcept;
    void set_speed(float speed) noexcept;
    void set_walk_manual_phase(bool enabled) noexcept;
    void next_walk_manual_stage() noexcept;
    void next_walk_manual_leg() noexcept;

    GaitKind gait() const noexcept { return _gait; }
    MotionCommand motion() const noexcept { return _motion; }
    float speed() const noexcept { return _speed; }
    bool walk_manual_phase_enabled() const noexcept { return _walk_manual_phase_enabled; }
    uint8_t walk_manual_stage() const noexcept { return _walk_manual_stage; }
    LegSide walk_manual_leg() const noexcept { return _walk_manual_leg; }

    void stand(std::array<Leg, 4>& legs) const noexcept;
    void forward_start(std::array<Leg, 4>& legs) const noexcept;
    void backward_start(std::array<Leg, 4>& legs) const noexcept;
    void update(std::array<Leg, 4>& legs, uint32_t now_ms) const noexcept;  

  private:
    float period_seconds() const noexcept;
    float offset_for(LegSide side) const noexcept;
    float phase_for(LegSide side, float time_s, float period) const noexcept;
    void write_leg(std::array<Leg, 4>& legs, LegSide side, float time_s) const noexcept;
    void write_bow(std::array<Leg, 4>& legs, float time_s) const noexcept;
    void write_paw(std::array<Leg, 4>& legs, float time_s) const noexcept;

    GaitKind _gait        = GaitKind::TROT;
    MotionCommand _motion = MotionCommand::IDLE;
    float _speed          = 0.65F;
    bool _walk_manual_phase_enabled = false;
    uint8_t _walk_manual_stage      = 0;
    LegSide _walk_manual_leg        = LegSide::TOP_LEFT;
  };
} // namespace pluto::motion
