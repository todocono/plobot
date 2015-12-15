
#include "pulse_counter.h"

const int dir_a_pin = 9;
const int dir_b_pin = 10;
const int pulse_pin = 8;
const int encoder_pin = 7;

void setup() {
  Serial.begin(9600);
  Serial.println("--- Started ---");

  // 64khz pwm
  TCCR1A = B10000001; // Mode 5 = fast 8-bit PWM with TOP=FF
  TCCR1B = B00001001; // ... WGM, 1:1 clock scale -> 64 kHz
  
  pinMode(dir_a_pin, OUTPUT);
  pinMode(dir_b_pin, OUTPUT);
  pinMode(pulse_pin, INPUT);
  pinMode(encoder_pin, INPUT);
}

PulseCounter emf_counter(pulse_pin);
PulseCounter encoder_counter(encoder_pin);

void sample_and_print() {
  
//  emf_counter.start();
  //encoder_counter.start();
  
  for(unsigned long m=millis();millis() < (m+2000);) {
    emf_counter.do_count();
    encoder_counter.do_count();
  }
  
  float emf_val = emf_counter.count();
  const float encoder_val = encoder_counter.count();
  
  /*
  Serial.print("Sampled via back emf: ");
  Serial.println(emf_val);
  Serial.print("Sampled via encoder: ");
  Serial.println(encoder_val);
  Serial.print("Ratio: ");
  Serial.println(emf_val / encoder_val);
  Serial.println();
  */
  
  Serial.print(emf_val);
  Serial.print(", ");
  Serial.println(encoder_val);
  
}

void loop() {
#if 1
#if 1
  emf_counter.start();
  encoder_counter.start();
while(true){
//  for(int dir=0;dir<2;++dir)
  const int dir = 1;
  {
    for(float duty = 0.35f;duty <= 1.1f;duty += 0.05f){
      analogWrite(dir ? dir_a_pin : dir_b_pin, 
                  min(255, (unsigned byte)(duty * 255.0f)));
      //delay(600);
      sample_and_print();
    }
    analogWrite(dir_a_pin, 0);
    analogWrite(dir_b_pin, 0);
  }
  delay(500);
}
#else
  analogWrite(dir_a_pin, 250);
//  analogWrite(dir_b_pin, 180);
  
  sample_and_print();
#endif

#else
  analogWrite(dir_a_pin, 110);
  delayMicroseconds(10000);
  
  analogWrite(dir_a_pin, 0);
  delayMicroseconds(10000);
#endif

}
