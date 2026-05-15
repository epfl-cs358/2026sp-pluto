#include <motion/gait.h>

#include <Arduino.h>
#include <cmath>
#include <motion/ik_solver.h>

namespace pluto::motion
{
  namespace
  {
    constexpr float COXA_LENGTH  = 7.00F;
    constexpr float FEMUR_LENGTH = 12.00F;
    constexpr float TIBIA_LENGTH = 13.50F;

    constexpr float FOOT_Z_STAND = -22.0F; // standing height in cm

    constexpr float SHIFT_END = 0.25F;
    constexpr float LIFT_END  = 0.50F;
    constexpr float STEP_END  = 0.75F;

    constexpr float STRIDE = 4.00F;
    constexpr float LIFT   = 2.00F;

    constexpr float FOOT_Y_STANCE          = 7.00F;
    constexpr float WALK_BALANCE_SHIFT_Y   = 2.50F;
    constexpr float WALK_SUPPORT_PUSH_DOWN = 0.80F;
    constexpr float BOW_FRONT_DROP         = 5.00F;
    constexpr float BOW_REAR_RISE          = 2.00F;
    constexpr float BOW_FRONT_BACK         = 1.50F;
    constexpr float BOW_PERIOD             = 2.00F;

    constexpr float PAW_REST_X             = 1.50F;
    constexpr float PAW_REST_Z             = -16.00F;
    constexpr float PAW_PEAK_X             = 4.50F;
    constexpr float PAW_PEAK_Z             = -12.00F;
    constexpr float PAW_PERIOD             = 1.20F;

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

    float smoothstep(float t) noexcept
    {
      return t * t * (3.0F - 2.0F * t);
    }

    bool is_airborne(float phase) noexcept
    {
      return phase >= SHIFT_END && phase < STEP_END;
    }

    float support_request_weight(float phase) noexcept
    {
      if (phase < SHIFT_END)
      {
        return smoothstep(phase / SHIFT_END);
      }

      if (phase < STEP_END)
      {
        return 1.0F;
      }

      return 1.0F - smoothstep((phase - STEP_END) / (1.0F - STEP_END));
    }

    float side_stance_y(LegSide side) noexcept
    {
      return is_right_side(side) ? -FOOT_Y_STANCE : FOOT_Y_STANCE;
    }

    FootTarget foot_from_phase(float phase, float forward_scale, float foot_y) noexcept
    {
      if (phase < SHIFT_END)
      {
        const float t = phase / SHIFT_END;
        return {
            (-STRIDE * smoothstep(t)) * forward_scale,
            foot_y,
            FOOT_Z_STAND};
      }

      if (phase < LIFT_END)
      {
        const float t = (phase - SHIFT_END) / (LIFT_END - SHIFT_END);
        return {
            -STRIDE * forward_scale,
            foot_y,
            FOOT_Z_STAND + LIFT * sinf(0.5F * PI * t)};
      }

      if (phase < STEP_END)
      {
        const float t = (phase - LIFT_END) / (STEP_END - LIFT_END);
        return {
            (-STRIDE + 2.0F * STRIDE * smoothstep(t)) * forward_scale,
            foot_y,
            FOOT_Z_STAND + LIFT * cosf(0.5F * PI * t)};
      }

      const float t = (phase - STEP_END) / (1.0F - STEP_END);
      return {
          (STRIDE * (1.0F - smoothstep(t))) * forward_scale,
          foot_y,
          FOOT_Z_STAND};
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
    if (_motion == MotionCommand::BOW)
    {
      write_bow(legs, time_s);
      return;
    }

    if (_motion == MotionCommand::PAW)
    {
      write_paw(legs, time_s);
      return;
    }

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
      return 1.50F;
    case GaitKind::TROT:
      return 0.90F;
    case GaitKind::GALLOP:
      return 0.60F;
    case GaitKind::TURN:
      return 0.60F;
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

    case GaitKind::TURN:
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
    }

    return 0.0F;
  }

  void GaitController::write_leg(
      std::array<Leg, 4>& legs, LegSide side, float time_s) const noexcept
  {
    const float direction = _motion == MotionCommand::BACKWARD ? -1.0F : 1.0F;
    const float turn_flip = (_gait == GaitKind::TURN && is_right_side(side)) ? -1.0F : 1.0F;
    const float period = period_seconds();
    const float phase = normalized_phase(time_s, period, offset_for(side));
    const bool current_leg_airborne = is_airborne(phase);

    float balance_y = 0.0F;
    float support_push_down = 0.0F;
    if (_gait == GaitKind::WALK)
    {
      for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
      {
        const auto other_side = static_cast<LegSide>(i);
        if (other_side == side)
        {
          continue;
        }

        const float other_phase = normalized_phase(time_s, period, offset_for(other_side));
        const float other_support_request = support_request_weight(other_phase);
        const float away_from_swing_side = is_right_side(other_side) ? -1.0F : 1.0F;
        balance_y += away_from_swing_side * WALK_BALANCE_SHIFT_Y * other_support_request;
        support_push_down = fmaxf(support_push_down, other_support_request);
      }
    }

    auto foot = foot_from_phase(
        phase,
        direction * turn_flip * _speed,
        side_stance_y(side) + balance_y);
    if (_gait == GaitKind::WALK && !current_leg_airborne)
    {
      foot.z -= WALK_SUPPORT_PUSH_DOWN * support_push_down;
    }

    const auto angles = solve_leg(foot, side);

    // DEBUG: print angles for leg 0 only (remove after tuning)
    static uint32_t last_print = 0;
    if (side == LegSide::TOP_LEFT && millis() - last_print > 500) {
      last_print = millis();
      Serial.printf("TL foot=(%.2f,%.2f,%.2f) coxa=%d femur=%d tibia=%d\n",
        foot.x, foot.y, foot.z,
        angles.coxa_md/1000, angles.femur_md/1000, angles.tibia_md/1000);
    }

    legs[static_cast<uint8_t>(side)].write_angles(
        angles.coxa_md, angles.femur_md, angles.tibia_md);
  }

  void GaitController::write_bow(std::array<Leg, 4>& legs, float time_s) const noexcept
  {
    const float phase = normalized_phase(time_s, BOW_PERIOD, 0.0F);
    const float pose  = sinf(phase * PI);

    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
      const auto side = static_cast<LegSide>(i);
      FootTarget foot = {0.0F, side_stance_y(side), FOOT_Z_STAND};

      if (is_front_side(side))
      {
        foot.x += BOW_FRONT_BACK * pose;
        foot.z -= BOW_FRONT_DROP * pose;
      }
      else
      {
        foot.z += BOW_REAR_RISE * pose;
      }

      const auto angles = solve_leg(foot, side);
      legs[i].write_angles(angles.coxa_md, angles.femur_md, angles.tibia_md);
    }
  }

  void GaitController::write_paw(std::array<Leg, 4>& legs, float time_s) const noexcept
  {
    constexpr LegSide paw_side = LegSide::TOP_RIGHT;
    const float phase          = normalized_phase(time_s, PAW_PERIOD, 0.0F);
    const float wave           = 0.5F - 0.5F * cosf(2.0F * PI * phase);

    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
      const auto side = static_cast<LegSide>(i);
      FootTarget foot = {0.0F, side_stance_y(side), FOOT_Z_STAND};

      if (side == paw_side)
      {
        foot.x = PAW_REST_X + (PAW_PEAK_X - PAW_REST_X) * wave;
        foot.z = PAW_REST_Z + (PAW_PEAK_Z - PAW_REST_Z) * wave;
      }
      else if (is_front_side(side))
      {
        foot.x = -0.80F;
      }
      else
      {
        foot.z = FOOT_Z_STAND - 1.00F;
      }

      const auto angles = solve_leg(foot, side);
      legs[i].write_angles(angles.coxa_md, angles.femur_md, angles.tibia_md);
    }
  }
} // namespace pluto::motion
