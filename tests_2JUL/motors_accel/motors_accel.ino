
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include<SPIFlash.h>
#include<SPI.h>
#include <PID_v1.h>

#include "Vec2f.h"

#define cs 18
#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above

SPIFlash flash(cs);

const int motor_l_pulse = 28;
const int motor_r_pulse = 27;

const int motor_l_en = 13;
const int motor_l_dir = 24;

const int motor_r_en = 12;
const int motor_r_dir = 25;

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

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
    int pin_;
    
    int last_state_;
    int pulses_;
};

PulseCounter count_left(motor_l_pulse);
PulseCounter count_right(motor_r_pulse);

ISR (PCINT0_vect)
{
  noInterrupts();
  count_left.count();
  count_right.count();
  interrupts();
}

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}


void setup() {
  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);
  
  pciSetup(motor_l_pulse);
  pciSetup(motor_r_pulse);
  
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  pinMode(RFID_NSS_PIN, OUTPUT);
  digitalWrite(RFID_NSS_PIN, HIGH);
  
  Serial.begin(57600);
  
  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
 
  analogWrite(motor_l_en, 0);
  analogWrite(motor_r_en, 0);
  
}

void loop() {  
  count_left = PulseCounter(motor_l_pulse);
  count_right = PulseCounter(motor_r_pulse);
  
  double pid_setpoint = 400, pid_output = 0, pid_input = 0;
  
  const double Kp = .15, Ki = .9, Kd = .1;
  PID pulsesPID(&pid_input, &pid_output, &pid_setpoint,Kp,Ki,Kd, DIRECT);
  
  pulsesPID.SetMode(AUTOMATIC);
  pulsesPID.SetOutputLimits(-250, 250);
  
  // Wait for the ticks to stop. 
  int last_left = count_left.pulses(), last_right = count_right.pulses();
  unsigned long last_count_millis = millis();
  
  // TODO: Timeout
  while((millis() - last_count_millis) < 200) {
    if(count_left.pulses() != last_left || count_right.pulses() != last_right) {
      last_left = count_left.pulses();
      last_right = count_right.pulses();
      last_count_millis = millis();
    }
    
    /*
    Serial.print("input ");
    Serial.print(pid_input);
    Serial.print(" output ");
    Serial.print(pid_output);
    Serial.print(" set ");
    Serial.print(pid_setpoint);
    Serial.println();
    */
    
    pid_input = (count_left.pulses() + count_right.pulses()) / 2;
    pulsesPID.Compute();
    
    // Max should be <255 for bootstrap
    int mtr_pwr = max(0, min(250, abs(pid_output)));
    digitalWrite(motor_l_dir, (pid_output < 0) ? HIGH : LOW);
    digitalWrite(motor_r_dir, (pid_output < 0) ? LOW : HIGH);

    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    if(abs(int(iclp) - int(icrp)) > 5) {
      if(iclp > icrp) {
        analogWrite(motor_r_en, 0);   
        analogWrite(motor_l_en, mtr_pwr);
      } else {
        analogWrite(motor_r_en, mtr_pwr);   
        analogWrite(motor_l_en, 0);
      }
    } else {
      analogWrite(motor_r_en, mtr_pwr);   
      analogWrite(motor_l_en, mtr_pwr);   
    }
  }
  
  analogWrite(motor_r_en, 0);
  analogWrite(motor_l_en, 0);
  
  delay(750);
}
