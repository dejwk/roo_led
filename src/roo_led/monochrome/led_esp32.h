#pragma once

#include "roo_led/monochrome/led.h"

#if defined(ESP32)

#include "driver/ledc.h"

namespace roo_led {
namespace esp32 {

static constexpr ledc_timer_bit_t kDutyRes = LEDC_TIMER_10_BIT;
static constexpr int kDuty = 1 << kDutyRes;

static constexpr int kFreq = 40000;

class GpioMonochromeLed : public MonochromeLed {
 public:
  enum Mode { ON_HIGH, ON_LOW };

  GpioMonochromeLed(int gpio_num, Mode mode = ON_LOW,
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

class BuiltinLed : public GpioMonochromeLed {
 public:
  BuiltinLed() : GpioMonochromeLed(8, ON_LOW) {}
};

// #endif

#endif

}  // namespace esp32
}  // namespace roo_led