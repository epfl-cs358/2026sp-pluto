#include <motion/gait.h>

#include <Arduino.h>
#include <cmath>
#include <motion/ik_solver.h>

namespace pluto::motion
{
  namespace
  {
    // TODO: Change these constants to reflect the actual measurements of the robot.
    constexpr float COXA_LENGTH  = 0.40F; // 6.3 centimeters
    constexpr float FEMUR_LENGTH = 1.20F; // 10 centimeters
    constexpr float TIBIA_LENGTH = 1.36F; // 9 centimeters

    constexpr float STAND_COMPRESSION = 0.26F;
    constexpr float FOOT_Z_STAND      = -(FEMUR_LENGTH + TIBIA_LENGTH - STAND_COMPRESSION);

    constexpr float SWING_RATIO = 0.35F;
    constexpr float STRIDE      = 0.60F;
    constexpr float LIFT        = 0.50F;

    constexpr bool is_right_side(LegSide side) noexcept
    {
      return side == LegSide::TOP_RIGHT || side == LegSide::BOTTOM_RIGHT;
    }

    constexpr bool is_front_side(LegSide side) noexcept
    {
      return side == LegSide::TOP_LEFT || side == LegSide::TOP_RIGHT;
    }

    float normalized_phase(float time_s, float period_s, float offset) noexcept
    {
      float phase = fmodf(time_s / period_s + offset, 1.0F);
      if (phase < 0.0F)
      {
        phase += 1.0F;
      }

      return phase;
    }

    FootTarget foot_from_phase(float phase, float forward_scale) noexcept
    {
      if (phase < SWING_RATIO)
      {
        const float t = phase / SWING_RATIO;
        return {
            (-STRIDE + 2.0F * STRIDE * t) * forward_scale,
            0.0F,
            FOOT_Z_STAND + LIFT * sinf(PI * t)};
      }

      const float t = (phase - SWING_RATIO) / (1.0F - SWING_RATIO);
      return {(STRIDE - 2.0F * STRIDE * t) * forward_scale, 0.0F, FOOT_Z_STAND};
    }

    JointAnglesMd solve_leg(FootTarget foot, LegSide side) noexcept
    {
      const float signed_coxa = is_right_side(side) ? -COXA_LENGTH : COXA_LENGTH;
      return to_millidegrees(solve_ik(foot, signed_coxa, FEMUR_LENGTH, TIBIA_LENGTH));
    }
  } // namespace

  void GaitController::set_gait(GaitKind gait) noexcept { _gait = gait; }

  void GaitController::set_motion(MotionCommand motion) noexcept { _motion = motion; }

  void GaitController::set_speed(float speed) noexcept { _speed = constrain(speed, 0.0F, 1.0F); }

  void GaitController::stand(std::array<Leg, 4>& legs) const noexcept
  {
    for (auto& leg : legs)
    {
      leg.write_starting();
    }

    //for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    //{
    //  const auto side   = static_cast<LegSide>(i);
    //  const auto angles = solve_leg({0.0F, 0.0F, FOOT_Z_STAND}, side);
    //  legs[i].write_angles(angles.coxa_md, angles.femur_md, angles.tibia_md);
    //}
  }

  void GaitController::update(std::array<Leg, 4>& legs, uint32_t now_ms) const noexcept
  {
    if (_motion == MotionCommand::IDLE || _speed <= 0.0F)
    {
      return;
    }

    const float time_s = static_cast<float>(now_ms) * 0.001F;
    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
      write_leg(legs, static_cast<LegSide>(i), time_s);
    }
  }

  float GaitController::period_seconds() const noexcept
  {
    switch (_gait)
    {
    case GaitKind::WALK:
      return 0.50F;
    case GaitKind::TROT:
      return 0.35F;
    case GaitKind::GALLOP:
      return 0.30F;
    }

    return 0.35F;
  }

  float GaitController::offset_for(LegSide side) const noexcept
  {
    switch (_gait)
    {
    case GaitKind::WALK:
      switch (side)
      {
      case LegSide::TOP_LEFT:
        return 0.0F;
      case LegSide::TOP_RIGHT:
        return 0.5F;
      case LegSide::BOTTOM_LEFT:
        return 0.25F;
      case LegSide::BOTTOM_RIGHT:
        return 0.75F;
      }
      break;

    case GaitKind::TROT:
      switch (side)
      {
      case LegSide::TOP_LEFT:
      case LegSide::BOTTOM_RIGHT:
        return 0.0F;
      case LegSide::TOP_RIGHT:
      case LegSide::BOTTOM_LEFT:
        return 0.5F;
      }
      break;

    case GaitKind::GALLOP:
      switch (side)
      {
      case LegSide::TOP_LEFT:
        return 0.0F;
      case LegSide::TOP_RIGHT:
        return 0.1F;
      case LegSide::BOTTOM_LEFT:
        return 0.6F;
      case LegSide::BOTTOM_RIGHT:
        return 0.5F;
      }
      break;
    }

    return 0.0F;
  }

  void GaitController::write_leg(
      std::array<Leg, 4>& legs, LegSide side, float time_s) const noexcept
  {
    const float direction = _motion == MotionCommand::BACKWARD ? -1.0F : 1.0F;
    const float phase = normalized_phase(time_s, period_seconds(), offset_for(side));
    const auto foot   = foot_from_phase(phase, direction * _speed);
    const auto angles = solve_leg(foot, side);

    legs[static_cast<uint8_t>(side)].write_angles(
        angles.coxa_md, angles.femur_md, angles.tibia_md);
  }
} // namespace pluto::motion
