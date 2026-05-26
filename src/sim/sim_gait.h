#pragma once

#include <array>
#include <cstdint>

#include <sim/sim_leg.h>

namespace pluto::sim
{
  enum class SimGaitKind : uint8_t
  {
    WALK = 0,
    TROT,
    GALLOP,
    TURN, // Move forward on this gait to turn right, backward to turn left
  };

  enum class SimMotionCommand : uint8_t
  {
    IDLE = 0,

    FORWARD = 1,
    RIGHT = FORWARD,

    BACKWARD = 2,
    LEFT = BACKWARD,

    BOW = 3,
    PAW = 4,
  };

  class SimGaitController
  {
  public:
    void set_gait(SimGaitKind gait) noexcept;
    void set_motion(SimMotionCommand motion) noexcept;
    void set_speed(float speed) noexcept;
    void set_walk_manual_phase(bool enabled) noexcept;
    void next_walk_manual_stage() noexcept;
    void next_walk_manual_leg() noexcept;

    SimGaitKind gait() const noexcept { return _gait; }
    SimMotionCommand motion() const noexcept { return _motion; }
    float speed() const noexcept { return _speed; }
    bool walk_manual_phase_enabled() const noexcept { return _walk_manual_phase_enabled; }
    uint8_t walk_manual_stage() const noexcept { return _walk_manual_stage; }
    LegSide walk_manual_leg() const noexcept { return _walk_manual_leg; }

    void stand(std::array<pluto::sim::SimLeg, 4>& legs) const noexcept;
    void update(std::array<pluto::sim::SimLeg, 4>& legs, uint32_t now_ms) const noexcept;

  private:
    float period_seconds() const noexcept;
    float offset_for(LegSide side) const noexcept;
    float phase_for(LegSide side, float time_s, float period) const noexcept;
    void write_leg(std::array<pluto::sim::SimLeg, 4>& legs, LegSide side, float time_s) const noexcept;
    void write_bow(std::array<pluto::sim::SimLeg, 4>& legs, float time_s) const noexcept;
    void write_paw(std::array<pluto::sim::SimLeg, 4>& legs, float time_s) const noexcept;

    SimGaitKind _gait        = SimGaitKind::TROT;
    SimMotionCommand _motion = SimMotionCommand::IDLE;
    float _speed          = 0.65F;
    bool _walk_manual_phase_enabled = false;
    uint8_t _walk_manual_stage      = 0;
    LegSide _walk_manual_leg        = LegSide::TOP_LEFT;
  };
} // namespace pluto::motion
