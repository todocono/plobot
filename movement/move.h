
#ifndef MOVE_H
#define MOVE_H

#include <Arduino.h>

void init_movement();
boolean turn(int degs);
// Sign determines forward/back
boolean move_straight(int pulses);

#endif//MOVE_H

