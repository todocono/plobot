#ifndef SENSE_H
#define SENSE_H

#include <Arduino.h>

void init_sense();

// Returns FLT_MAX when nothing is seen
float sense_distance_cm();

// Returns FLT_MAX when nothing is seen
float sense_mic();

// Returns FLT_MAX when nothing is seen
float sense_gyro();

#endif//SENSE_H

