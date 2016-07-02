#include <Adafruit_NeoPixel.h>
#include <Servo.h> 

#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(14, 29);  // Create MFRC522 instance

const int ultra_power = 22;
const int trigPin = 19;
const int echoPin = 20;

const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, 21, NEO_GRB + NEO_KHZ400);

//Servo servos[2];
int servos[2] = {3,4};

void setup() {
  
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  pinMode(ultra_power, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(ultra_power, HIGH);
  
  pinMode(servos[0], OUTPUT);
  pinMode(servos[1], OUTPUT);
  //servos[0].attach(3);  // attaches the servo on pin 9 to the servo object 
//  servos[1].attach(4);  // attaches the servo on pin 9 to the servo object 
  strip.begin();

  for (uint16_t i = 0; i < 5; i++) {
    strip.setPixelColor(i, strip.Color(128,64,200));
  }
  strip.show();

	Serial.begin(57600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, type, and data blocks..."));

}

void loop() {
  
  analogWrite(servos[0], millis() % 128);
  analogWrite(servos[1], millis() % 128);
  
 // servos[0].write(millis() % 128);
 // servos[1].write(millis() % 128);
  digitalWrite(motor_l_dir, LOW);
  analogWrite(motor_l_en, 100);
  digitalWrite(motor_r_dir, HIGH);
  analogWrite(motor_r_en, 100);
  analogWrite(pin_spk_en, 100);
  digitalWrite(pin_spk_dir, HIGH);
  
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 10000L);
  long distance = (duration/2) / 29.1;
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }

	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
