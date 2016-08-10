
#include <Wire.h>
#include <L3G4200D.h>
#include <PID_v1.h>

#include "move.h"

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

void setup() {
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  Serial.begin(57600);
  init_movement();
  
  delay(1000);
}

void loop() {

  delay(500);
  Serial.println("Turning...");
  turn(90);

  delay(500);
  Serial.println("Moving sraight...");
  move_straight(1300);
}
