/*
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                             readWriteString.ino                                                               |
|                                                               SPIFlash library                                                                |
|                                                                   v 2.2.0                                                                     |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                    Marzogh                                                                    |
|                                                                  25.11.2015                                                                   |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
|                                                                                                                                               |
|                        This program shows the method of reading a string from the console and saving it to flash memory                       |
|                                                                                                                                               |
|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/
#include<SPIFlash.h>
#include<SPI.h>

#define cs 18
#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above


int strPage, strSize;
byte strOffset;

SPIFlash flash(cs);

void setup() {
  
  pinMode(RFID_NSS_PIN, OUTPUT);
  digitalWrite(RFID_NSS_PIN, HIGH);
  pinMode(cs, OUTPUT);
  digitalWrite(cs, LOW);
  
  Serial.begin(9600);
  Serial.println("--- Flash test ** ---");

  flash.begin();
  Serial.println("Initialized");
  Serial.print("Flash name ");
  Serial.print(flash.getChipName());
  Serial.print(" capacity ");
  Serial.println(flash.getCapacity());

  Serial.println(F("Please type the string into the console"));
  randomSeed(224);
  strPage = random(0, 4095);
  strOffset = random(0, 255);
  String inputString;
  while (!readSerialStr(inputString));
  flash.writeStr(strPage, strOffset, inputString);
  Serial.print(F("Written string: "));
  Serial.print(inputString);
  Serial.print(F(" to page "));
  Serial.print(strPage);
  Serial.print(F(", at offset "));
  Serial.println(strOffset);
  String outputString = "";
  flash.readStr(strPage, strOffset, outputString);
  Serial.print(F("Read string: "));
  Serial.print(outputString);
  Serial.print(F(" from page "));
  Serial.print(strPage);
  Serial.print(F(", at offset "));
  Serial.println(strOffset);
  flash.eraseSector(strPage, 0);
}

void loop() {

}

//Reads a string from Serial
bool readSerialStr(String &inputStr) {
  if (!Serial)
    Serial.begin(9600);
  while (Serial.available()) {
    inputStr = Serial.readStringUntil('\n');
    return true;
  }
  return false;
}

