

#include "motors.h"
#include <Arduino.h>

const int motor_l_pulse = 28;
const int motor_r_pulse = 27;

const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;


void init_motors() {
  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);
  
  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
}

class PulseCounter
{public:
   PulseCounter(int pin)
     : pin_(pin), 
       pulses_(0), last_state_(digitalRead(pin))
   {
   }
   
   void count() {
     const unsigned long now = micros();
     const int state = digitalRead(pin_);
     
     if(state == LOW && last_state_ == HIGH) {
       // Pulse start
       last_state_ = LOW;
     } else if(state == HIGH && last_state_ == LOW) {
       // Pulse end
       last_state_ = HIGH;

       ++pulses_;
     }
   }
   
   int pulses()const {
     return pulses_;
   }
 private:
    const int pin_;
    
    int last_state_;
    int pulses_;
};

void do_move(const int l_sign, const int r_sign, const int pulses)
{
//  coast();
  
  const int min_power = 80, max_power = 140;
  const unsigned long sm = micros();
  PulseCounter count_left(motor_l_pulse);
  PulseCounter count_right(motor_r_pulse);

  if(l_sign >= 0) {
    digitalWrite(motor_l_dir, HIGH);
  } else {
    digitalWrite(motor_l_dir, LOW);
  }
  if(r_sign >= 0)
    digitalWrite(motor_r_dir, HIGH);
  else
    digitalWrite(motor_r_dir, LOW);

  while(count_left.pulses() < pulses || count_right.pulses() < pulses) {
    count_left.count();
    count_right.count();
    
    const int avg_pulses = (count_left.pulses() + count_right.pulses()) / 2;
    const int triangle = (avg_pulses > pulses / 2) ? (pulses - avg_pulses) : avg_pulses;

    const int mtr_speed = max(min_power, min(max_power, triangle));

    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    
    if(abs(int(iclp) - int(icrp)) > 1) {
      if(iclp > icrp) {
        analogWrite(motor_r_en, min_power);   
        analogWrite(motor_l_en, mtr_speed);
      } else {
        analogWrite(motor_r_en, mtr_speed);   
        analogWrite(motor_l_en, min_power);
      }
      
    } else {
      analogWrite(motor_r_en, mtr_speed);   
      analogWrite(motor_l_en, mtr_speed);   
    } 
  }
  analogWrite(motor_r_en, 0);
  analogWrite(motor_l_en, 0);

  // Wait for the ticks to stop. 
  int last_left = count_left.pulses(), last_right = count_right.pulses();
  unsigned long last_count_millis = millis();
  
  while((millis() - last_count_millis) < 200) {
    if(count_left.pulses() != last_left || count_right.pulses() != last_right) {
      last_left = count_left.pulses();
      last_right = count_right.pulses();
      last_count_millis = millis();
    }
  }
  
  // Count overshoot. 
  Serial.print("Final counts: left(");
  Serial.print(count_left.pulses());
  Serial.print(") right (");
  Serial.print(count_right.pulses());
  Serial.print(")");
  Serial.println();
  
  // TODO: Use this value
}



