
#include <Servo.h>

Servo servos[2];
const int pin_left_servo = 25;
const int pin_right_servo = 24;
int pos = 0;    // variable to store the servo position

void setup()
{
  servos[0].attach(pin_left_servo);  // attaches the servo on pin 9 to the servo object
  servos[1].attach(pin_right_servo);  // attaches the servo on pin 9 to the servo object
}

void loop()
{
  for (pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    servos[0].write(pos);              // tell servo to go to position in variable 'pos'
    servos[1].write(180 - pos);              // tell servo to go to position in variable 'pos'
    delay(3);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) // goes from 180 degrees to 0 degrees
  {
    servos[0].write(pos);              // tell servo to go to position in variable 'pos'
    servos[1].write(180 - pos);              // tell servo to go to position in variable 'pos'
    delay(3);                       // waits 15ms for the servo to reach the position
  }
}

