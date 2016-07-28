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


const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

int strPage, strSize;
byte strOffset;

SPIFlash flash(cs);

void setup() {
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);
  
  pinMode(RFID_NSS_PIN, OUTPUT);
  digitalWrite(RFID_NSS_PIN, HIGH);
  pinMode(cs, OUTPUT);
  digitalWrite(cs, LOW);
  
  Serial.begin(57600);
  Serial.println("--- Flash test ---");

  flash.begin();
  
  uint8_t page[256];
 // flash.eraseBlock32K(1,0);
  /*
  sprintf(page, "Fiiiiiiiii");
  if(!flash.writePage(1,(uint8_t*)&page[0])) {
    Serial.println("writePage error");
    //return;
  }
  */
  
  /*
  const unsigned long sm = micros();
  boolean b = flash.eraseBlock64K(1, 0);
  const unsigned long em = micros();
  Serial.println();
  Serial.print("eraseBlock64K micros ");
  Serial.print(b);
  Serial.print(": ");
  Serial.println(em-sm);
*/
  const unsigned short pages_read[] = {
    1,2,3,4,5
  };
  for(unsigned short i=0;i<(sizeof(pages_read)/sizeof(*pages_read));++i) {
    flash.readPage(pages_read[i],(uint8_t*)&page[0]);
    
    Serial.println("------- START ------");
    for(int i=0;i<256;++i) {
      Serial.print(page[i], HEX);
      Serial.print(" ");
    }
    Serial.println("------- END ------");
  }
  
#if 0
  Serial.println("Initialized");
  Serial.print("Flash name ");
  Serial.print(flash.getChipName());
  Serial.print(" capacity ");
  Serial.println(flash.getCapacity());

  Serial.println(F("Please type the string into the console"));
  randomSeed(224);
//  strPage = random(0, 4095);
//  strOffset = random(0, 255);
strPage = 1;
strOffset = 0;
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
#endif
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

