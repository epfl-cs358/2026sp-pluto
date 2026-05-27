#pragma once

#include <cstdint>
#include <array>

#include <legs/leg_joint.h>
#include <legs/leg_data.h>

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
    default:
      return "UNKNOWN JOINT";
    }
  }

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
    default:
      return "UNKNOWN LEG SIDE";
    }
  }

  class Leg
  {
  public:
    static constexpr uint8_t CHANNEL_STEPS_PER_SIDE = 4;

    Leg(Adafruit_PWMServoDriver& pwm, LegSide side) noexcept
        : _side(side)
        , _joints{
            LegJoint(
              pwm, 
              static_cast<uint8_t>(side) * CHANNEL_STEPS_PER_SIDE + 0,
              LEG_CONFIGS[static_cast<uint8_t>(side)].coxa),

            LegJoint(
              pwm,
              static_cast<uint8_t>(side) * CHANNEL_STEPS_PER_SIDE + 1,
              LEG_CONFIGS[static_cast<uint8_t>(side)].femur),
            
            LegJoint(
              pwm,
              static_cast<uint8_t>(side) * CHANNEL_STEPS_PER_SIDE + 2,
              LEG_CONFIGS[static_cast<uint8_t>(side)].tibia)
        }
    {
    }
    Leg(Leg&&) noexcept                 = default;
    Leg(const Leg&) noexcept            = default;
    Leg& operator=(Leg&&) noexcept      = default;
    Leg& operator=(const Leg&) noexcept = default;

    /// @brief Indexes into the joints
    /// @param type The LegJointType
    /// @return LegJoint reference
    LegJoint& operator[](LegJointType type) noexcept
    {
      return _joints[static_cast<uint8_t>(type)];
    }

    /// @brief Indexes into the joints (const version)
    /// @param type The LegJointType
    /// @return LegJoint const reference
    const LegJoint& operator[](LegJointType type) const noexcept
    {
      return _joints[static_cast<size_t>(type)];
    }

    /// @brief Moves all joints in the leg to their starting positions.
    void write_standing() noexcept
    {
      for (auto& joint : _joints) 
      {
        joint.write_standing();
      }
    }

    /// @brief Moves all joints in the leg to the forward starting position.
    void write_forward_start() noexcept
    {
      for (auto& joint : _joints) 
      {
        joint.write_forward_start();
      }
    }

    /// @brief Moves all joints in the leg to the backward starting position.
    void write_backward_start() noexcept
    {
      for (auto& joint : _joints) 
      {
        joint.write_backward_start();
      }
    }

    /// @brief Keeps compatibility with the old starting position function.
    void write_starting() noexcept
    {
      write_standing();
    }

    /// @brief Sets all three joint angles simultaneously.
    /// @param coxa_md The coxa angle in millidegrees
    /// @param femur_md The femur angle in millidegrees
    /// @param tibia_md The tibia angle in millidegrees
    void write_angles(int32_t coxa_md, int32_t femur_md, int32_t tibia_md) noexcept
    {
      _joints[0].write_angle(coxa_md);
      _joints[1].write_angle(femur_md);
      _joints[2].write_angle(tibia_md);
    }
    /// @brief Sets all three joint raw values simultaneously
    /// @param coxa_raw The coxa raw PWM
    /// @param femur_raw The femor raw PWM
    /// @param tibia_raw The tibia raw PWM
    void write_raws(int16_t coxa_raw, int16_t femur_raw, int16_t tibia_raw) noexcept
    {
      _joints[0].write_raw(coxa_raw);
      _joints[1].write_raw(femur_raw);
      _joints[2].write_raw(tibia_raw);
    }

    /// @brief Function template to apply an operation to each joint in the leg.
    /// @tparam Func The type of the function or lambda to apply to each joint. It should take a LegJoint& as a parameter.
    /// @param fn The function or lambda to apply to each joint.
    template<typename Func>
    void for_each_joint(Func&& fn) noexcept 
    {
      for (auto& joint : _joints) 
      {
        fn(joint);
      }
    }

    /// @brief Returns the side of the leg.
    /// @return The leg side.
    LegSide side() const noexcept 
    {
      return _side;
    }

    private:
      /// @brief The side of the leg on the chassis.
      LegSide _side;
      /// @brief The three joints of the leg: coxa, femur, tibia.
      std::array<LegJoint, JOINT_COUNT> _joints;
  };
} // namespace pluto
