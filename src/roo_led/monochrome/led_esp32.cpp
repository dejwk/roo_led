#include "roo_led/monochrome/led_esp32.h"
#include "roo_logging.h"
#include "esp_err.h"

#if defined(ESP32)

namespace roo_led {
namespace esp32 {

GpioMonochromeLed::GpioMonochromeLed(int gpio_num, Mode mode,
                                     ledc_timer_t timer_num,
                                     ledc_channel_t channel)
    : channel_(channel), mode_(mode) {
  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .duty_resolution = kDutyRes,
                                    .timer_num = timer_num,
                                    .freq_hz = kFreq,
                                    .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {.gpio_num = gpio_num,
                                        .speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = channel,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .timer_sel = timer_num,
                                        .duty = 0,
                                        .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  ledc_fade_func_install(0);
}

void GpioMonochromeLed::setLevel(uint16_t level) {
  LOG(INFO) << "SETTING LEVEL " << level;
  ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_, dutyForLevel(level));
  ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_);
}

void GpioMonochromeLed::fade(uint16_t target_level,
                             roo_time::Interval duration) {
  ESP_ERROR_CHECK(ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, channel_,
                                          dutyForLevel(target_level),
                                          duration.inMillis()));

  ESP_ERROR_CHECK(
      ledc_fade_start(LEDC_LOW_SPEED_MODE, channel_, LEDC_FADE_NO_WAIT));
}

int GpioMonochromeLed::dutyForLevel(uint16_t level) const {
  if (mode_ == ON_LOW) {
    level = 65535 - level;
  }
  return (uint32_t)level * kDuty / 65536;
}

}  // namespace esp32
}  // namespace roo_led

#endif