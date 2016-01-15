
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

int clamp_pwm(int x)
{
  return max(185, min(255, x));
}

const unsigned long sMinimumTimeBetweenPulses = 200;

const int rfid_pulse = PB4;

class PulseCounter
{public:
   PulseCounter(int pin)
     : pin_(pin), 
       last_pulse_start_(0),
       pulses_(0), last_state_(digitalRead(pin))
   {
     //pinMode(rfid_pulse, OUTPUT);
   }
   
   void count() {
     const int state = digitalRead(pin_);
     
     if(state == LOW && last_state_ == HIGH) {
       // Pulse start
       const unsigned long now = micros();
       if((now - last_pulse_start_) > sMinimumTimeBetweenPulses) {
         ++pulses_;
         last_pulse_start_ = now;
         last_state_ = LOW;
         /*
         if(pin_ == motor_r_pulse)
           digitalWrite(rfid_pulse, HIGH);
           */
       }
     } else if(state == HIGH && last_state_ == LOW) {
       // Pulse end
       last_state_ = HIGH;
/*
       if(pin_ == motor_r_pulse)
         digitalWrite(rfid_pulse, LOW);*/
     }
   }
   
   unsigned pulses()const {
     return pulses_;
   }
 private:
    const int pin_;
    
    int last_state_;
    unsigned pulses_;
    unsigned long last_pulse_start_;
};

static int left_power = 230, right_power = 230;
void setup_motors(int left, int right) {
  
  if(left != 0) {
    if(left > 0)
      analogWrite(motor_l_fwd, clamp_pwm(left_power));
    else
      analogWrite(motor_l_back, clamp_pwm(left_power));
  }
  if(right != 0) {
    analogWrite(motor_r_en, clamp_pwm(right_power));
    if(right > 0)
      digitalWrite(motor_r_fwd, HIGH);
    else
      digitalWrite(motor_r_back, HIGH);
  }
}

void move(int left_fwd, int right_fwd, int left_pn, int right_pn)
{
  Serial.print("left_power");
  Serial.print(left_power);
  Serial.print(" right_power ");
  Serial.println(right_power);
  
  /*
  strip.setPixelColor(0, strip.Color(32,32,32));
  strip.setPixelColor(1, strip.Color(32,32,32));
  strip.show();
  */

  stop();
  delayMicroseconds(5);
  setup_motors(left_fwd, right_fwd);
    
  const int sMaxPulseSlip = 20;
  const unsigned long started_micros = micros();
  unsigned long left_finished_micros = 0, right_finished_micros = 0;
  PulseCounter count_left(motor_l_pulse),
               count_right(motor_r_pulse);
  while(count_left.pulses() < left_pn || count_right.pulses() < right_pn)
  {
    count_left.count();
    count_right.count();
    
    /*
    const int slip = int(count_left.pulses()) - int(count_right.pulses());
    if(abs(slip) <= sMaxPulseSlip) {
      setup_motors(left, right);
    } else if(slip > 0) {
      setup_motors(180, right);
    } else {
      setup_motors(left, 180);
    }
    */
    
    boolean turn_off_left = false, turn_off_right = false;
 
    if(count_left.pulses() >= left_pn && left_finished_micros == 0) {
      left_finished_micros = micros();
      turn_off_left = true;
    }
    if(count_right.pulses() >= right_pn && right_finished_micros == 0) {
      right_finished_micros = micros();
      turn_off_right = true;
    }
    
    if(count_right.pulses() >= right_pn && count_left.pulses() >= left_pn) {
      stop_left();
      stop_right();
    }
    /*
    if(turn_off_left) {
      strip.setPixelColor(1, strip.Color(0,0,0));
      strip.show();
    }
    
    if(turn_off_right) {
      strip.setPixelColor(0, strip.Color(0,0,0));
      strip.show();
    }
    */
  }
  
  Serial.print("left off pulses ");
  Serial.println(count_left.pulses());
  Serial.print("right off pulses ");
  Serial.println(count_right.pulses());

  Serial.print("left off millis ");
  Serial.println(left_finished_micros / 1000L);
  Serial.print("right off millis ");
  Serial.println(right_finished_micros / 1000L);
  
  const unsigned long sOffsetEpsilon = 1000L;
  const long offset = long(left_finished_micros) - long(right_finished_micros);
  Serial.print("offset ");
  Serial.println(offset);
  if(offset > 0) {
    const unsigned long offset = left_finished_micros - right_finished_micros;
    
    --right_power;
    ++left_power;
    
  } else {
    
    ++right_power;
    --left_power;
    
  }
}


void loop() {
  
  move(1, 1, 1600, 1600);
  delay(500);
}

