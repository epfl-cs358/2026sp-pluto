#pragma once

#include <cstdint>
#include <Arduino.h>

namespace pluto
{
  struct SensorData
  {
    volatile uint32_t start_tick = 0;
    volatile uint32_t duration   = 0;
    volatile bool data_ready     = false;
    portMUX_TYPE mux             = portMUX_INITIALIZER_UNLOCKED;
  };

  // Usual pins:
  // #define TRIG_PIN 5
  // #define ECHO_PIN 18

  /// @brief Sound speed in cm per microseconds
  static constexpr float SOUND_SPEED = 0.0343f;
  /// @brief Wait time to read from the echo pin in microseconds
  static constexpr float ULTRASONIC_WAIT_TIME = 5000.0f;
  /// @brief Range of the ultrasonic sensor in cm
  static constexpr float ULTRASONIC_RANGE =
      SOUND_SPEED * ULTRASONIC_WAIT_TIME / 2.0f;

  template<uint8_t TRIGGER_PIN, uint8_t ECHO_PIN>
  class SensorUltraSonic
  {
    static SensorData state;

    static void IRAM_ATTR handleInterrupt()
    {
      uint32_t now = micros();
      portENTER_CRITICAL_ISR(&state.mux);
      if (digitalRead(ECHO_PIN) == HIGH)
        state.start_tick = now;
      else
      {
        state.duration   = now - state.start_tick;
        state.data_ready = true;
      }
      portEXIT_CRITICAL_ISR(&state.mux);
    }

  public:
    SensorUltraSonic()                                   = default;
    SensorUltraSonic(SensorUltraSonic&&)                 = default;
    SensorUltraSonic(const SensorUltraSonic&)            = default;
    SensorUltraSonic& operator=(SensorUltraSonic&&)      = default;
    SensorUltraSonic& operator=(const SensorUltraSonic&) = default;

    /// @brief Setups the pins
    void begin() noexcept
    {
      // set the pin modes
      pinMode(TRIGGER_PIN, OUTPUT);
      pinMode(ECHO_PIN, INPUT);
      // set to low: trigger on high
      digitalWrite(TRIGGER_PIN, LOW);
      // attach interrupt
      attachInterrupt(digitalPinToInterrupt(ECHO_PIN), handleInterrupt, CHANGE);
    }

    void read_begin() const noexcept
    {
      portENTER_CRITICAL(&state.mux);
      state.data_ready = false;
      portEXIT_CRITICAL(&state.mux);

      digitalWrite(TRIGGER_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIGGER_PIN, LOW);
    }
    /// @brief Reads a distance in cm.
    /// @returns The distance in cm or a negative number if nothing is detected.
    float read_end() const noexcept
    {
      uint32_t pulse_time = 0;
      bool ready          = false;

      portENTER_CRITICAL(&state.mux);
      ready      = state.data_ready;
      pulse_time = state.duration;
      portEXIT_CRITICAL(&state.mux);

      // TODO: improve me to have a different value if not ready
      if (!ready || pulse_time == 0)
        return -1.0f;
      return (pulse_time * SOUND_SPEED) / 2.0f;
    }

    /// @brief Reads a distance in cm. 
    /// @param limit_cm Minimum allowed distance in cm. 
    /// @return true if the robot is too close to an obstacle.
    bool too_close(float limit_cm) const noexcept
    {
      const float distance_cm = read_end();

      return distance_cm > 0.0F && distance_cm < limit_cm;
    }
  };

  template<uint8_t T, uint8_t E>
  SensorData SensorUltraSonic<T, E>::state;
} // namespace pluto
