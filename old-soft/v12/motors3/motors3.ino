
#include <WS2812.h>
#define LEDCount 4   // Number of LEDs to drive
WS2812 LED(LEDCount);

//this is for the latest version of 17 MAR 2017 and ahead
//pins are assigned with Arduino 1.6.4 and version 1.0.2 from http://lauszus.com/Sanguino/


const int rst_wifi = 0;   //wifi
const int opt_A = 1;    //opt
const int opt_B = 2;    //opt
const int spk_A = 3;    //spk
const int spk_B = 4;    //spk
const int miso = 5;       //miso
const int debug = 30;       //debug pin

const int motor_l_pulse = 27;
const int motor_r_pulse = 28;

const int motor_l_a = 12;
const int motor_l_b = 13;

const int motor_r_a = 14;
const int motor_r_b = 15;

const int outputPin = 21;    //leds
const int pwr_off   = 29;    //pwr off

int time = 1000;
int speed = 240;

void setup() {
  pinMode(debug, OUTPUT);

  pinMode(10, INPUT);
  pinMode(11, INPUT);

  LED.setOutput(outputPin);
  /* You may uncomment one of the following three lines to switch
    to a different data transmission sequence for your addressable LEDs.
    (These functions can be used at any point in your program as needed.)   */

  LED.setColorOrderRGB();  // Uncomment for RGB color order
  //LED.setColorOrderBRG();  // Uncomment for BRG color order
  //LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)

  pinMode(pwr_off, OUTPUT);
  digitalWrite(pwr_off, HIGH);

  pinMode(motor_l_pulse, INPUT);
  pinMode(motor_r_pulse, INPUT);

  pinMode(motor_l_a, OUTPUT);
  pinMode(motor_l_b, OUTPUT);
  pinMode(motor_r_a, OUTPUT);
  pinMode(motor_r_b, OUTPUT);

  delay (1000);

  /*
  Frequency of oscillation f =  (clock) / (2 * Divisor * TOP)
  These are all for 16MHz, calculate half for 8MHz
  Setting 	Divisor 	Frequency
  0x01 	 	1 	 	31372.55
  0x02 	 	8 	 	3921.16
  0x03          64 	 	490.20   <--DEFAULT
  0x04  	256 	 	122.55
  0x05 	 	1024 	 	30.64
  TCCR1B = (TCCR1B & 0b11111000) | <setting>;

  Setting 	Divisor 	Frequency
  0x01 	 	1  		31372.55
  0x02 	 	8 	 	3921.16
  0x03  	32  		980.39
  0x04 	 	64 	 	490.20   <--DEFAULT
  0x05 	 	128  		245.10
  0x06  	256  		122.55
  0x07 	 	1024  		30.64
  TCCR2B = (TCCR2B & 0b11111000) | <setting>;
  */

  // TCCR1B = (TCCR1B & 0b11111000) | 0x03;
  // TCCR2B = (TCCR2B & 0b11111000) | 0x04;
  digitalWrite(debug, HIGH);

  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    delay(50);
    set_color(255, 255, 255);
    delay(50);
  }
  digitalWrite(debug, LOW);

  Serial.begin(57600);
  Serial.println("Gogogo");
}

void loop() {
  int direction = 0;
  set_color(255, 0, 0);
  Serial.println(" front");
  motorLeft( speed, 0);
  motorRight( speed, 0);

  for (int i = 0; i < 10; i++) {
    digitalWrite( debug, HIGH );
    set_color(0, 0, 0);
    delay(50);
    digitalWrite(debug, LOW);
    set_color(255, 255, 255);
    delay(50);
  }

  set_color(0, 0, 0);
  Serial.println("stop");
  motorLeft( 0, direction);
  motorRight( 0, direction);
  delay(time);

  direction = 1;
  set_color(0, 0, 255);
  Serial.println("back");
  motorLeft( speed, direction);
  motorRight( speed, direction);

  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(255, 255, 255);
    digitalWrite(debug, LOW);
    delay(50);
  }

  set_color(0, 0, 0);
  Serial.println("stop");
  motorLeft( 0, direction);
  motorRight( 0, direction);
  delay(time);

  Serial.println("left front");
  motorLeft( speed, direction);
  motorRight( 0, direction);
  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(255, 0, 0);
    digitalWrite(debug, LOW);
    delay(50);
  }

  set_color(0, 0, 0);
  Serial.println("stop");
  motorLeft( 0, direction);
  motorRight( 0, direction);
  delay(time);

  direction = 1;
  Serial.println("left back");
  motorLeft( speed, direction);
  motorRight( 0, direction);

  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(0, 0, 255);
    digitalWrite(debug, LOW);
    delay(50);
  }

  set_color(0, 0, 0);
  Serial.println("stop");
  motorLeft( 0, direction);
  motorRight( 0, direction);
  delay(time);

  direction = 0;
  Serial.println("right front");
  motorLeft( 0, direction);
  motorRight( speed, direction);
  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(0, 255, 255);
    digitalWrite(debug, LOW);
    delay(50);
  }

  set_color(0, 0, 0);
  motorLeft( 0, direction);
  motorRight( 0, direction);
  Serial.println("stop");
  delay(time);

  direction = 1;
  set_color(255, 255, 0);
  Serial.println("right back");
  motorLeft( 0, direction);
  motorRight( speed, direction);
  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(255, 255, 0);
    digitalWrite(debug, LOW);
    delay(50);
  }

  set_color(0, 0, 0);
  Serial.println("stop");
  motorLeft( 0, direction);
  motorRight( 0, direction);
  delay(time);

  for (int i = 0; i < 10; i++) {
    set_color(0, 0, 0);
    digitalWrite(debug, HIGH);
    delay(50);
    set_color(0, 255, 0);
    digitalWrite(debug, LOW);
    delay(50);
  }
  digitalWrite(pwr_off, LOW);
  delay(time);
  //  while (1);
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


void motorLeft( int speed, int direction)
{
  if (speed > 235) speed = 235;
  if (speed < 20) speed = 20;
  if (direction = 0)
  {
    analogWrite(motor_l_a, 0);
    digitalWrite(motor_l_a, LOW);
    analogWrite(motor_l_b, 255 - speed);
  }
  if (direction = 1)
  {
    analogWrite(motor_l_a, 0);
    digitalWrite(motor_l_a, LOW);
    analogWrite(motor_l_b, speed);
  }
}

void motorRight( int speed, int direction)
{

  if (speed > 235) speed = 235;
  if (speed < 20) speed = 20;
  if (direction = 0)
  {
    analogWrite(motor_r_a, 0);
    digitalWrite(motor_r_a, LOW);
    analogWrite(motor_r_b, 255 - speed);
  }
  if (direction = 1)
  {
    analogWrite(motor_r_a, 1);
    digitalWrite(motor_r_b, LOW);
    analogWrite(motor_r_b, speed);
  }

}
