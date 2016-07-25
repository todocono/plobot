
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
  noInterrupts();
  boolean ret = true;
  
  uint8_t block_mem[256];
  memset(&block_mem[0], 0, sizeof(block_mem));

  unsigned long last_micros = micros();
  
  flash._beginRead(flash._prepRead((uint16_t)block));
  for(uint32_t i=0;i<(uint32_t(count) * uint32_t(256L));++i) {
      // 16khz
//      _delay_loop_1(120);
      // 32khz
      _delay_loop_1(30);
      
      uint8_t sample = flash._readNextByte(true);
  
      int setI = int(sample) - 128;
      digitalWrite(pin_spk_dir, (setI < 0) ? HIGH : LOW);
      
      // TODO: 1 bit of wasted precision
      OCR2A = min(255, abs(setI) << 1);
  }
  OCR2A = 0;
  flash._endProcess();
  interrupts();
  return ret;
}
