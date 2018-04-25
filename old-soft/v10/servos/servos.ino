
#include <Servo.h>


#include <WS2812.h>
#define LEDCount 4   // Number of LEDs to drive
WS2812 LED(LEDCount);

const int led = 21;    //leds

Servo servos[2];
const int pin_left_servo = 25;
const int pin_right_servo = 24;
int pos = 0;    // variable to store the servo position

void setup()
{
  servos[0].attach(pin_left_servo);  // attaches the servo on pin 9 to the servo object
  servos[1].attach(pin_right_servo);  // attaches the servo on pin 9 to the servo object
  
  LED.setOutput(led);
  /* You may uncomment one of the following three lines to switch
    to a different data transmission sequence for your addressable LEDs.
    (These functions can be used at any point in your program as needed.)   */

  LED.setColorOrderRGB();  // Uncomment for RGB color order
  //LED.setColorOrderBRG();  // Uncomment for BRG color order
  //LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)

}

void loop()
{
  servos[0].write(180);              // tell servo to go to position in variable 'pos'
    servos[1].write(0); 
  
  for (int i = 0; i < 10; i++) {
    set_color(255, 0, 0);
    delay(50);
    set_color(0, 0, 0);
    delay(50);
  }
  
    
    delay (5000);
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



void set_color(int colorR, int colorG, int colorB) {
  for (int i = 0; i < LEDCount; i++) {
    cRGB value;
    value.b = colorB;
    value.g = colorG;
    value.r = colorR; // RGB Value -> Red Only
    LED.set_crgb_at(i, value); // Set value at LED found at index 0
  }

  LED.sync(); // Sends the data to the LEDs
  delay(10); // Wait (ms)
}


