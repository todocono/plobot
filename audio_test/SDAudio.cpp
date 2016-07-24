
#include<SPIFlash.h>
#include<SPI.h>

#include "SDAudio.h"

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

void SDAudio::Setup()
{
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  // Off: don't burn speaker
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

boolean SDAudio::StreamBlocks(SPIFlash &flash, unsigned long block, unsigned long count)
{
  boolean ret = true;
  
  const unsigned long to = block + count;
  uint8_t block_mem[256];
  unsigned long last_micros = micros();
  for(;block < to;++block) {
    if(!flash.readPage(block, (uint8_t*)&block_mem[0])) {
      ret = false;
      break;
    }
      
    for(unsigned int idx=0;idx<256;++idx) {   
      // TODO: Tune precisely for new 8Mhz configuration   
      _delay_loop_1(140);
      uint8_t sample = block_mem[idx];
      digitalWrite(pin_spk_dir, (sample & 0b10000000) ? HIGH : LOW);
      
      // TODO: Volume level
      OCR2A = (sample & 0b01111111) << 1;
    }
  }
  return ret;
}
