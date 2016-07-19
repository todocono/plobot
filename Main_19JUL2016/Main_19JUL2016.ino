#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>

#include <Servo.h> 
#include <SPI.h>

#include "cards.h"
#include "lights.h"
#include "sound.h"
#include "jingles.h"
#include "motors.h"

const int ultra_power = 22;
const int trigPin = 19;
const int echoPin = 20;

int servos[2] = {3,4};

IdleGlow idle_glow(2000000L, 255, 255, 255);
const unsigned max_cards = 200;
unsigned n_cards_queued = 0;
CardId cards_queued[max_cards];

const int pause_millis = 300;

const int straight_ticks = 600;
const int turn_ticks = 650;

void setup() {
  
  init_sound();
  init_motors();
  
  pinMode(ultra_power, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(ultra_power, HIGH);
  
  pinMode(servos[0], OUTPUT);
  pinMode(servos[1], OUTPUT);
  
  TCCR1B = 0x01;   // Timer 1: PWM 9 & 10 @ 32 kHz
  TCCR2B = 0x01;   // Timer 2: PWM 3 & 11 @ 32 kHz
  
  init_lights();
  
  Serial.begin(57600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  
  init_cards();
}

void do_reset() {
    n_cards_queued = 0;
    reset_jingle();
}

void do_go() {
  if(n_cards_queued == 0) {
    error_jingle();
    return;
  }
  
  go_jingle();
  
  for(unsigned card_idx=0;card_idx<n_cards_queued;++card_idx) {
    const CardId card = cards_queued[card_idx];
    switch(card) {
      case kCardForward:
        do_move(1,1,straight_ticks);
        break;
      case kCardBackward:
        do_move(-1,-1,straight_ticks);
        break;
      case kCardLeft:
        do_move(-1,1,turn_ticks);
        break;
      case kCardRight:
        do_move(1,-1,turn_ticks);
        break;
    }
    
    set_glow(0,0,0);
    for(const unsigned long smillis = millis();(millis() - smillis) < pause_millis;) {
      CardId scanned = read_one_card();
      if(scanned == kCardReset) {
        do_reset();
        goto finished_actions;
      } else if(scanned == kCardGo) {
        card_idx = ~0;  // Loop will increment
        go_jingle();
        break;
      }
    }
  }
  
 finished_actions:
  flush_cards();

}

void loop() {
  // Glow
  idle_glow.do_glow();
  
  // Card reading
  CardId scanned;
  if(scanned = read_one_card()) {
    Serial.println(scanned);
  
    // Card handling
    if(is_key_card(scanned)) {
      if(scanned == kCardRed) {
        idle_glow.set(255,0,0);
      } else if(scanned == kCardGreen) {
        idle_glow.set(0,255,0);
      } else if(scanned == kCardBlue) {
        idle_glow.set(0,0,255);
      } else if(scanned == kCardGo) {
        do_go();
      } else if(scanned == kCardReset) {
        do_reset();
      }
    } else {
        if(n_cards_queued == max_cards) {
          error_jingle();
        }
        else {
           cards_queued[n_cards_queued++] = scanned;
           scan_success_jingle();
        }
    }
  }
}


