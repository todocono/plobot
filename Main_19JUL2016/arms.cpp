
#include "motors.h"
#include <Arduino.h>

const int pin_left_servo = 3;
const int pin_right_servo = 4;

void init_arms() {
  pinMode(pin_left_servo, OUTPUT);
  pinMode(pin_right_servo, OUTPUT);
}

// pos is -50 to 50
// duration is in millis
void set_arms(int both_pos, int duration) {
  const unsigned long left_pulse_duration = 1000L + 10L * max(0, min(100, 50 + both_pos));
  const unsigned long right_pulse_duration = 1000L + 10L * max(0, min(100, 50 - both_pos));
  const unsigned long left_dead_time = 10000L - left_pulse_duration;
  const unsigned long right_dead_time = 10000L - right_pulse_duration;
  for(int cycles = max(2, duration / 10);cycles;--cycles) {
    if(cycles % 2 == 0) {
      digitalWrite(pin_left_servo, HIGH);
      delayMicroseconds(left_pulse_duration);
      digitalWrite(pin_left_servo, LOW);
      delayMicroseconds(left_dead_time);
    } else {
      digitalWrite(pin_right_servo, HIGH);
      delayMicroseconds(right_pulse_duration);
      digitalWrite(pin_right_servo, LOW);
      delayMicroseconds(right_dead_time);
    }
  }
}


