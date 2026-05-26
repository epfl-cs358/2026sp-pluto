#pragma once
#include <Adafruit_PWMServoDriver.h>

/// @brief A mock implementation of Adafruit_PWMServoDriver that does nothing, for use in the simulation.
class MockPWMServoDriver : public Adafruit_PWMServoDriver {
public:
    MockPWMServoDriver() : Adafruit_PWMServoDriver() {}

    bool begin(uint8_t prescale = 0) override {return true;}
    void reset() override {}

    void setPWMFreq(float freq) override {
        // Do nothing
        (void)freq;
    }
    
    void setPWM(uint8_t num, uint16_t on, uint16_t off) override {
        // Do nothing
        (void)num;
        (void)on;
        (void)off;
    }
};