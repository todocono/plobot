
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
  memcpy(&raw_id, mfrc522.uid.uidByte, sizeof(raw_id));
Serial.print("raw_id ");
Serial.println(raw_id);
  CardId ret = kCardNull;

  switch(raw_id) {
    case 4022845438:
      ret = kCardStart;
      break;
    case 4023366910:
      ret = kCardReset;
      break;	
    case 4022881806:
      ret = kCardForward;
      break;	
    case 4023044334:
      ret = kCardLeft;
      break;
    case 4024163854:
      ret = kCardRight;
      break;
    case 4024580926:
      ret = kCardBackward;
      break;	
    case 4024373918:
      ret = kCardLift;
      break;
    case 4023109790:
      ret = kCardListen;
      break;
    case 4023814350:
      ret = kCardSee;
      break;
    case 4022752894:
      ret = kCardFeel;
      break;
    case 4023838654:
      ret = kCardRepeat;
      break;
    case 4023479230:
      ret = kCardStore;
      break;
    case 4022925038:
      ret = kCardRandom;
      break;
    case 4022726558:
      ret = kCardTeach;
      break;
    case 4023381310:
      ret = kCardBoost;
      break;
    case 4023822254:
      ret = kCardPause;
      break;
    case 4022660942:
      ret = kCardSound;
      break;
    case 4024090958:
      ret = kCardRed;
      break;
    case 4022888430:
      ret = kCardGreen;
      break;
    case 4022892430:
      ret = kCardOrange;
      break;
    case 4023831934:
      ret = kCardYellow;
      break;
    case 4023282862:
      ret = kCardPink;
      break;
    case 4024213694:
      ret = kCardPurple;
      break;
    case 4024006366:
      ret = kCardBlue;
      break;
    case 4023906254:
      ret = kCardMusicDo;
      break;
    case 4024583486:
      ret = kCardMusicRe;
      break;	
    case 4024035966:
      ret = kCardMusicMi;
      break;	
    case 4024720814:
      ret = kCardMusicFa;
      break;	
    case 4024715182:
      ret = kCardMusicSol;
      break;	
    case 4023629246:
      ret = kCardMusicLa;
      break;
    case 3310867317:
      ret = kCardMusicSi;
      break;	

    default:
      Serial.print("Unknown card ID received: ");
      Serial.println(raw_id);
      ret = kCardNull;
  }

  // One card at a time
  flush_cards();
  
  return ret;
}

void flush_cards() {
  while(mfrc522.PICC_IsNewCardPresent()) {
    mfrc522.PICC_ReadCardSerial();
  }
}

boolean is_key_card(CardId card) {
  return card == kCardStart || 
         card == kCardReset || 
         card == kCardRed ||
         card == kCardGreen ||
         card == kCardOrange ||
         card == kCardYellow ||
         card == kCardPink ||
         card == kCardPurple ||
         card == kCardBlue;
}

boolean is_note_card(CardId card) {
  return card >= kCardMusicDo && card <= kCardMusicSi;
}

