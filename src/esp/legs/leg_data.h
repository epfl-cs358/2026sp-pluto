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

  // + opens the leg
  static constexpr JointRawData LEG_DATA_TOP_RIGHT_BOTTOM = {155, 315, 300};
  // + opens the leg
  static constexpr JointRawData LEG_DATA_TOP_RIGHT_MIDDLE = {275, 365, 300};
  // + closes the leg
  static constexpr JointRawData LEG_DATA_TOP_RIGHT_TOP = {235, 375, 315};

  // + closes the leg
  static constexpr JointRawData LEG_DATA_TOP_LEFT_BOTTOM = {285, 445, 300};
  // + closes the leg
  static constexpr JointRawData LEG_DATA_TOP_LEFT_MIDDLE = {275, 365, 300};
  // + opens the leg
  static constexpr JointRawData LEG_DATA_TOP_LEFT_TOP = {240, 380, 300};

  // + closes the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_LEFT_BOTTOM = {270, 440, 300};
  // + opens the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_LEFT_MIDDLE = {200, 355, 300};
  // + closes the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_LEFT_TOP = {240, 380, 300};

  // + opens the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_RIGHT_BOTTOM = {160, 340, 300};
  // + closes the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_RIGHT_MIDDLE = {245, 400, 300};
  // + opens the leg
  static constexpr JointRawData LEG_DATA_BOTTOM_RIGHT_TOP = {240, 380, 300};

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
