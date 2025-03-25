#include "roo_led/rgb/blinker.h"

#include "roo_led.h"
#include "roo_logging.h"

using namespace roo_time;

namespace roo_led {

RgbBlinker::RgbBlinker(RgbLed& led, roo_scheduler::Scheduler& scheduler)
    : led_(led),
      stepper_(scheduler, [this]() { step(); }),
      sequence_(),
      pos_(0) {}

void RgbBlinker::loop(std::vector<RgbStep> sequence) {
  updateSequence(std::move(sequence), -1);
}

void RgbBlinker::repeat(std::vector<RgbStep> sequence, int repetitions) {
  updateSequence(std::move(sequence), repetitions - 1);
}

void RgbBlinker::execute(std::vector<RgbStep> sequence) {
  updateSequence(std::move(sequence), 0);
}

void RgbBlinker::setColor(Color color) {
  updateSequence({}, 0);
  led_.setColor(color);
}

void RgbBlinker::turnOff() {
  updateSequence({}, 0);
  led_.setColor(Color());
}

void RgbBlinker::updateSequence(std::vector<RgbStep> sequence,
                                int repetitions) {
  sequence_ = std::move(sequence);
  repetitions_ = repetitions;
  pos_ = 0;
  if (!sequence_.empty() && !stepper_.is_scheduled()) {
    stepper_.scheduleNow(roo_scheduler::PRIORITY_ELEVATED);
  }
}

void RgbBlinker::step() {
  uint16_t next_delay = 0;
  do {
    if (pos_ >= sequence_.size()) {
      sequence_.clear();
      return;
    }
    const RgbStep& s = sequence_[pos_];
    switch (s.type_) {
      case RgbStep::kSet: {
        led_.setColor(s.color_);
        break;
      }
      case RgbStep::kHold:
      default: {
        next_delay = s.duration_millis_;
        break;
      }
        //   case RgbStep::kFade:
        //   default: {
        //     led_.fade(s.target_level_, roo_time::Millis(s.duration_millis_));
        //     next_delay = s.duration_millis_;
        //   }
    }
    ++pos_;
    if (pos_ == sequence_.size() && repetitions_ != 0) {
      if (repetitions_ > 0) --repetitions_;
      pos_ = 0;
    }
  } while (next_delay == 0);
  stepper_.scheduleAfter(roo_time::Millis(next_delay),
                         roo_scheduler::PRIORITY_ELEVATED);
}

std::vector<RgbStep> RgbBlink(roo_time::Interval period, Color color,
                              int duty_percent) {
  CHECK_GE(duty_percent, 0);
  CHECK_LE(duty_percent, 100);
  int millis = period.inMillis();
  int millis_1st = duty_percent * millis / 100;
  int millis_2nd = millis - millis_1st;

  std::vector<RgbStep> result;
  result.reserve(4);

  result.push_back(RgbSetTo(color));
  result.push_back(RgbHold(Millis(millis_1st)));

  result.push_back(RgbTurnOff());
  result.push_back(RgbHold(Millis(millis_2nd)));
  return result;
}

}  // namespace roo_led
