

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

const int led_pin = 21;

const int sd_cs = 2; // 4??

enum ActionId{
  sIdNull=0,
  sIdTurnRight,
  sIdForward,
  sIdTurnLeft,
  sIdBackward,
  sIdReset, 
  sIdGo};

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

class mover
{public:
  mover()
    : left_factor(1), right_factor(1) {
  }
  
  static int clamp_pwm(int x)
  {
    return max(175, min(255, x));
  }

  void move(int left, int right, int dur_millis)
  {
    stop();
    delayMicroseconds(5);
    if(left != 0) {
      if(left > 0)
        analogWrite(motor_l_fwd, clamp_pwm(left));
      else
        analogWrite(motor_l_back, clamp_pwm(-left));
    }
    if(right != 0) {
      analogWrite(motor_r_en, clamp_pwm(abs(right)));
      if(right > 0)
        digitalWrite(motor_r_fwd, HIGH);
      else
        digitalWrite(motor_r_back, HIGH);
    }
    
    int dur_left = dur_millis * left_factor;
    int dur_right = dur_millis * right_factor;
    
    strip.setPixelColor(0, strip.Color(128,128,128));
    strip.setPixelColor(1, strip.Color(128,128,128));
    strip.show();
    
    // TODO: Ignore short pulses at the start
    delay(75);
    
    int steps_left = 0, steps_right = 0;
    int last_st_l = digitalRead(motor_l_pulse);
    int last_st_r = digitalRead(motor_r_pulse);
    unsigned long sm = millis();
    boolean left_stopped = false, right_stopped = false;
    while(true)
    {
      int st_l = digitalRead(motor_l_pulse);
      int st_r = digitalRead(motor_r_pulse);
      if(st_l != last_st_l) {
        ++steps_left;
      }
      if(st_r != last_st_r) {
        ++steps_right;
      }
      last_st_l = st_l;
      last_st_r = st_r;
      
      const unsigned long now = millis();
      if((now - sm) > dur_left) {
        stop_left();
        left_stopped = true;
      }
      
      if((now - sm) > dur_right) {
        stop_right();
        right_stopped = true;
      }
      
      if(left_stopped && right_stopped)
        break;
    }
    Serial.print("steps_left ");
    Serial.print(steps_left);
    Serial.print(" steps_right ");
    Serial.println(steps_right);
    /*
    if(steps_left > steps_right) {
      right_factor = 1;
      left_factor = float(steps_right) / float(steps_left);
    } else {
      left_factor = 1;
      right_factor = float(steps_left) / float(steps_right);
    }
    */
    Serial.print("left_factor ");
    Serial.print(left_factor);
    Serial.print(" right_factor ");
    Serial.println(right_factor);
  }

 private:
  float left_factor, right_factor;
};


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
    default:
      return 0;
  }
} 

boolean is_special_action(unsigned action) {
  return action == sIdGo || action == sIdReset;
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
  
  static mover forward, backward, left, right;
  
  const int ls = 233, rs = 230;
  
  for(unsigned card_idx=0;card_idx<n_cards_queued;++card_idx) {
    set_both_light_colors(200,200,200);
    const unsigned card = cards_queued[card_idx];
    switch(card) {
      case sIdForward:
        forward.move(ls, rs, 800);
        break;
      case sIdBackward:
        backward.move(-ls, -rs, 800);
        break;
      case sIdTurnLeft:
//        move(-ls, rs, 300);
        left.move(-ls, rs, 335);
        break;
      case sIdTurnRight:
//        move(ls, -rs, 300);
        right.move(ls, -rs, 335);
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
    set_both_light_colors(glow_n * 255.0f, glow_n * 255.0f, glow_n * 255.0f);
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
  }
}

