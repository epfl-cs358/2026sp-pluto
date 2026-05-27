#pragma once

#include <array>
#include <cstdint>

namespace pluto
{
  struct JointConfig
  {
    uint16_t raw_min;
    uint16_t raw_max;

    uint16_t raw_stand;
    uint16_t raw_forward;
    uint16_t raw_backward;

    int32_t angle_min_md;
    int32_t angle_max_md;

    bool inverted = false;
  };

  struct LegConfig
  {
    JointConfig coxa;
    JointConfig femur;
    JointConfig tibia;
  };

  /// @brief Identifiers for the physical position of each leg on the chassis.
  enum class LegSide : uint8_t
  {
    /// @brief Forward facing left leg
    TOP_LEFT = 0,
    /// @brief Forward facing right leg
    TOP_RIGHT = 1,
    /// @brief Rear facing left leg
    BOTTOM_LEFT = 2,
    /// @brief Rear facing right leg
    BOTTOM_RIGHT = 3,

    _count_LegSide
  };

  static constexpr size_t LEG_COUNT = 4;
  static constexpr size_t JOINT_COUNT = 3;

  static constexpr LegConfig LEG_CONFIGS[LEG_COUNT] = {
      // TOP LEFT
      // {raw_min, raw_max, raw_stand, raw_forward, raw_backward, angle_min_md, angle_max_md, inverted}
      // TODO: set the limits for each servo
      {
        {270, 375, 330, 330, 330, -30000, 30000, true},
        {240, 355, 320, 320, 300, 15000, 60000, false},
        {150, 305, 265, 265, 275, -95000, -40000, false}
      },
      // TOP RIGHT
      {
        {250, 350, 295, 295, 295, -30000, 30000, false},
        {180, 270, 215, 215, 235, 15000, 60000, true},
        {270, 435, 305, 305, 305, -95000, -40000, true}
      },
      // BOTTOM LEFT
      {
        {185, 285, 230, 230, 230, -30000, 30000, true},
        {210, 370, 285, 285, 300, 15000, 60000, false},
        {195, 370, 335, 335, 360, -95000, -40000, false}
      }, 
      // BOTTOM RIGHT
      {
        {258, 358, 313, 313, 313, -30000, 30000, false},
        {200, 355, 265, 265, 265, 15000, 60000, true},
        {230, 425, 255, 255, 260, -95000, -40000, true}
      }
  };
} // namespace pluto

#define PLUTO_EXPAND_LEG_DATA(leg_data) \
  leg_data.raw_min, leg_data.raw_max, leg_data.raw_stand, leg_data.raw_forward, leg_data.raw_backward, leg_data.angle_min_md, leg_data.angle_max_md
