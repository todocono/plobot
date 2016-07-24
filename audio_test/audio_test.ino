
#include<SPIFlash.h>
#include<SPI.h>
#include "SDAudio.h"

#define cs 18
#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above

SPIFlash flash(cs);

void setup() {
  Serial.begin(57600);
  while (!Serial);
  
  SDAudio::Setup();

  flash.begin();
}

void loop(void) {
  Serial.println("Streaming..");
  if(!SDAudio::StreamBlocks(flash, 0, 5000)) {
    Serial.println("Error streaming blocks");
  }
  delay(750);
}
