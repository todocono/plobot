

#include<SPIFlash.h>
#include<SPI.h>

#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above


SPIFlash flash(18);


const int pin_spk_en = 14;


const int mic = 31;

int16_t avg_val = 0;


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


void setup() {
 // pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);
  
  Serial.begin(9600);

  flash.begin();
 
  pinMode(mic, INPUT);
  
  int32_t avg_val_accum = 0L;
  for(int i=0;i<50;++i) {
    avg_val_accum += int32_t(analogRead(mic));
    delay(1);
  }
  avg_val = avg_val_accum / 50L;
  
  Serial.print("Initialized with mic avg ");
  Serial.print(avg_val);
  Serial.print(" and flash capacity ");
  Serial.println(flash.getCapacity());
 

  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_16;    // set our own prescaler to 64 
}
  
void loop() {
  
  uint32_t last_address = 0;
  unsigned long lm = micros();
//  for(unsigned long fm = micros();(micros() - fm) < 1000000L;) {
  for(;last_address < 3000;) {/*
      const unsigned long nm = micros();
      const int16_t micros_elapsed = nm - lm;
      */
      const int8_t val = max(-120, min(120, (analogRead(mic) - avg_val) >> 2));
      /*
      flash.writeShort(last_address+0, val);
      flash.writeShort(last_address+2, micros_elapsed);
      */
      buffff[last_address++] = val;
      //last_address++;
      //last_address += 4L;
      //lm = nm;
  }
  
  const unsigned long elapsed_m = micros() - lm;
  
  Serial.print("Recorded ");
  Serial.print((last_address - 0)/* / 4L*/);
  Serial.print(" samples in ");
  Serial.println(elapsed_m);
  
  for(uint32_t address=0;address < last_address;address+=4) {
    /*
    int16_t val = flash.readShort(address+0);
    int16_t micros_elapsed = flash.readShort(address+2);
    digitalWrite(pin_spk_dir, (micros_elapsed >= 0) ? HIGH : LOW);
    analogWrite(pin_spk_en, abs(micros_elapsed));
    */
  //  digitalWrite(pin_spk_dir, (buffff[address] >= 0) ? HIGH : LOW);
    analogWrite(pin_spk_en, abs(buffff[address]));
    
    delayMicroseconds(100);
  }
  analogWrite(pin_spk_en, 0);
}
