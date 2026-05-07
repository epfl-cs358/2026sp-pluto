#pragma once

#include <legs/leg_joint.h>
#include <legs/leg_data.h>
#include <cstdint>

namespace pluto
{
  /// @brief Identifiers for the three degrees of freedom in a robotic leg.
  enum class LegJointType : uint8_t
  {
    /// @brief Proximal joint attached to the chassis.
    /// Controls horizontal rotation (abduction/adduction) for swing and orientation.
    TOP  = 0,
    COXA = TOP,
    /// @brief Intermediate joint controlling the upper leg segment.
    /// Handles vertical rotation to determine leg height and step extension.
    MIDDLE = 1,
    FEMUR  = MIDDLE,
    /// @brief Distal joint controlling the lower leg segment.
    /// Handles vertical rotation of the knee to manage reach and ground contact.
    BOTTOM = 2,
    TIBIA  = BOTTOM,

    /// @brief Count of enums. Do not use!
    _count_LegJointType,
  };

  /// @brief Returns the next joint type.
  /// After BOTTOM, this returns TOP.
  /// @param joint The current joint type
  /// @return The next joint type
  constexpr LegJointType next_leg_joint_type(LegJointType joint) noexcept
  {
    return (LegJointType)(((uint8_t)joint + 1)
                          % (uint8_t)LegJointType::_count_LegJointType);
  }

  /// @brief Converts a LegJointType to a string.
  /// @param joint The joint type
  /// @return String
  constexpr const char* str_leg_joint_type(LegJointType joint) noexcept
  {
    switch (joint)
    {
    case LegJointType::TOP:
      return "COXA (TOP)";
    case LegJointType::MIDDLE:
      return "FEMUR (MIDDLE)";
    case LegJointType::BOTTOM:
      return "TIBIA (BOTTOM)";
    }
  }

  /// @brief Identifiers for the physical position of each leg on the chassis.
  enum class LegSide : uint8_t
  {
    /// @brief Forward-facing left leg.
    TOP_LEFT = 0,
    /// @brief Forward-facing right leg.
    TOP_RIGHT = 1,
    /// @brief Rear-facing left leg.
    BOTTOM_LEFT = 2,
    /// @brief Rear-facing right leg.
    BOTTOM_RIGHT = 3,

    /// @brief Count of enums. Do not use!
    _count_LegSide,
  };

  /// @brief Returns the next leg side.
  /// @param joint The current leg side
  /// @return The next leg side
  constexpr LegSide next_leg_side(LegSide side) noexcept
  {
    return (LegSide)(((uint8_t)side + 1) % (uint8_t)LegSide::_count_LegSide);
  }

  constexpr const char* str_leg_side(LegSide side) noexcept
  {
    switch (side)
    {
    case LegSide::TOP_LEFT:
      return "TOP_LEFT";
    case LegSide::TOP_RIGHT:
      return "TOP_RIGHT";
    case LegSide::BOTTOM_LEFT:
      return "BOTTOM_LEFT";
    case LegSide::BOTTOM_RIGHT:
      return "BOTTOM_RIGHT";
    }
  }

  class Leg
  {
    LegJoint<PLUTO_EXPAND_LEG_DATA(LEG_DATA_TOP)> top;
    LegJoint<PLUTO_EXPAND_LEG_DATA(LEG_DATA_MIDDLE)> middle;
    LegJoint<PLUTO_EXPAND_LEG_DATA(LEG_DATA_BOTTOM)> bottom;

  public:
    Leg(Adafruit_PWMServoDriver& pwm, LegSide side) noexcept
        : top(pwm, (uint8_t)side * 3 + 0)
        , middle(pwm, (uint8_t)side * 3 + 1)
        , bottom(pwm, (uint8_t)side * 3 + 2)
    {
    }
    Leg(Leg&&) noexcept                 = default;
    Leg(const Leg&) noexcept            = default;
    Leg& operator=(Leg&&) noexcept      = default;
    Leg& operator=(const Leg&) noexcept = default;

    /// @brief Indexes into the joints
    /// @param type The LegJointType
    /// @return View over a LegJoint
    LegJointView operator[](LegJointType type) noexcept
    {
      switch (type)
      {
      case LegJointType::COXA:
        return LegJointView(top);
      case LegJointType::FEMUR:
        return LegJointView(middle);
      case LegJointType::TIBIA:
        return LegJointView(bottom);
      }
    }

    /// @brief For each joint, call a function.
    /// The functor should usually be a templated lambda.
    /// @tparam T The lambda
    /// @param functor The functor
    template<typename T>
    void for_each_joint(T&& functor) noexcept
    {
      functor(top);
      functor(middle);
      functor(bottom);
    }

    /// @brief Moves all joints in the leg to their starting positions.
    void write_starting() noexcept
    {
      top.write_starting();
      middle.write_starting();
      bottom.write_starting();
    }

    /// @brief Sets all three joint angles simultaneously.
    /// @param coxa_md The coxa angle in millidegrees
    /// @param femur_md The femur angle in millidegrees
    /// @param tibia_md The tibia angle in millidegrees
    void write_angles(int32_t coxa_md, int32_t femur_md, int32_t tibia_md) noexcept
    {
      top.write_angle(coxa_md);
      middle.write_angle(femur_md);
      bottom.write_angle(tibia_md);
    }
    /// @brief Sets all three joint raw values simultaneously
    /// @param coxa_raw The coxa raw PWM
    /// @param femur_raw The femor raw PWM
    /// @param tibia_raw The tibia raw PWM
    void write_raws(int16_t coxa_raw, int16_t femur_raw, int16_t tibia_raw) noexcept
    {
      top.write_raw(coxa_raw);
      middle.write_raw(femur_raw);
      bottom.write_raw(tibia_raw);
    }
  };
} // namespace pluto
