
#include<SPIFlash.h>
#include<SPI.h>
#include "SDAudio.h"

#define cs 18
#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above

SPIFlash flash(cs);


const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

void setup() {
  Serial.begin(57600);
  while (!Serial);
  
  Serial.println("Starting...");
  
  SDAudio::Setup();

  flash.begin();

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
}

void loop(void) {
  Serial.println("Streaming..");
  if(!SDAudio::StreamBlocks(flash, 1, 5000)) {
    Serial.println("Error streaming blocks");
  }
  delay(750);
}
