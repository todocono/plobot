

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#include <SPI.h>
#include <SD.h>

const int rxd = 23;
const int txd = 22;

SoftwareSerial rfid_serial(rxd, txd); // RX, TX

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

const int antenna_off_pin = PB4;

const int led_pin = 21;

const int sd_cs = 2; // 4??

float glow_r = 255, glow_g = 255, glow_b = 255;

enum ActionId{
  sIdNull=0,
  sIdTurnRight,
  sIdForward,
  sIdTurnLeft,
  sIdBackward,
  sIdReset, 
  sIdGo,
  sColorOrange,
  sColorRed,
  sColorGreen,
  sColorPurple,
  sColorBlue,
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, led_pin, NEO_GRB + NEO_KHZ800);

const unsigned max_cards = 64;
unsigned n_cards_queued = 0;
unsigned cards_queued[max_cards];

void setup() {
  Serial.begin(9600);
  rfid_serial.begin(9600);

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
  
  pinMode(antenna_off_pin, OUTPUT);

  pinMode(sd_cs, OUTPUT);
  
  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
  TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz
 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.println("-----"); 
}

#define DEFAULT_VOLUME 75

void my_tone(unsigned long for_millis, unsigned long period_micros, int volume=DEFAULT_VOLUME) {
  analogWrite(spk_en_pin, volume);
  for(const unsigned long sm=millis();(millis() - sm) < for_millis;) {
    digitalWrite(spk_p_pin, HIGH);
    delayMicroseconds(period_micros / 2);
    digitalWrite(spk_p_pin, LOW);  
    delayMicroseconds(10);

    digitalWrite(spk_n_pin, HIGH);
    delayMicroseconds(period_micros / 2);
    digitalWrite(spk_n_pin, LOW);
    delayMicroseconds(10);
  }
}

void stop_right()
{
  digitalWrite(motor_r_fwd, LOW);
  digitalWrite(motor_r_back, LOW);
  analogWrite(motor_r_en, 0);
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


const unsigned long sMinimumTimeBetweenPulses = 200;

// Try to filter any 32khz pwm
const unsigned long sMinimumPulseLength = 32;

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
         last_state_ = LOW;
         this_pulse_start_ = now;
         /*
         if(pin_ == motor_r_pulse)
           digitalWrite(rfid_pulse, HIGH);
           */
       }
     } else if(state == HIGH && last_state_ == LOW) {
       // Pulse end
       last_state_ = HIGH;

       const unsigned long now = micros();
       if((now - this_pulse_start_) > sMinimumPulseLength) {
         ++pulses_;
         last_pulse_start_ = this_pulse_start_;
       }
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
    unsigned long this_pulse_start_;
};

const int mtr_r_speed = 255, mtr_l_speed = 240;


void set_both_light_colors(uint8_t r, uint8_t g, uint8_t b) {
  const uint32_t c = strip.Color(r,g,b);
  strip.setPixelColor(0, c);
  strip.setPixelColor(1, c);
  strip.show();
}

void show_error() {
  set_both_light_colors(255, 0, 0);
  my_tone(500, 5000);
}

unsigned get_action_for_card(uint32_t card)
{
  switch(card) {
    case 144449024:
    case 1262626304:
      return sIdTurnRight;
    case 2475845120:
    case 1447175680:
      return sIdForward;
    case 583624192:
    case 1413621248:
      return sIdTurnLeft;
    case 2917033472:
    case 1514284544:
      return sIdBackward;
    case 2455276800:
    case 2772771584:
      return sIdReset;  
    case 2402900224:
    case 1982144768:
      return sIdGo;   
    // Colors
    case 1068768768:
      return sColorOrange;
    case 3470007808:
      return sColorRed;
    case 4027457792:
      return sColorGreen;
    case 63118848:
      return sColorPurple;
    case 3154059264:
      return sColorBlue;
    default:
      return 0;
  }
} 

boolean is_special_action(unsigned action) {
  return action == sIdGo || action == sIdReset ||
          action == sColorOrange ||
          action == sColorRed ||
          action == sColorGreen ||
          action == sColorPurple ||
          action == sColorBlue;
}

void do_move(const int l_sign, const int r_sign, const int pulses)
{
  digitalWrite(antenna_off_pin, HIGH);
  
  const int mtr_r_speed = 255, mtr_l_speed = 240;
  const unsigned long sm = micros();
  PulseCounter count_left(motor_l_pulse);
  PulseCounter count_right(motor_r_pulse);

  long left_stop_t = 0, right_stop_t = 0;

  if(r_sign >= 0)
    digitalWrite(motor_r_fwd, HIGH);
  else
    digitalWrite(motor_r_back, HIGH);
  
  const int avg_speed = (mtr_r_speed + mtr_l_speed) / 2;

  analogWrite(motor_r_en, avg_speed);
  
  if(l_sign >= 0)
    analogWrite(motor_l_fwd, avg_speed);
  else
    analogWrite(motor_l_back, avg_speed);
  
  // Noise at signal start
 //delay(100);
 delay(50);

  Serial.println("-----");
  while(count_left.pulses() < pulses || count_right.pulses() < pulses) {
    count_left.count();
    count_right.count();
    const int iclp = count_left.pulses();
    const int icrp = count_right.pulses();
    
    if(abs(int(iclp) - int(icrp)) > 1) {
      if(iclp > icrp) {
        analogWrite(motor_r_en, mtr_r_speed);  
        if(l_sign >= 0)
          analogWrite(motor_l_fwd, 170);
        else
          analogWrite(motor_l_back, 170);
        
      } else {
        analogWrite(motor_r_en, 170);  
        if(l_sign >= 0)
          analogWrite(motor_l_fwd, mtr_l_speed);
        else
          analogWrite(motor_l_back, mtr_l_speed);
      }
      
    } else {
      analogWrite(motor_r_en, mtr_r_speed);   
      if(l_sign >= 0)
        analogWrite(motor_l_fwd, mtr_l_speed); 
      else
        analogWrite(motor_l_back, mtr_l_speed);
    }
    
  }
  stop();
  digitalWrite(antenna_off_pin, LOW);
}

void do_go()
{ 
  if(n_cards_queued == 0) {
    set_both_light_colors(255,0,0);
    my_tone(750, 6000);
    return;
  }
  
  set_both_light_colors(64,64,0);
  my_tone(300, 1250);
  set_both_light_colors(0,0,0);
  delay(200);
  set_both_light_colors(255,255,0);
  my_tone(300, 1250);
  set_both_light_colors(0,0,0);
  delay(200);
  set_both_light_colors(0,255,0);
  my_tone(300, 800);
  set_both_light_colors(0,0,0);
  
  const int straight_ticks = 340;  
  const int turn_ticks = 125;
// R35 
//  const int straight_ticks = 300;
//  const int turn_ticks = 115;
  
  for(unsigned card_idx=0;card_idx<n_cards_queued;++card_idx) {
    set_both_light_colors(200,200,200);
    const unsigned card = cards_queued[card_idx];
    switch(card) {
      case sIdForward:
        do_move(1,1,straight_ticks);
        break;
      case sIdBackward:
        do_move(-1,-1,straight_ticks);
        break;
      case sIdTurnLeft:
        do_move(-1,1,turn_ticks);
        break;
      case sIdTurnRight:
        do_move(1,-1,turn_ticks);
        break;
    }
    set_both_light_colors(0,0,0);
    delay(300);
  }
}

void do_reset()
{
  n_cards_queued = 0;
  const float tmax = 2.75f;
  for(float t=0;t<tmax;t+=0.1f) {
    const float n = sin(t * 2 * M_PI);
    const float scale = 1.0f - (t / tmax);
    my_tone(100, 750 + 750 * (0.5f + 0.5f * n), scale * DEFAULT_VOLUME);
    set_both_light_colors(max(0, n*scale*255),0,max(0, -n*scale*255));
  }
}

void loop() {
  
  // Glow
  static const unsigned long glow_period_micros = 2000000L;
  static unsigned long last_glow_micros = micros();
  static unsigned long glow_t_micros = 0;
  
  // Limit glow frequency so we don't interfere with RFID antenna
  {
    const unsigned long m = micros();
    const unsigned long glow_t_micros_offset = m - last_glow_micros;
    glow_t_micros += glow_t_micros_offset;
    last_glow_micros = m;
    float glow_t = float(glow_t_micros) / glow_period_micros;

    float glow_n = 0.5f + 0.5f*sin(glow_t * 2.0f * M_PI);
    set_both_light_colors(glow_n * glow_r, glow_n * glow_g, glow_n * glow_b);
  }
  
  // "Blink" the antenna when waiting for commands, to avoid interfering with
  // nearby Plobots
  {
    static unsigned long last_antenna_millis = millis();
    const unsigned long now = millis();
    static boolean antenna_mode = false;
    if((now - last_antenna_millis) >= 225) {
       last_antenna_millis = now;
       antenna_mode = (random(3) != 0);
       digitalWrite(antenna_off_pin, antenna_mode ? HIGH : LOW);
    }
    if((now - last_antenna_millis) > 75) {
       digitalWrite(antenna_off_pin, antenna_mode ? LOW : HIGH);
    }
  }

  // Card scanning
  if (rfid_serial.available()) {
    uint32_t card_id = 0;
    if(4 == rfid_serial.readBytes((char*)&card_id, 4)) {
      Serial.print("card: ");
      Serial.println(card_id);
      const int action_id = get_action_for_card(card_id);
      if(!action_id)
      {
        // Ignore
        // TODO: Figure out why this happens
        /*
        show_error();
        delay(100);
        show_error();
        */
      }
      else
      {
        Serial.print("action: ");
        Serial.println(action_id);
        
        if(is_special_action(action_id)) {
          switch(action_id) {
            case sIdGo:
              do_go();
              break;
            case sIdReset:
              do_reset();
              break;
            case sColorOrange:
              glow_r = 255.0f;
              glow_g = 165.0f;
              glow_b = 0.0f;
              break;
            case sColorRed:
              glow_r = 255.0f;
              glow_g = 0.0f;
              glow_b = 0.0f;
              break;
            case sColorGreen:
              glow_r = 0.0f;
              glow_g = 255.0f;
              glow_b = 0.0f;
              break;
            case sColorPurple:
              glow_r = 128.0f;
              glow_g = 0.0f;
              glow_b = 128.0f;
              break;
            case sColorBlue:
              glow_r = 0.0f;
              glow_g = 0.0f;
              glow_b = 255.0f;
              break;
          }
        } else {
          if(n_cards_queued == max_cards) {
            show_error();
          }
          else
          {
            // Record card
            cards_queued[n_cards_queued++] = action_id;
  
            // Indicate success
            for(int i=0;i<4;++i) {
              my_tone(100, 2500 - 500 * i);
              set_both_light_colors(0,25 + 75 * i,0);
            }
          }
        }
      }
    }
    // One card at a time
    while(rfid_serial.available())
      rfid_serial.read();
  }
}

