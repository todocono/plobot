#include <Adafruit_NeoPixel.h>

#define PIN 21

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


const int motor_l_fwd = 14;
const int motor_l_back = 15;

const int motor_r_en = 13;

const int motor_r_fwd = PB3;
const int motor_r_back = PB5;
const int rfid_pulse = PB4;

const int spk_en_pin = 12;

const int motor_r_pulse = 30;
const int motor_l_pulse = 29;

const int antenna_off_pin = PB4;

const unsigned long sMinimumTimeBetweenPulses = 200;


// Try to filter any 32khz pwm
const unsigned long sMinimumPulseLength = 32;

#define ticks_fwd 2500
//#define ticks_turn 1440 // 1420  for D65 yellow wheel
#define ticks_turn 1880 // 1420  for blue Plobot wheels with pointy tires

const int mtr_r_speed = 240, mtr_l_speed = 240;

byte color = 0;

void setup() {
  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
  TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz
  
  
  pinMode(motor_l_fwd, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_r_fwd, OUTPUT);
  pinMode(motor_r_back, OUTPUT);
  pinMode(motor_l_back, OUTPUT);
  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);

  strip.begin();

  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.show();
  delay(1000);
  strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();
  delay(1000);

  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();
  

 // stop();
  pinMode(antenna_off_pin, OUTPUT);

  pinMode(spk_en_pin, OUTPUT);

  pinMode(rfid_pulse, OUTPUT);

  Serial.begin(57600);

  
}



class PulseCounter
{ public:
    PulseCounter(int pin)
      : pin_(pin),
        last_pulse_start_(0),
        pulses_(0), last_state_(digitalRead(pin))
    {
      //pinMode(rfid_pulse, OUTPUT);
    }

    void count() {
      const int state = digitalRead(pin_);
      //if (pin_ == motor_r_pulse)
      //   digitalWrite(rfid_pulse, state);

      if (state != last_state_) {
        ++pulses_;
        last_state_ = state;

        /*only debugging
        color = !color * 0xFF;
        if ( pin_ == motor_r_pulse) {
          strip.setPixelColor(0, strip.Color(0xFF - color, color, 0));
          strip.show();
        }
        else {
          strip.setPixelColor(1, strip.Color(0xFF - color, color, 0));
          strip.show();
        }
        */
      }


    }

    unsigned pulses()const {
      return pulses_;
    }
  private:
    const int pin_;

    boolean count_up_;
    int last_state_;
    unsigned pulses_;
    unsigned long last_pulse_start_;
    unsigned long this_pulse_start_;
};

void stop() {
  digitalWrite(motor_r_en, HIGH);
  digitalWrite(motor_r_fwd, HIGH);
  digitalWrite(motor_r_back, HIGH);
  digitalWrite(motor_l_fwd, HIGH);
  digitalWrite(motor_l_back, HIGH);
}

void coast() {
  digitalWrite(motor_r_en, LOW);
  digitalWrite(motor_r_fwd, LOW);
  digitalWrite(motor_r_back, LOW);
  digitalWrite(motor_l_fwd, LOW);
  digitalWrite(motor_l_back, LOW);
}

void loop() {
    static int t_idx = 0;
  
  boolean dir = (t_idx/2)%2 == 0;
  

  if(t_idx == 4) {
    strip.setPixelColor(0, strip.Color(255, 255, 0));
    strip.setPixelColor(1, strip.Color(255, 255, 0));
    strip.show();
    return;
  }
  
  t_idx++;
  
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();
  

  
  dir = false;
  
  coast();
  PulseCounter count_left(motor_l_pulse);
  PulseCounter count_right(motor_r_pulse);
  long left_stop_t = 0, right_stop_t = 0;
  const int pulses = dir ? ticks_fwd : ticks_turn;
  //if(dir)
    //digitalWrite(motor_r_fwd, HIGH);
  //else
  //  digitalWrite(motor_r_back, HIGH);
  
  unsigned long on_target_since_micros = 0L;
  
  boolean overshot = false;
  
  // TODO: 
  // One wheel doesn't stop when it should
  // Speed match wheels for true turn around center
  
  while (true) {
    count_left.count();
    count_right.count();

    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    
    {
      static int ii = 0;
      if((++ii) % 5000 == 0) {
        Serial.print(iclp);
        Serial.print(" ");
        Serial.println(icrp);
      }
    }
    
    
    if(abs(iclp-pulses) < 32 && abs(icrp-pulses) < 32) {
      if(on_target_since_micros == 0L) {
        on_target_since_micros = micros();
        stop();
        strip.setPixelColor(0, strip.Color(0, 255, 0));
        strip.setPixelColor(1, strip.Color(0, 255, 0));
        strip.show();
      } else if((micros() - on_target_since_micros) > 500000L) {
        break;
      }
    } else {
      if(on_target_since_micros != 0L) {
        
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.setPixelColor(1, strip.Color(0, 0, 0));
        strip.show();
        
        on_target_since_micros = 0L;
      }
      
      const int left_power = min(255, 150 + abs(iclp - pulses) / 30);
      const int right_power = min(255, 150 + abs(icrp - pulses) / 30);
      
      if(iclp < (pulses - 16) && iclp < (icrp + 8) ) {
        analogWrite(motor_l_fwd, 0);
        analogWrite(motor_l_back, left_power);
      } else if(iclp > (pulses + 32)) {
        overshot = true;
        break;
      } else {
        // Brake
        analogWrite(motor_l_back, 255);
        analogWrite(motor_l_fwd, 255);
      }
      
      if(icrp < (pulses - 16) && icrp < (iclp + 8)) {
        digitalWrite(motor_r_back, LOW);
        digitalWrite(motor_r_fwd, HIGH);
        analogWrite(motor_r_en, right_power);
      } else if(icrp > (pulses + 32)) {
        overshot = true;
        break;
      } else {
        // Brake
        digitalWrite(motor_r_fwd, HIGH);
        digitalWrite(motor_r_back, HIGH);
        analogWrite(motor_r_en, 255);
      }
    }
  }
  stop();
  
  if(!overshot) {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.setPixelColor(1, strip.Color(0, 0, 255));
  } else {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.setPixelColor(1, strip.Color(255, 0, 0));
  }
  strip.show();
  Serial.print( count_left.pulses(), DEC );
  Serial.print ( "from left and ");
  Serial.print( count_right.pulses(), DEC );
  Serial.println ( "from right");

  delay(1000);
  
}


