
#include "cards.h"
#include <Arduino.h>
#include <MFRC522.h>

MFRC522 mfrc522(14, 29);

void init_cards() {
  mfrc522.PCD_Init();		// Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
}

CardId read_one_card() {
  if(!mfrc522.PICC_IsNewCardPresent()) {
     return kCardNull;
  }

  if(!mfrc522.PICC_ReadCardSerial()) {
    return kCardNull;
  }

  uint32_t raw_id = 0;
  if(mfrc522.uid.size != sizeof(raw_id)) {
    Serial.print("ERROR: Card has wrong UID size: ");
    Serial.println(mfrc522.uid.size);
    return kCardNull;
  }
  memcpy(&raw_id, mfrc522.uid.uidByte, sizeof(CardId));

  switch(raw_id) {
    case 49486:
       return kCardRed;
    case 26606:
       return kCardGreen;
    case 30430:
       return kCardBlue;
    case 49150:
       return kCardGo;
    case 46334:
       return kCardReset;
    case 19982:
      return kCardForward;
    case 15166:
      return kCardBackward;
    case 51438:
      return kCardLeft;
    case 56846:
      return kCardRight;
    case 4766:
      return kCardArms;
    default:
      Serial.print("Unknown card ID received: ");
      Serial.println(raw_id);
      return kCardNull;
  }

  // One card at a time
  flush_cards();
  
  return kCardNull;
}

void flush_cards() {
  while(mfrc522.PICC_IsNewCardPresent()) {
    mfrc522.PICC_ReadCardSerial();
  }
}

boolean is_key_card(CardId card) {
  return card == kCardGo || 
         card == kCardReset || 
         card == kCardRed ||
         card == kCardGreen ||
         card == kCardBlue;
}

