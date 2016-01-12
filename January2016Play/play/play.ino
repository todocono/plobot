

#include <Adafruit_NeoPixel.h>

const int spk_en_pin = 12;
const int spk_p_pin = PB6;
const int spk_n_pin = PB7;

const int motor_l_fwd = 14;
const int motor_l_back = 15;

const int motor_r_en = 13;
const int motor_r_fwd = PB3;
const int motor_r_back = PB5;

const int motor_r_pulse = 27;
const int motor_l_pulse = 28;

const int led_pin = 21;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, led_pin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(motor_l_fwd, OUTPUT);
  pinMode(motor_l_back, OUTPUT);

  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_r_fwd, OUTPUT);
  pinMode(motor_r_back, OUTPUT);

  pinMode(motor_r_pulse, INPUT);
  pinMode(motor_l_pulse, INPUT);

  pinMode(spk_en_pin, OUTPUT);
  pinMode(spk_p_pin, OUTPUT);
  pinMode(spk_n_pin, OUTPUT);

  analogWrite(spk_en_pin, 255);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // put your main code here, to run repeatedly:

}
