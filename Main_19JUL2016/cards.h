
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
};

void init_cards();
CardId read_one_card();
void flush_cards();
boolean is_key_card(CardId card);

#endif//CARDS_H
