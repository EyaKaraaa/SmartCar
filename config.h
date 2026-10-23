#pragma once
#include <Arduino.h>

// Proposed wiring for ESP32-WROOM-32 DevKit WITHOUT PSRAM.
// These are not GPIO assignments recovered from the original prototype.
struct MotorPins { uint8_t in1; uint8_t in2; bool inverted; };
constexpr MotorPins LEFT_FRONT  = {13, 14, false};
constexpr MotorPins LEFT_REAR   = {16, 17, false};
constexpr MotorPins RIGHT_FRONT = {18, 19, false};
constexpr MotorPins RIGHT_REAR  = {21, 22, false};
constexpr uint8_t TRIG_PIN = 25;
constexpr uint8_t ECHO_PIN = 34; // Input only; voltage divider REQUIRED.
constexpr uint8_t AUTO_LED_PIN = 26; // Optional LED + 330 ohm series resistor.
constexpr char BT_NAME[] = "SMART_CAR";
constexpr float OBSTACLE_CM = 25.0f;
constexpr float MIN_DISTANCE_CM = 2.0f;
constexpr float MAX_DISTANCE_CM = 400.0f;
constexpr uint32_t SONAR_INTERVAL_MS = 80;
constexpr uint32_t SONAR_STALE_MS = 300;
constexpr uint32_t ECHO_TIMEOUT_US = 25000;
constexpr uint32_t LINK_TIMEOUT_MS = 1500;
constexpr uint32_t MANUAL_TIMEOUT_MS = 700;
constexpr uint32_t STOP_BEFORE_REVERSE_MS = 200;
constexpr uint32_t REVERSE_MS = 300;
constexpr uint32_t STOP_BEFORE_TURN_MS = 150;
constexpr uint32_t TURN_MS = 450;
constexpr uint32_t STOP_AFTER_TURN_MS = 150;
