
#include <SPI.h>
#include <SD.h>

#include "SDAudio.h"

const int sSpeakerPin = 15;

void SDAudio::Setup()
{
  pinMode(sSpeakerPin, OUTPUT);
  // Off: Coupling cap
  OCR2A = 0;
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
  
  // 62.5Khz fast PWM

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

  // Do non-inverting PWM on pin OC2A (p.155)
  // On the Arduino this is pin 11.
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~_BV(COM2A0);
  TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
}

boolean SDAudio::StreamBlocks(Sd2Card &card, unsigned long block, unsigned long count)
{
  const uint8_t start_partial = card.partialBlockRead();
  card.partialBlockRead(1);

  boolean ret = true;
  // Charge coupling cap softly
  for(int i=0;i<128;++i) {
    OCR2A = i;
    delayMicroseconds(1000);
  }
  
  const unsigned long to = block + count;
  uint8_t block_mem[512];
  unsigned long last_micros = micros();
  for(;block < to;++block) {
    for(unsigned int idx=0;idx<512;++idx) {
      uint8_t sample = 128;
      if(!card.readData(block, idx, 1, &sample)) {
        ret = false;
        goto done;
      }
      
      // 16khz. The above logic takes varying time, so this value was found experimentally
      // so that it sounds right (averages out)
      _delay_loop_1(255);
      _delay_loop_1(5);
      
      OCR2A = sample;
    }
  }
  
    // Discharge coupling cap softly
  for(int i=128;i>=0;--i) {
    OCR2A = i;
    delayMicroseconds(1000);
  }
  
  
 done:
  card.readEnd();
  card.partialBlockRead(start_partial);
  return ret;
}
