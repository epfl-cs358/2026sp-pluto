#pragma once

#include <cstdint>

namespace pluto::motion
{
  struct FootTarget
  {
    float x;
    float y;
    float z;
  };

  struct JointAngles
  {
    float coxa_rad;
    float femur_rad;
    float tibia_rad;
  };

  struct JointAnglesMd
  {
    int32_t coxa_md;
    int32_t femur_md;
    int32_t tibia_md;
  };

  JointAngles solve_ik(
      FootTarget foot, float coxa_length, float femur_length, float tibia_length) noexcept;

  int32_t radians_to_millidegrees(float radians) noexcept;

  JointAnglesMd to_millidegrees(JointAngles angles) noexcept;
} // namespace pluto::motion
