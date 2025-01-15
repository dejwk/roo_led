#pragma once

#include <Arduino.h>

#include <vector>

#include "roo_led/led.h"
#include "roo_logging.h"
#include "roo_scheduler.h"
#include "roo_time.h"

namespace roo_led {

class Step {
 public:
  friend constexpr Step TurnOn();
  friend constexpr Step TurnOff();
  friend constexpr Step FadeOn(roo_time::Interval duration);
  friend constexpr Step FadeOff(roo_time::Interval duration);

  friend constexpr Step SetTo(uint16_t level);
  friend constexpr Step FadeTo(uint16_t level, roo_time::Interval duration);
  friend constexpr Step Hold(roo_time::Interval duration);

 private:
  friend class Blinker;

  enum Type { kSet, kHold, kFade };

  constexpr Step(Type type, uint16_t target_level, uint16_t duration_millis);

  Type type_;
  uint16_t target_level_;
  uint16_t duration_millis_;
};

constexpr Step TurnOn();
constexpr Step TurnOff();
constexpr Step SetTo(uint16_t level);
constexpr Step FadeTo(uint16_t level, roo_time::Interval duration);
constexpr Step FadeOn(roo_time::Interval duration);
constexpr Step FadeOff(roo_time::Interval duration);
constexpr Step Hold(roo_time::Interval duration);

class Blinker {
 public:
  Blinker(Led& led, roo_scheduler::Scheduler& scheduler);

  // Infinite loop of steps.
  void loop(std::vector<Step> sequence);

  // Specified count of repetitions of steps.
  void repeat(std::vector<Step> sequence, int repetitions);

  // Executes the sequence once.
  void execute(std::vector<Step> sequence);

  // Enables the LED.
  void turnOn();

  // Disables the LED.
  void turnOff();

 private:
  void updateSequence(std::vector<Step> sequence, int repetitions);
  void step();

  Led& led_;
  roo_scheduler::SingletonTask stepper_;
  std::vector<Step> sequence_;
  int repetitions_;
  int pos_;
};

std::vector<Step> Blink(roo_time::Interval period, int duty_percent = 50,
                        int rampup_percent_on = 0, int rampup_percent_off = 0);

// Implementation details.

constexpr Step::Step(Type type, uint16_t target_level, uint16_t duration_millis)
    : type_(type),
      target_level_(target_level),
      duration_millis_(duration_millis) {}

constexpr Step TurnOn() { return Step(Step::kSet, 65535, 0); }
constexpr Step TurnOff() { return Step(Step::kSet, 0, 0); }

constexpr Step SetTo(uint16_t level) { return Step(Step::kSet, level, 0); }

constexpr Step FadeTo(uint16_t level, roo_time::Interval duration) {
  return Step(Step::kFade, level, (uint16_t)duration.inMillis());
}

constexpr Step FadeOn(roo_time::Interval duration) {
  return FadeTo(65535, duration);
}

constexpr Step FadeOff(roo_time::Interval duration) {
  return FadeTo(0, duration);
}

constexpr Step Hold(roo_time::Interval duration) {
  return Step(Step::kHold, 0, (uint16_t)duration.inMillis());
}

}  // namespace roo_led