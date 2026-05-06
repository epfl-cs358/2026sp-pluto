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
  };

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
  };

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
  };
} // namespace pluto
