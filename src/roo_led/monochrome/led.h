#pragma once

#include "stdint.h"
#include "roo_time.h"

namespace roo_led {

class MonochromeLed {
 public:
  virtual void setLevel(uint16_t level) = 0;

  void turnOn() { setLevel(65535); }
  void turnOff() { setLevel(0); }

  virtual void fade(uint16_t target_level, roo_time::Interval duration) = 0;
};

}