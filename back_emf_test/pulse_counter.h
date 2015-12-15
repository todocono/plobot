
#include <Arduino.h>

class PulseCounter {
 private:
  unsigned long started_micros;
  int pin_;
  boolean prev_pulse_state;
  unsigned long count_;
 public:
  PulseCounter(int pin) {
    pin_ = pin;
    prev_pulse_state = (digitalRead(pin_) == HIGH);
  }
  void start() {
    count_ = 0;
    started_micros = micros();
  }
  void do_count() {
    boolean state = (digitalRead(pin_) == HIGH);
    if(state != prev_pulse_state) {
      ++count_;
      prev_pulse_state = state;
    }
  }
  unsigned long count() {
    return count_;
  }
  unsigned long micros_elapsed() {
    return micros() - started_micros;
  }
};
