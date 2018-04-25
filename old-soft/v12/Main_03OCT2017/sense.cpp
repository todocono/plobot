
#include "sense.h"
#include <L3G4200D.h>
#include <Wire.h>

#include <float.h>

const int trigPin = 19;
const int echoPin = 20;
const int micPin = A0;

void init_sense() {
  //for ultrasound
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //for gyro
  Wire.begin();
  // TODO: Hack: First calibration is somehow wrong
  {
    L3G4200D gyroscope;
    while (!gyroscope.begin(L3G4200D_SCALE_250DPS, L3G4200D_DATARATE_400HZ_50))     // Set scale 250 dps and 400HZ Output data rate (cut-off 50)
    {
      Serial.println("Could not find the L3G4200D sensor, check wiring!");
      return;
    }
    gyroscope.calibrate(100);
  }
  //for microphone
  pinMode(micPin, INPUT);
}

float sense_distance_cm() {
  float num = 0, denum = 0;

  for (int s = 0; s < 5; ++s) {
    long duration;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 10000L);
    float distance = (duration / 2) / 29.1;
    if (distance < 200 && distance > 0) {
      num += distance;
      denum += 1.0f;
    }
    delay(50);
  }

  if (denum == 0.0f)
    return FLT_MAX;

  return num / denum;
}


float sense_mic() {
  float num = 0, denum = 0;

  if (denum == 0.0f)
    return FLT_MAX;

  return num / denum;
}


float sense_gyro() {
  float num = 0, denum = 0;

  if (denum == 0.0f)
    return FLT_MAX;

  return num / denum;
}
