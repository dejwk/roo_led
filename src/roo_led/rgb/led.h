#pragma once

#include "stdint.h"
#include "roo_time.h"

#include "roo_led/rgb/color.h"

namespace roo_led {

class RgbLed {
 public:
  virtual void setColor(Color color);
};

}