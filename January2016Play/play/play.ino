

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

const int mic_pin = A0;
boolean dance_mode = false;

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
  sPlayDo,
  sPlayRe,
  sPlayMi,
  sPlayFa,
  sPlaySol,
  sPlayLa,
  sPlaySi,
};

const int sNotePeriods[] = {
  3830,
  3400,
  3038,
  2864,
  2550,
  2272,
  2028,
};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, led_pin, NEO_GRB + NEO_KHZ800);

const uint32_t sNoteColors[] = {
  strip.Color(255,0,0),
  strip.Color(255,128,0),
  strip.Color(255,255,0),
  strip.Color(0,255,0),
  strip.Color(0,204,204),
  strip.Color(127,0,255),
  strip.Color(255,0,127),
  
};

const unsigned max_cards = 256;
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
  
  pinMode(mic_pin, INPUT);

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


void set_both_light_colors(uint32_t c) {
  strip.setPixelColor(0, c);
  strip.setPixelColor(1, c);
  strip.show();
}

void set_both_light_colors(uint8_t r, uint8_t g, uint8_t b) {
  const uint32_t c = strip.Color(r,g,b);
  set_both_light_colors(c);
}

void show_error() {
  set_both_light_colors(255, 0, 0);
  my_tone(500, 5000);
}

void set_move(const int l_sign, const int r_sign)
{
  coast();
  if(r_sign >= 0)
    digitalWrite(motor_r_fwd, HIGH);
  else
    digitalWrite(motor_r_back, HIGH);

  analogWrite(motor_r_en, 255);
  
  if(l_sign >= 0)
    analogWrite(motor_l_fwd, 255);
  else
    analogWrite(motor_l_back, 255);
}

boolean is_movement_action(unsigned action)
{
  return action == sIdForward ||
         action == sIdBackward ||
         action == sIdTurnLeft ||
         action == sIdTurnRight;
}

void play_note(unsigned action_id, unsigned movement_action = sIdNull) {
  action_id = max(sPlayDo, min(sPlaySi, action_id));
  const unsigned long period = sNotePeriods[action_id - sPlayDo];
  set_both_light_colors(sNoteColors[action_id - sPlayDo]);

  my_tone(100, period);
  if(movement_action != sIdNull) {
    switch(movement_action) {
      case sIdForward:
        set_move(1,1);
        break;
      case sIdBackward:
        set_move(-1,-1);
        break;
      case sIdTurnLeft:
        set_move(-1,1);
        break;
      case sIdTurnRight:
        set_move(1,-1);
        break;
    }
  }
  my_tone(100, period);
  if(movement_action != sIdNull) {
    stop();
  }
  my_tone(100, period);
}

unsigned get_action_for_card(uint32_t card)
{
  switch(card) {
    case 16522752:
    case 1986208512:
    case 3349243392:
    case 246244864:
    case 246248192:
    case 3468959232:
    case 2054252032:
    case 2054238976:
    case 1208098560:
    case 1408704512:
      return sPlayDo;
    case 131276288:
    case 596076032:
    case 545744384:
    case 127279872:
    case 1718820608:
    case 2364349952:
    case 1113858816:
    case 45704704:
    case 45704832:
    case 4175633920:
    case 364846592:
      return sPlayRe;
    case 1128472832:
    case 2929925632:
    case 516038144:
    case 516034304:
    case 1438719488:
    case 3740475008:
    case 3740474880:
    case 1400792576:
    case 81403392:
    case 1040654080:
      return sPlayMi;
    case 1101602304:
    case 111484416:
    case 1467377152:
    case 329457152:
    case 502472192:
    case 502472320:
    case 3539541504:
    case 3505987072:
    case 1692081664:
    case 1708858880:
    case 2348424704:
      return sPlayFa;
    case 817372672:
    case 3303087616:
    case 3336642048:
    case 3099139584:
    case 2340561664:
    case 3621807616:
    case 3621807872:
    case 2071553536:
    case 2071553792:
    case 3415500288:
    case 720445440:
      return sPlaySol;
    case 3164806656:
    case 3083432448:
    case 881601024:
    case 1175480576:
    case 2971101440:
    case 1181950720:
    case 1212817792:
    case 1212817664:
    case 652513024:
    case 652484096:
    case 1851138048:
    case 1851138304:
      return sPlayLa;
    case 3574144768:
    case 2251563264:
    case 3876003584:
    case 3859226368:
    case 1057431296:
    case 2533760768:
    case 1155163648:
    case 1188718080:
    case 3086753280:
    case 839458688:
    case 839458560:
      return sPlaySi;
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
    case 1053498880:
      return sColorRed;
    case 4027457792:
      return sColorGreen;
    case 63118848:
    case 14489344:
    case 2885754624:
      return sColorPurple;
    case 3154059264:
    case 181149184:
      return sColorBlue;
    default:
      return 0;
  }
} 

boolean in_red_mode() {
  return glow_r == 255.0f &&
         glow_g == 0.0f &&
         glow_b == 0.0f;
}

boolean is_note(unsigned action) {
  return action >= sPlayDo && action <= sPlaySi;
}

boolean is_special_action(unsigned action) {
  if(in_red_mode() && is_note(action))
    return true;
  
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
  
  coast();
  
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
          analogWrite(motor_l_fwd, 0);
        else
          analogWrite(motor_l_back, 0);
        
      } else {
        analogWrite(motor_r_en, 0);  
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
  
//  const int straight_ticks = 340;  
//  const int turn_ticks = 138;
// R35 
  const int straight_ticks = 400;
  const int turn_ticks = 150;
  
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
      default:
      {
        if(is_note(card)) {
          int next_movement_action = sIdNull;
          if(card_idx < (n_cards_queued-1) && is_movement_action(cards_queued[card_idx+1])) {
            next_movement_action = cards_queued[++card_idx];
          }
          play_note(card, next_movement_action);
        }
        break;
      }
    }
    set_both_light_colors(0,0,0);
    //delay(300);
    
    // Ignore cards queued during action
    while(rfid_serial.available())
      rfid_serial.read();
  
    const unsigned long pause_millis = dance_mode ? 100 : 300;
  
    for(const unsigned long smillis = millis();(millis() - smillis) < pause_millis;) {
      if(rfid_serial.available()) {
        uint32_t card_id = 0;
        if(4 == rfid_serial.readBytes((char*)&card_id, 4)) {
          Serial.print("card between actions: ");
          Serial.println(card_id);
          const int action_id = get_action_for_card(card_id);
          if(action_id == sIdReset) {
            do_reset();
            goto finished_actions;
          }
        }
      }
    }
  }
 finished_actions:
  // Ignore cards queued during action
  while(rfid_serial.available())
    rfid_serial.read();
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
  if(!in_red_mode())
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
  
  // Listen for clap
  if(dance_mode) {
    if(analogRead(mic_pin) > 1000) {
      do_go();
      return;
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
              dance_mode = false;
              glow_r = 255.0f;
              glow_g = 165.0f;
              glow_b = 0.0f;
              break;
            case sColorRed:
              digitalWrite(antenna_off_pin, LOW);
              dance_mode = false;
              glow_r = 255.0f;
              glow_g = 0.0f;
              glow_b = 0.0f;
              break;
            case sColorGreen:
              dance_mode = true;
              glow_r = 0.0f;
              glow_g = 255.0f;
              glow_b = 0.0f;
              break;
            case sColorPurple:
              dance_mode = true;
              glow_r = 128.0f;
              glow_g = 0.0f;
              glow_b = 128.0f;
              break;
            case sColorBlue:
              dance_mode = true;
              glow_r = 0.0f;
              glow_g = 0.0f;
              glow_b = 255.0f;
              break;
            default:
              if(is_note(action_id)) {
                play_note(action_id);
              }
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
  
            if(is_note(action_id)) {
              play_note(action_id);
            } else {
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
    // One card at a time
    while(rfid_serial.available())
      rfid_serial.read();
  }
}

