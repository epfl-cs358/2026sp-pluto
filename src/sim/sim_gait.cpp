#include <sim/sim_gait.h>

#include <algorithm>
#include <cmath>
#include <motion/ik_solver.h>

namespace pluto::sim
{
  namespace
  {
    constexpr float COXA_LENGTH  = 7.00F;
    constexpr float FEMUR_LENGTH = 12.00F;
    constexpr float TIBIA_LENGTH = 13.50F;

    constexpr float FOOT_Z_STAND = -23.0F; // standing height

    constexpr float SHIFT_END                = 0.25F;
    constexpr float LIFT_END                 = 0.50F;
    constexpr float STEP_END                 = 0.75F;
    constexpr float PHASE_QUANTIZATION_STEPS = 10.0F;
    constexpr float FOOT_X_QUANTIZATION_STEP = 0.25F;
    constexpr float FOOT_Z_QUANTIZATION_STEP = 0.25F;

    constexpr float STRIDE = 1.80F;
    constexpr float LIFT   = 3.20F;

    constexpr float FOOT_Y_STANCE               = 7.00F;
    constexpr float WALK_REAR_LEG_EXTEND_Z      = 0.00F;
    constexpr float WALK_FRONT_STRIDE_SCALE     = 0.82F;
    constexpr float WALK_REAR_STRIDE_SCALE      = 0.82F;
    constexpr float WALK_FRONT_SWING_DROP_Z     = 0.00F;
    constexpr float WALK_FRONT_LEAN_DROP_Z      = 0.00F;
    constexpr float WALK_REAR_LEAN_RISE_Z       = 0.00F;
    constexpr float WALK_REAR_X_BIAS            = 0.00F;
    constexpr float WALK_LF_EXTRA_DROP_Z        = 0.00F;
    constexpr int32_t WALK_REAR_TIBIA_EXTEND_MD = 0;
    constexpr int32_t WALK_LF_FEMUR_FLAT_MD     = 30000;
    constexpr int32_t WALK_RF_FEMUR_FLAT_MD     = 0;
    constexpr float BOW_FRONT_DROP              = 5.00F;
    constexpr float BOW_REAR_RISE               = 2.00F;
    constexpr float BOW_FRONT_BACK              = 1.50F;
    constexpr float BOW_PERIOD                  = 2.00F;

    constexpr float PAW_REST_X               = 1.20F;
    constexpr float PAW_REST_Z               = -16.20F;
    constexpr float PAW_PEAK_X               = 2.60F;
    constexpr float PAW_PEAK_Z               = -15.20F;
    constexpr float PAW_PERIOD               = 1.40F;
    constexpr float PAW_SHIFT_Y              = 1.60F;
    constexpr float PAW_REAR_SUPPORT_DROP_Z  = 1.80F;
    constexpr float PAW_FRONT_SUPPORT_BACK_X = -1.00F;

    constexpr float PI = 3.14159265F;

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

    float quantize_step(float value, float step) noexcept
    {
      if (step <= 0.0F)
      {
        return value;
      }

      return roundf(value / step) * step;
    }

    float quantize_phase(float phase) noexcept
    {
      if (PHASE_QUANTIZATION_STEPS <= 0.0F)
      {
        return phase;
      }

      return roundf(phase * PHASE_QUANTIZATION_STEPS) / PHASE_QUANTIZATION_STEPS;
    }

    float smoothstep(float t) noexcept
    {
      return t * t * (3.0F - 2.0F * t);
    }

    bool is_airborne(float phase) noexcept
    {
      return phase >= SHIFT_END
             && phase
                    < STEP_END; // add multiplier to where the angles is fed (like 2) whenevern it should be higher
    }

    float side_stance_y(LegSide side) noexcept
    {
      return is_right_side(side) ? -FOOT_Y_STANCE : FOOT_Y_STANCE;
    }

    pluto::motion::FootTarget foot_from_phase(
        float phase, float forward_scale, float foot_y, float stride, float lift,
        float z_stand) noexcept
    {
      if (phase < SHIFT_END)
      {
        const float t = phase / SHIFT_END;
        return {(stride * smoothstep(t)) * forward_scale, foot_y, z_stand};
      }

      if (phase < LIFT_END)
      {
        const float t = (phase - SHIFT_END) / (LIFT_END - SHIFT_END);
        return {
            stride * forward_scale, foot_y, z_stand + lift * sinf(0.5F * PI * t)};
      }

      if (phase < STEP_END)
      {
        const float t = (phase - LIFT_END) / (STEP_END - LIFT_END);
        return {
            (stride - 2.0F * stride * smoothstep(t)) * forward_scale, foot_y,
            z_stand + lift * cosf(0.5F * PI * t)};
      }

      const float t = (phase - STEP_END) / (1.0F - STEP_END);
      return {(-stride * (1.0F - smoothstep(t))) * forward_scale, foot_y, z_stand};
    }

    pluto::motion::JointAnglesMd solve_leg(pluto::motion::FootTarget foot, LegSide side) noexcept
    {
      const float signed_coxa = is_right_side(side) ? -COXA_LENGTH : COXA_LENGTH;
      return to_millidegrees(
          solve_ik(foot, signed_coxa, FEMUR_LENGTH, TIBIA_LENGTH));
    }

    constexpr const char* leg_name(LegSide side) noexcept
    {
      switch (side)
      {
      case LegSide::TOP_LEFT:
        return "TL";
      case LegSide::TOP_RIGHT:
        return "TR";
      case LegSide::BOTTOM_LEFT:
        return "BL";
      case LegSide::BOTTOM_RIGHT:
        return "BR";
      default:
        return "??";
      }
    }

    int32_t raw_to_angle_md(const JointConfig& config, uint16_t raw) noexcept
    {
      uint16_t normalized_raw = raw;
      if (config.inverted)
      {
        normalized_raw = config.raw_max - (raw - config.raw_min);
      }

      return config.angle_min_md
             + static_cast<int32_t>(
                 static_cast<int64_t>(normalized_raw - config.raw_min)
                 * static_cast<int64_t>(config.angle_max_md - config.angle_min_md)
                 / static_cast<int64_t>(config.raw_max - config.raw_min));
    }

    pluto::motion::JointAnglesMd standing_servo_angles_md(LegSide side) noexcept
    {
      const auto& config = LEG_CONFIGS[static_cast<uint8_t>(side)];
      return {
          raw_to_angle_md(config.coxa, config.coxa.raw_start),
          raw_to_angle_md(config.femur, config.femur.raw_start),
          raw_to_angle_md(config.tibia, config.tibia.raw_start)};
    }

    pluto::motion::JointAnglesMd standing_ik_angles_md(LegSide side) noexcept
    {
      return solve_leg({0.0F, side_stance_y(side), FOOT_Z_STAND}, side);
    }

    pluto::motion::JointAnglesMd standing_angle_offsets_md(LegSide side) noexcept
    {
      const auto servo = standing_servo_angles_md(side);
      const auto ik    = standing_ik_angles_md(side);
      return {
          servo.coxa_md - ik.coxa_md, servo.femur_md - ik.femur_md,
          servo.tibia_md - ik.tibia_md};
    }

    pluto::motion::JointAnglesMd apply_standing_offsets(pluto::motion::JointAnglesMd angles, LegSide side) noexcept
    {
      const auto offsets = standing_angle_offsets_md(side);
      angles.coxa_md += offsets.coxa_md;
      angles.femur_md += offsets.femur_md;
      angles.tibia_md += offsets.tibia_md;
      return angles;
    }
  } // namespace

  void SimGaitController::set_gait(SimGaitKind gait) noexcept
  {
    _gait = gait;
  }

  void SimGaitController::set_motion(SimMotionCommand motion) noexcept
  {
    _motion = motion;
  }

  void SimGaitController::set_speed(float speed) noexcept
  {
    _speed = std::clamp(speed, 0.0F, 1.0F);
  }

  void SimGaitController::set_walk_manual_phase(bool enabled) noexcept
  {
    _walk_manual_phase_enabled = enabled;
    _walk_manual_stage         = 0;
  }

  void SimGaitController::next_walk_manual_stage() noexcept
  {
    _walk_manual_stage = static_cast<uint8_t>((_walk_manual_stage + 1) % 4);
  }

  void SimGaitController::next_walk_manual_leg() noexcept
  {
    _walk_manual_leg = pluto::sim::next_leg_side(_walk_manual_leg);
  }

  void SimGaitController::stand(std::array<pluto::sim::SimLeg, 4>& legs) const noexcept
  {
    for (auto& leg : legs)
    {
      leg.write_starting();
    }
  }

  void SimGaitController::update(
      std::array<pluto::sim::SimLeg, 4>& legs, uint32_t now_ms) const noexcept
  {
    if (_motion == SimMotionCommand::IDLE || _speed <= 0.0F)
    {
      return;
    }

    const float time_s = static_cast<float>(now_ms) * 0.001F;
    if (_motion == SimMotionCommand::BOW)
    {
      write_bow(legs, time_s);
      return;
    }

    if (_motion == SimMotionCommand::PAW)
    {
      write_paw(legs, time_s);
      return;
    }

    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
      write_leg(legs, static_cast<LegSide>(i), time_s);
    }
  }

  float SimGaitController::period_seconds() const noexcept
  {
    switch (_gait)
    {
    case SimGaitKind::WALK:
      return 5.50F;
    case SimGaitKind::TROT:
      return 0.90F;
    case SimGaitKind::GALLOP:
      return 0.60F;
    case SimGaitKind::TURN:
      return 0.60F;
    }

    return 0.35F;
  }

  float SimGaitController::offset_for(LegSide side) const noexcept
  {
    switch (_gait)
    {
    case SimGaitKind::WALK:
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

    case SimGaitKind::TROT:
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

    case SimGaitKind::GALLOP:
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

    case SimGaitKind::TURN:
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

  float SimGaitController::phase_for(
      LegSide side, float time_s, float period) const noexcept
  {
    if (!(_gait == SimGaitKind::WALK && _walk_manual_phase_enabled))
    {
      return normalized_phase(time_s, period, offset_for(side));
    }

    // Manual single-leg walk debugging:
    // 0: pre-shift, 1: lift, 2: step forward, 3: place/support.
    static constexpr float STAGE_PHASES[4] = {0.12F, 0.38F, 0.62F, 0.88F};
    if (side == _walk_manual_leg)
    {
      return STAGE_PHASES[_walk_manual_stage];
    }

    // Keep non-active legs in a stable support phase.
    return 0.88F;
  }

  void SimGaitController::write_leg(
      std::array<pluto::sim::SimLeg, 4>& legs, LegSide side, float time_s) const noexcept
  {
    const bool explicit_right_turn = _motion == SimMotionCommand::RIGHT;
    const bool explicit_left_turn  = _motion == SimMotionCommand::LEFT;

    float direction = _motion == SimMotionCommand::BACKWARD ? -1.0F : 1.0F;
    float turn_flip = 1.0F;

    if (explicit_right_turn)
    {
      direction = 1.0F;
      turn_flip = is_right_side(side) ? -1.0F : 1.0F;
    }
    else if (explicit_left_turn)
    {
      direction = 1.0F;
      turn_flip = is_right_side(side) ? 1.0F : -1.0F;
    }
    else if (_gait == SimGaitKind::TURN)
    {
      turn_flip = is_right_side(side) ? -1.0F : 1.0F;
    }

    const float period = period_seconds();
    const float phase  = quantize_phase(phase_for(side, time_s, period));
    const bool current_leg_airborne = is_airborne(phase);

    float leg_stride_scale = 1.0F;
    if (_gait == SimGaitKind::WALK)
    {
      leg_stride_scale =
          is_front_side(side) ? WALK_FRONT_STRIDE_SCALE : WALK_REAR_STRIDE_SCALE;
    }

    float current_stride  = STRIDE;
    float current_lift    = LIFT;
    float current_z_stand = FOOT_Z_STAND;

    if (_gait == SimGaitKind::TROT) //  && !is_front_side(side)
    {
      current_stride *= 0.70F; // Limits Femur forward/backward sweep
      current_lift *= 0.50F;   // Limits Tibia upward compression
    }

    auto foot = foot_from_phase(
        phase, direction * turn_flip * _speed * leg_stride_scale,
        side_stance_y(side), current_stride, current_lift, current_z_stand);

    if (_gait == SimGaitKind::WALK && !is_front_side(side))
    {
      foot.x += WALK_REAR_X_BIAS;
    }

    if (_gait == SimGaitKind::WALK && !is_front_side(side))
    {
      foot.z -= WALK_REAR_LEG_EXTEND_Z;
    }

    if (_gait == SimGaitKind::WALK)
    {
      if (is_front_side(side))
      {
        foot.z -= WALK_FRONT_LEAN_DROP_Z;
        if (side == LegSide::TOP_LEFT)
        {
          foot.z += WALK_LF_EXTRA_DROP_Z;
        }
        if (current_leg_airborne)
        {
          foot.z -= WALK_FRONT_SWING_DROP_Z;
        }
      }
      else
      {
        foot.z += WALK_REAR_LEAN_RISE_Z;
      }
    }

    foot.x = quantize_step(foot.x, FOOT_X_QUANTIZATION_STEP);
    foot.z = quantize_step(foot.z, FOOT_Z_QUANTIZATION_STEP);

    auto angles = solve_leg(foot, side);
    if (_gait == SimGaitKind::WALK && side == LegSide::TOP_LEFT)
    {
      angles.femur_md -= WALK_LF_FEMUR_FLAT_MD;
    }
    if (_gait == SimGaitKind::WALK && side == LegSide::TOP_RIGHT)
    {
      angles.femur_md += WALK_RF_FEMUR_FLAT_MD;
    }

    if (_gait == SimGaitKind::WALK && !is_front_side(side))
    {
      angles.tibia_md += WALK_REAR_TIBIA_EXTEND_MD;
    }

    const auto servo_angles = apply_standing_offsets(angles, side);

    legs[static_cast<uint8_t>(side)].write_angles(
        servo_angles.coxa_md, servo_angles.femur_md, servo_angles.tibia_md);
  }

  void SimGaitController::write_bow(
      std::array<pluto::sim::SimLeg, 4>& legs, float time_s) const noexcept
  {
    const float phase = normalized_phase(time_s, BOW_PERIOD, 0.0F);
    const float pose  = sinf(phase * PI);

    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
        const auto side = static_cast<LegSide>(i);
        pluto::motion::FootTarget foot = {0.0F, side_stance_y(side), FOOT_Z_STAND};

        if (is_front_side(side))
        {
            foot.x += BOW_FRONT_BACK * pose;
            foot.z -= BOW_FRONT_DROP * pose;
        }
        else
        {
            foot.z += BOW_REAR_RISE * pose;
        }

        const auto servo_angles = apply_standing_offsets(solve_leg(foot, side), side);
        legs[i].write_angles(
            servo_angles.coxa_md, servo_angles.femur_md, servo_angles.tibia_md);
}
  }

  void SimGaitController::write_paw(
      std::array<pluto::sim::SimLeg, 4>& legs, float time_s) const noexcept
  {
    constexpr LegSide paw_side = LegSide::TOP_RIGHT;
    const float phase          = normalized_phase(time_s, PAW_PERIOD, 0.0F);
    const float shift_up       = smoothstep(fminf(1.0F, phase / 0.30F));
    const float shift_down     = smoothstep(fmaxf(0.0F, (phase - 0.85F) / 0.15F));
    const float shift_weight   = shift_up * (1.0F - shift_down);
    const float wave           = 0.5F - 0.5F * cosf(2.0F * PI * phase);
    const float swing_scale    = shift_weight;
    const float away_from_paw  = is_right_side(paw_side) ? 1.0F : -1.0F;

    for (uint8_t i = 0; i < static_cast<uint8_t>(LegSide::_count_LegSide); ++i)
    {
        const auto side = static_cast<LegSide>(i);
        pluto::motion::FootTarget foot = 
        {
            0.0F, 
            side_stance_y(side) + away_from_paw * PAW_SHIFT_Y * shift_weight,
            FOOT_Z_STAND
        };

        if (side == paw_side)
        {
            foot.x = PAW_REST_X + (PAW_PEAK_X - PAW_REST_X) * wave * swing_scale;
            foot.z = PAW_REST_Z + (PAW_PEAK_Z - PAW_REST_Z) * wave * swing_scale;
        }
        else if (is_front_side(side))
        {
            foot.x = PAW_FRONT_SUPPORT_BACK_X;
        }
        else
        {
            foot.z = FOOT_Z_STAND - PAW_REAR_SUPPORT_DROP_Z;
        }

        const auto servo_angles = apply_standing_offsets(solve_leg(foot, side), side);
        legs[i].write_angles(
            servo_angles.coxa_md, servo_angles.femur_md, servo_angles.tibia_md);
    }
  }
} // namespace pluto::motion
