

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
  
  // 64khz pwm
  TCCR1A = B10000001; // Mode 5 = fast 8-bit PWM with TOP=FF
  TCCR1B = B00001001; // ... WGM, 1:1 clock scale -> 64 kHz
 
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.println("-----"); 
}

void my_tone(unsigned long for_millis, unsigned long period_micros, int volume=100) {
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
  
  for(unsigned card_idx=0;card_idx<n_cards_queued;++card_idx) {
    set_both_light_colors(200,200,200);
    const unsigned card = cards_queued[card_idx];
    switch(card) {
      case sIdForward:
        
        break;
      case sIdBackward:
        break;
      case sIdTurnLeft:
        break;
      case sIdTurnRight:
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
    my_tone(100, 750 + 750 * (0.5f + 0.5f * n), scale * 100);
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
