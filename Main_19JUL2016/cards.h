
#ifndef CARDS_H
#define CARDS_H

#include <Arduino.h>

enum CardId {
  kCardNull = 0,
  kCardGo,
  kCardReset,
  kCardForward,
  kCardBackward,
  kCardLeft,
  kCardRight,
  kCardArms,
  kCardRed,
  kCardGreen,
  kCardBlue,
  kCardTone,
  kCardVolume,
  kCardFuncA,
};

void init_cards();
CardId read_one_card();

#endif//CARDS_H
