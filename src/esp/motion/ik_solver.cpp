#include <motion/ik_solver.h>

#include <Arduino.h>
#include <cmath>

namespace pluto::motion
{
  namespace
  {
    constexpr float RADIANS_TO_DEGREES = 57.29577951308232F;

    float clamp_unit(float value) noexcept
    {
      if (value > 1.0F)
      {
        return 1.0F;
      }

      if (value < -1.0F)
      {
        return -1.0F;
      }

      return value;
    }
  } // namespace

  JointAngles solve_ik(
      FootTarget foot, float coxa_length, float femur_length, float tibia_length) noexcept
  {
    const float c_squared = foot.y * foot.y + foot.z * foot.z;
    const float d_squared = max(0.0F, c_squared - coxa_length * coxa_length);
    const float d         = sqrtf(d_squared);

    const float coxa_angle = atan2f(foot.y, -foot.z) + atan2f(-coxa_length, d);

    const float g = sqrtf(d * d + foot.x * foot.x);
    if (g <= 0.0001F)
    {
      return {coxa_angle, 0.0F, 0.0F};
    }

    const float n = (g * g - femur_length * femur_length - tibia_length * tibia_length)
                    / (2.0F * femur_length);

    const float tibia_angle = -acosf(clamp_unit(n / tibia_length));
    const float femur_angle =
        -atan2f(foot.x, d) + acosf(clamp_unit((femur_length + n) / g));

    return {coxa_angle, femur_angle, tibia_angle};
  }

  int32_t radians_to_millidegrees(float radians) noexcept
  {
    return static_cast<int32_t>(lroundf(radians * RADIANS_TO_DEGREES * 1000.0F));
  }

  JointAnglesMd to_millidegrees(JointAngles angles) noexcept
  {
    return {
        radians_to_millidegrees(angles.coxa_rad),
        radians_to_millidegrees(angles.femur_rad),
        radians_to_millidegrees(angles.tibia_rad)};
  }
} // namespace pluto::motion
