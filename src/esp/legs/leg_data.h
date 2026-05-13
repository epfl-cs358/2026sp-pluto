#pragma once

#include <cstdint>

namespace pluto
{
  struct JointRawData
  {
    uint16_t raw_min;
    uint16_t raw_max;
    uint16_t raw_start;
  };

  struct JointCalibration
  {
    int32_t angle_min_md;
    int32_t angle_max_md;
  };

  struct LegCalibration
  {
    JointCalibration top;
    JointCalibration middle;
    JointCalibration bottom;
  };

  static constexpr JointRawData LEG_DATA_TOP  = {250, 500, 315};
  static constexpr JointRawData LEG_DATA_COXA = LEG_DATA_TOP;

  static constexpr JointRawData LEG_DATA_MIDDLE = {100, 240, 150};
  static constexpr JointRawData LEG_DATA_FEMUR  = LEG_DATA_MIDDLE;

  static constexpr JointRawData LEG_DATA_BOTTOM = {340, 465, 400};
  static constexpr JointRawData LEG_DATA_TIBIA  = LEG_DATA_BOTTOM;

  static constexpr JointCalibration CAL_TOP = {-36000, 58000};

  static constexpr LegCalibration LEG_CALIBRATIONS[] = {
      // TOP_LEFT: TODO replace with measured values.
      {CAL_TOP, {-66000, 45000}, {-50000, 17000}},
      // TOP_RIGHT / front right.
      {CAL_TOP, {-66000, -25000}, {-50000, 15000}},
      // BOTTOM_LEFT / back left.
      {CAL_TOP, {-13000, 45000}, {-50000, 17000}},
      // BOTTOM_RIGHT: TODO replace with measured values.
      {CAL_TOP, {-66000, 45000}, {-50000, 17000}},
  };
} // namespace pluto

#define PLUTO_EXPAND_LEG_DATA(leg_data) \
  leg_data.raw_min, leg_data.raw_max, leg_data.raw_start
