#pragma once

#include <array>
#include <cstdint>

namespace pluto
{
  struct JointConfig
  {
    uint16_t raw_min;
    uint16_t raw_max;
    uint16_t raw_start;

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
      // {raw_min, raw_max, raw_start, angle_min_md, angle_max_md, inverted}
      {
        {240, 380, 300, -30000, 30000, false}, // + opens the leg
        {275, 365, 300,  15000, 60000, true},  // + closes the leg
        {285, 445, 300, -95000, -40000, true}  // + closes the leg
      },
      // TOP RIGHT
      {
        {235, 375, 320, -30000, 30000, true},  // + closes the leg
        {275, 365, 300,  15000, 60000, false}, // + opens the leg
        {155, 315, 300, -95000, -40000, false} // + opens the leg
      },
      // BOTTOM LEFT
      {
        {240, 380, 310, -30000, 30000, true},  // + closes the leg
        {200, 355, 300,  15000, 60000, false}, // + opens the leg
        {270, 440, 300, -95000, -40000, true}  // + closes the leg
      },
      // BOTTOM RIGHT
      {
        {240, 380, 300, -30000, 30000, false}, // + opens the leg
        {245, 400, 300,  15000, 60000, true},  // + closes the leg
        {160, 340, 300, -95000, -40000, false} // + opens the leg
      }
  };
} // namespace pluto

#define PLUTO_EXPAND_LEG_DATA(leg_data) \
  leg_data.raw_min, leg_data.raw_max, leg_data.raw_start, leg_data.angle_min_md, leg_data.angle_max_md
