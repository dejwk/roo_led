#pragma once

#include <Arduino.h>

#include <vector>

#include "roo_led/rgb/led.h"
#include "roo_logging.h"
#include "roo_scheduler.h"
#include "roo_time.h"

namespace roo_led {

class RgbStep {
 public:
  friend constexpr RgbStep RgbSetTo(Color color);
  friend constexpr RgbStep RgbHold(roo_time::Interval duration);

 private:
  friend class RgbBlinker;

  enum Type { kSet, kHold };

  constexpr RgbStep(Type type, Color color, uint16_t duration_millis);

  Type type_;
  Color color_;
  uint16_t duration_millis_;
};

constexpr RgbStep RgbSetTo(Color color);
constexpr RgbStep RgbHold(roo_time::Interval duration);

class RgbBlinker {
 public:
  RgbBlinker(RgbLed& led, roo_scheduler::Scheduler& scheduler);

  // Infinite loop of steps.
  void loop(std::vector<RgbStep> sequence);

  // Specified count of repetitions of steps.
  void repeat(std::vector<RgbStep> sequence, int repetitions);

  // Executes the sequence once.
  void execute(std::vector<RgbStep> sequence);

  // Enables the LED.
  void setColor(Color color);

  // Disables the LED.
  void turnOff();

 private:
  void updateSequence(std::vector<RgbStep> sequence, int repetitions);
  void step();

  RgbLed& led_;
  roo_scheduler::SingletonTask stepper_;
  std::vector<RgbStep> sequence_;
  int repetitions_;
  int pos_;
};

std::vector<RgbStep> RgbBlink(roo_time::Interval period, Color color,
                              int duty_percent = 50);

// Implementation details.

constexpr RgbStep::RgbStep(Type type, Color color, uint16_t duration_millis)
    : type_(type), color_(color), duration_millis_(duration_millis) {}

constexpr RgbStep RgbSetTo(Color color) {
  return RgbStep(RgbStep::kSet, color, 0);
}

constexpr RgbStep RgbTurnOff() {
  return RgbSetTo(Color());
}

constexpr RgbStep RgbHold(roo_time::Interval duration) {
  return RgbStep(RgbStep::kHold, Color(), (uint16_t)duration.inMillis());
}

}  // namespace roo_led