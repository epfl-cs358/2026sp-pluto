#include <gz/plugin/Register.hh>
#include <gz/sim/Model.hh>
#include <gz/sim/System.hh>
#include <gz/sim/Util.hh>
#include <gz/sim/components/JointPositionReset.hh>
#include <gz/sim/components/Name.hh>

#include <array>
#include <cmath>

using namespace gz;
using namespace sim;

class PlutoGaitPlugin
    : public System,
      public ISystemConfigure,
      public ISystemPreUpdate
{
public:
  void Configure(
      const Entity &_entity,
      const std::shared_ptr<const sdf::Element> &,
      EntityComponentManager &_ecm,
      EventManager &) override
  {
    model_ = Model(_entity);

    joints_[0] = model_.JointByName(_ecm, "top_left_hip");
    joints_[1] = model_.JointByName(_ecm, "top_right_hip");
    joints_[2] = model_.JointByName(_ecm, "bottom_left_hip");
    joints_[3] = model_.JointByName(_ecm, "bottom_right_hip");
  }

  void PreUpdate(
      const UpdateInfo &_info,
      EntityComponentManager &_ecm) override
  {
    double time_s = std::chrono::duration<double>(_info.simTime).count();

    for (int i = 0; i < 4; ++i)
    {
      if (joints_[i] == kNullEntity)
        continue;

      double phase = normalizedPhase(time_s, PERIOD, offsetFor(i));
      double angle = legAngleFromPhase(phase);

      if (i == TOP_RIGHT || i == BOTTOM_RIGHT)
        angle = -angle;

      auto cmd = _ecm.Component<components::JointPositionReset>(joints_[i]);
      if (!cmd)
      {
        _ecm.CreateComponent(
            joints_[i],
            components::JointPositionReset({angle}));
      }
      else
      {
        cmd->Data()[0] = angle;
      }
    }
  }

private:
  enum LegSide
  {
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_LEFT = 2,
    BOTTOM_RIGHT = 3
  };

  static constexpr double PI = 3.141592653589793;
  static constexpr double SHIFT_END = 0.25;
  static constexpr double LIFT_END = 0.50;
  static constexpr double STEP_END = 0.75;

  static constexpr double STRIDE = 0.45;
  static constexpr double LIFT = 0.35;
  static constexpr double PERIOD = 2.5;

  static double smoothstep(double t)
  {
    return t * t * (3.0 - 2.0 * t);
  }

  static double normalizedPhase(double time_s, double period_s, double offset)
  {
    double phase = std::fmod(time_s / period_s + offset, 1.0);
    if (phase < 0.0)
      phase += 1.0;
    return phase;
  }

  static double offsetFor(int side)
  {
    switch (side)
    {
      case TOP_LEFT:
        return 0.0;
      case TOP_RIGHT:
        return 0.5;
      case BOTTOM_LEFT:
        return 0.25;
      case BOTTOM_RIGHT:
        return 0.75;
    }
    return 0.0;
  }

  static double legAngleFromPhase(double phase)
  {
    if (phase < SHIFT_END)
    {
      double t = phase / SHIFT_END;
      return -STRIDE * smoothstep(t);
    }

    if (phase < LIFT_END)
    {
      double t = (phase - SHIFT_END) / (LIFT_END - SHIFT_END);
      return -STRIDE + LIFT * std::sin(0.5 * PI * t);
    }

    if (phase < STEP_END)
    {
      double t = (phase - LIFT_END) / (STEP_END - LIFT_END);
      return -STRIDE + 2.0 * STRIDE * smoothstep(t);
    }

    double t = (phase - STEP_END) / (1.0 - STEP_END);
    return STRIDE * (1.0 - smoothstep(t));
  }

  Model model_{kNullEntity};
  std::array<Entity, 4> joints_{
      kNullEntity,
      kNullEntity,
      kNullEntity,
      kNullEntity};
};

GZ_ADD_PLUGIN(
    PlutoGaitPlugin,
    gz::sim::System,
    PlutoGaitPlugin::ISystemConfigure,
    PlutoGaitPlugin::ISystemPreUpdate)
