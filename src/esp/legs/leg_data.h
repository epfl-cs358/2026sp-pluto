#pragma once

#include <cstdint>

namespace pluto
{
  struct LegData
  {
    uint16_t raw_min;
    uint16_t raw_max;
    uint16_t raw_start;
    int32_t angle_min_md;
    int32_t angle_max_md;
  };

  // TODO: compute angles

  static constexpr LegData LEG_DATA_TOP  = {250, 500, 315, 0, 1};
  static constexpr LegData LEG_DATA_COXA = LEG_DATA_TOP;

  static constexpr LegData LEG_DATA_MIDDLE = {100, 240, 150, 0, 1};
  static constexpr LegData LEG_DATA_FEMUR  = LEG_DATA_MIDDLE;

  static constexpr LegData LEG_DATA_BOTTOM = {340, 465, 400, 0, 1};
  static constexpr LegData LEG_DATA_TIBIA  = LEG_DATA_BOTTOM;
} // namespace pluto

#define PLUTO_EXPAND_LEG_DATA(leg_data)                                          \
  leg_data.raw_min, leg_data.raw_max, leg_data.raw_start, leg_data.angle_min_md, \
      leg_data.angle_max_md
