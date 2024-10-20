#pragma once

#include "roo_led/led.h"

#if defined(ESP32)

#include "driver/ledc.h"

namespace roo_led {
namespace esp32 {

static constexpr ledc_timer_bit_t kDutyRes = LEDC_TIMER_10_BIT;
static constexpr int kDuty = 1 << kDutyRes;

static constexpr int kFreq = 40000;

class GpioLed : public Led {
 public:
  enum Mode { ON_HIGH, ON_LOW };

  GpioLed(int gpio_num, Mode mode = ON_LOW,
          ledc_timer_t timer_num = LEDC_TIMER_0,
          ledc_channel_t channel = LEDC_CHANNEL_0);

  void setLevel(uint16_t level) override;
  void fade(uint16_t target_level, roo_time::Interval duration) override;

 private:
  int dutyForLevel(uint16_t level) const;

  ledc_channel_t channel_;
  Mode mode_;
};

// #if CONFIG_IDF_TARGET_ESP32C3

class BuiltinLed : public GpioLed {
 public:
  BuiltinLed() : GpioLed(8, ON_LOW) {}
};

// #endif

#endif

}  // namespace esp32
}  // namespace roo_led