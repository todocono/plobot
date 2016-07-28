
#include<SPIFlash.h>
#include<SPI.h>

#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>

#include <Servo.h> 
#include <SPI.h>

// Before running this test, run:
//  ln -s ../../SPIAudio/SPIAudio.cpp
//  ln -s ../../SPIAudio/SPIAudio.h
#include "SPIAudio.h"

#include "cards.h"
#include "lights.h"
#include "sound.h"
#include "jingles.h"
#include "motors.h"
#include "arms.h"

const int trigPin = 19;
const int echoPin = 20;

IdleGlow idle_glow(2000000L, 255, 255, 255);
const int between_cards_default_pause = 300;

CardSequence main_sequence, stored_sequence;

CardSequence *current_sequence = &main_sequence;

const int straight_ticks = 800;
const int turn_ticks = 650;

void setup() {
  init_sound();
  init_motors();
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  init_lights();
  
  Serial.begin(57600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();			// Init SPI bus
  SPIAudio::Setup();
  
  init_cards();
  init_arms();
  
  set_arms(-150, 400);
}

void do_reset() {
    main_sequence.clear();
    stored_sequence.clear();
    card_scan_jingle(kCardReset);
}

void set_pause_glow(unsigned long paused_start_ms) {
    const float t_since_paused = float(millis() - paused_start_ms) / 1000.0f;
    const float norm_t = t_since_paused / 0.25f;
    const float norm_level = 0.5f * (1.0f + sin(norm_t * 2 * M_PI));
    uint8_t level = uint8_t(128 + 128 * norm_level);
    set_glow(level,level,level);
}

void do_pause_glow(int pause_millis) {
  for(const unsigned long paused_start_ms = millis();(millis() - paused_start_ms) < pause_millis;) {
    set_pause_glow(paused_start_ms);
  }
}

void execute_sequence(CardSequence const&sequence) {  
  Serial.print(">>>> execute_sequence ");
  Serial.print((unsigned long)&sequence, HEX);
  Serial.print(" ");
  Serial.println(sequence.count());
  
  int arms_pos = 50;
  boolean paused = false;
  unsigned long paused_start_ms = 0;
  
  for(int card_idx=0;card_idx<sequence.count();) {
    unsigned long pause_millis = between_cards_default_pause;
    
    if(!paused) {
      const CardId card = sequence.atIndex(card_idx);
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
        case kCardLift:
          set_arms(arms_pos, 400);
          arms_pos = arms_pos ? 0 : 50;
          break;
        case kCardPause:
          do_pause_glow(1000);
          break;
        case kCardRepeat:
          if(&sequence != &stored_sequence)
            execute_sequence(stored_sequence);
          break;
        default: {
          if(is_note_card(card)) {
            play_note(card);
            pause_millis = 100;
          }
          break;
        }
      }
      ++card_idx;
      set_glow(0,0,0);
    } else {
      set_pause_glow(paused_start_ms);
      pause_millis = 1;
    }   
    
    for(const unsigned long smillis = millis();(millis() - smillis) < pause_millis;) {
      CardId scanned = read_one_card();
      if(scanned == kCardReset) {
        do_reset();
        return;
      } else if(scanned == kCardStart) {
        card_idx = 0;  // Loop will increment
        paused = false;
        card_scan_jingle(kCardStart);
        break;
      } else if(scanned == kCardPause) {
        paused = !paused;
        pause_jingle(paused);
        paused_start_ms = millis();
      }
    }
  }
  Serial.print("<<<< execute_sequence ");
  Serial.println(sequence.count());
}

void do_go() {
  if(!main_sequence.empty()) {
    card_scan_jingle(kCardStart);
  } else {
    error_jingle();
    return;
  }
  
  execute_sequence(main_sequence);
  
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
      } else if(scanned == kCardOrange) {
        idle_glow.set(255,128,0);
      } else if(scanned == kCardPink) {
        idle_glow.set(255,0,255);
      } else if(scanned == kCardYellow) {
        idle_glow.set(255,200,0);
      } else if(scanned == kCardPurple) {
        idle_glow.set(127,0,255);
      } else if(scanned == kCardStart) {
        do_go();
      } else if(scanned == kCardReset) {
        do_reset();
      } else if(scanned == kCardStore) {
        if(current_sequence == &main_sequence) {
          stored_sequence.clear();
          current_sequence = &stored_sequence;
          subroutine_jingle(false);
        } else if(current_sequence == &stored_sequence) {
          current_sequence = &main_sequence;
          subroutine_jingle(true);
        }
      }
    } else {
        if(!current_sequence->queue(scanned)) {
          error_jingle();
        } else {
           card_scan_jingle(scanned);
        }
    }
  }
}


