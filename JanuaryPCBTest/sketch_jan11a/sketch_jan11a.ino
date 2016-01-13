
#include <Adafruit_NeoPixel.h>

#include <util/delay_basic.h>

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
  /*
  // 64khz pwm
  TCCR1A = B10000001; // Mode 5 = fast 8-bit PWM with TOP=FF
//  TCCR1B = B00001001; // ... WGM, 1:1 clock scale -> 64 kHz
  TCCR0B = TCCR0B & B11111000 | B00000001;
  TCCR1B = TCCR1B & B11111000 | B00000001;
  TCCR2B = TCCR2B & B11111000 | B00000001;
*/

  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz

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
  
  Serial.begin(9600);
}

void stop_right()
{
  digitalWrite(motor_r_fwd, LOW);
  digitalWrite(motor_r_back, LOW);
}

void stop_left()
{
  analogWrite(motor_l_fwd, 0);
  analogWrite(motor_l_back, 0);
}

void stop()
{
  stop_right();
  stop_left();
}

void move(int left, int right, int left_pn, int right_pn)
{
  stop();
  delayMicroseconds(5);
  if(left != 0) {
    if(left > 0)
      analogWrite(motor_l_fwd, left);
    else
      analogWrite(motor_l_back, -left);
  }
  if(right != 0) {
    analogWrite(motor_r_en, abs(right));
    if(right > 0)
      digitalWrite(motor_r_fwd, HIGH);
    else
      digitalWrite(motor_r_back, HIGH);
  }
  
  int steps_left = 0, steps_right = 0;
  int last_st_l = digitalRead(motor_l_pulse);
  int last_st_r = digitalRead(motor_r_pulse);
  while(steps_left < left_pn || steps_right < right_pn) 
  {/*
    static int i=0;
    if((i++) % 100 == 0) {
      Serial.print("steps_left ");
      Serial.print(steps_left);
      Serial.print(" steps_right ");
      Serial.println(steps_right);
    }*/
    int st_l = digitalRead(motor_l_pulse);
    int st_r = digitalRead(motor_r_pulse);
    if(st_l != last_st_l)
      ++steps_left;
    if(st_r != last_st_r)
      ++steps_right;
    if(steps_left >= left_pn)
      stop_left();
    if(steps_right >= right_pn)
      stop_right();
    last_st_l = st_l;
    last_st_r = st_r;
  }
}

void loop() {
  const int ls = 240, rs = 240;
  
  move(ls, rs, 1600, 1600);
  for(int i=0;i<100;++i)
    _delay_loop_2(30000);
    
    /*
  move(ls, -rs, 300, 300);
  for(int i=0;i<100;++i)
    _delay_loop_2(30000);
    */
  
}

