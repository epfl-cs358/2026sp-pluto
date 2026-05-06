#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include <array>
#include <utility>

namespace pluto
{
  /// @brief A class to represent and control a single robotic leg joint using a PCA9685 PWM driver.
  /// @tparam MIN The minimum raw pulse of that joint
  /// @tparam MAX The maximum raw pulse of that joint
  /// @tparam STARTING The starting raw pulse of that joint
  /// @tparam ANGLE_MIN_MD The logical minimum angle of the joint in millidegrees.
  /// @tparam ANGLE_MAX_MD The logical maximum angle of the joint in millidegrees.
  template<
      size_t MIN, size_t MAX, size_t STARTING, int32_t ANGLE_MIN_MD,
      int32_t ANGLE_MAX_MD>
  class LegJoint
  {
    static_assert(MIN < MAX, "MIN must be smaller than MAX");
    static_assert(STARTING >= MIN && STARTING <= MAX, "STARTING pulse out of range");
    static_assert(ANGLE_MIN_MD != ANGLE_MAX_MD, "Angle range cannot be zero");

  public:
    LegJoint(LegJoint&&) noexcept                 = default;
    LegJoint(const LegJoint&) noexcept            = default;
    LegJoint& operator=(LegJoint&&) noexcept      = default;
    LegJoint& operator=(const LegJoint&) noexcept = default;
    /// @brief Constructs a LegJoint and initializes the servo to the starting pulse.
    /// @param driver Pointer to the Adafruit_PWMServoDriver instance.
    /// @param channel The PCA9685 channel index (0-15).
    LegJoint(Adafruit_PWMServoDriver& driver, uint8_t channel)
        : _pwm(&driver)
        , _current(STARTING)
        , _channel(channel)
    {
      _pwm->setPWM(_channel, 0, _current);
    }

    /// @brief Directly writes a raw PWM pulse width to the servo.
    /// @param pulse The 12-bit pulse width value, constrained by MIN and MAX.
    void write_raw(uint16_t pulse) noexcept
    {
      _current = constrain(pulse, MIN, MAX);
      _pwm->setPWM(_channel, 0, _current);
    }
    /// @brief Sets the joint position using a logical angle in millidegrees.
    /// @param millidegrees The target angle in millidegrees, mapped to the PWM range.
    void write_angle(int32_t millidegrees) noexcept
    {
      millidegrees = constrain(millidegrees, ANGLE_MIN_MD, ANGLE_MAX_MD);

      // Linear mapping: P = MIN + (A - Amin) * (Pmax - Pmin) / (Amax - Amin)
      uint16_t pulse = MIN
                       + (uint32_t)(millidegrees - ANGLE_MIN_MD) * (MAX - MIN)
                             / (ANGLE_MAX_MD - ANGLE_MIN_MD);
      write_raw(pulse);
    }
    /// @brief Sets the joint to its starting position
    void write_starting() noexcept { write_raw(STARTING); }

    /// @brief Gets the PCA9685 channel assigned to this joint.
    /// @return The channel index.
    uint8_t channel() const noexcept { return _channel; }
    /// @brief Gets the last written raw PWM pulse width.
    /// @return The 12-bit PWM value.
    uint16_t current_raw() const noexcept { return _current; }
    /// @brief Calculates the current logical angle based on the last written pulse.
    /// @return The current angle in millidegrees.
    int32_t current_angle() const noexcept
    {
      return ANGLE_MIN_MD
             + (int32_t)(_current - MIN) * (ANGLE_MAX_MD - ANGLE_MIN_MD)
                   / (MAX - MIN);
    }

    static constexpr size_t RAW_MIN   = MIN;
    static constexpr size_t RAW_MAX   = MAX;
    static constexpr size_t RAW_START = STARTING;
    static constexpr size_t ANGLE_MIN = ANGLE_MIN_MD;
    static constexpr size_t ANGLE_MAX = ANGLE_MAX_MD;

  private:
    /// @brief Pointer to the servo driver.
    /// @warning Must outlive the current class.
    Adafruit_PWMServoDriver* _pwm;
    /// @brief The current raw PWM
    int16_t _current;
    /// @brief The channel
    uint8_t _channel;
  };

  /// @brief Type-erased view for any instantiation of LegJoint.
  /// Provides a uniform interface for disparate LegJoint template types
  /// without requiring a common base class or virtual methods in the source.
  class LegJointView
  {
  public:
    /// @brief Templated constructor that captures the specific LegJoint type.
    /// @tparam JointT The specific LegJoint template instantiation.
    /// @param joint Reference to the joint instance (must outlive this view).
    template<typename JointT>
    explicit LegJointView(JointT& joint) noexcept
        : _ptr(&joint)
        , _vtable(&vtable_for<JointT>)
    {
    }

    /// @brief Directly writes a raw PWM pulse width to the servo.
    /// @param pulse The 12-bit pulse width value, constrained by MIN and MAX.
    void write_raw(uint16_t pulse) noexcept { _vtable->write_raw(_ptr, pulse); }
    /// @brief Sets the joint position using a logical angle in millidegrees.
    /// @param millidegrees The target angle in millidegrees, mapped to the PWM range.
    void write_angle(int32_t md) noexcept { _vtable->write_angle(_ptr, md); }
    /// @brief Sets the joint to its starting position
    void write_starting() noexcept { _vtable->write_starting(_ptr); }

    /// @brief Gets the last written raw PWM pulse width.
    /// @return The 12-bit PWM value.
    uint16_t current_raw() const noexcept { return _vtable->current_raw(_ptr); }
    /// @brief Calculates the current logical angle based on the last written pulse.
    /// @return The current angle in millidegrees.
    int32_t current_angle() const noexcept { return _vtable->current_angle(_ptr); }

  private:
    struct VTable
    {
      void (*write_raw)(void*, uint16_t);
      void (*write_angle)(void*, int32_t);
      void (*write_starting)(void*);
      uint16_t (*current_raw)(const void*);
      int32_t (*current_angle)(const void*);
    };

    template<typename T>
    static constexpr VTable vtable_for = {
        [](void* p, uint16_t v) { static_cast<T*>(p)->write_raw(v); },
        [](void* p, int32_t v) { static_cast<T*>(p)->write_angle(v); },
        [](void* p) { static_cast<T*>(p)->write_starting(); },
        [](const void* p) { return static_cast<const T*>(p)->current_raw(); },
        [](const void* p) { return static_cast<const T*>(p)->current_angle(); }};

    /// @brief Pointer to the type-erased leg joint
    void* _ptr;
    /// @brief Pointer to the vtable
    const VTable* _vtable;
  };
} // namespace pluto
