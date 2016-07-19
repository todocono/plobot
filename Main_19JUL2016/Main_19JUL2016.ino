#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>

#include <Servo.h> 
#include <SPI.h>

#include "cards.h"
#include "lights.h"
#include "sound.h"
#include "jingles.h"

const int ultra_power = 22;
const int trigPin = 19;
const int echoPin = 20;

const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

int servos[2] = {3,4};

IdleGlow idle_glow(2000000L, 255, 255, 255);

void setup() {
  
  init_sound();

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
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


void loop() {
  // Glow
  idle_glow.do_glow();
  
  // Card reading
  if(read_one_card()) {
    scan_success_jingle();
  }
}


