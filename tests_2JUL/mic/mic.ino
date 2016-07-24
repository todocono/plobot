
#include<SPIFlash.h>
#include<SPI.h>


const int dled = 30;
const int mic = 31;


const int motor_l_en = 13;
const int motor_r_en = 12;

const int motor_l_dir = 24;
const int motor_r_dir = 25;

const int pin_spk_en = 15;
const int pin_spk_dir = 23;

int avg_val = 0;

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define cs 18
#define RFID_NSS_PIN          14         // Configurable, see typical pin layout above

SPIFlash flash(cs);

void setup() {
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
  
  Serial.begin(57600);
  
  pinMode(pin_spk_dir, OUTPUT);
  pinMode(pin_spk_en, OUTPUT);

  pinMode(motor_l_en, OUTPUT);
  pinMode(motor_r_en, OUTPUT);
  pinMode(motor_l_dir, OUTPUT);
  pinMode(motor_r_dir, OUTPUT);  
  
  pinMode(mic, INPUT);
  pinMode(dled, OUTPUT);
  
  flash.begin();
  
  long avg_val_accum = 0L;
  for(int i=0;i<100;++i) {
    avg_val_accum += analogRead(mic);
  }
  avg_val = avg_val_accum / 100L;
  
  
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

boolean RecordBlocks(uint16_t block, uint16_t count) {
  const uint16_t to = block + count;
  uint8_t block_mem[256];
  for(;block < to;++block) {
    for(unsigned int idx=0;idx<256;++idx) {   
      const int raw_val = analogRead(mic);
      const int val = raw_val - avg_val;
      uint8_t sample = max(0, min(255, 0x80 + val));
      block_mem[idx] = sample;
      
      // TODO: Tune precisely for new 8Mhz configuration   
      _delay_loop_1(120);
    }
    if(!flash.writePage(block,&block_mem[0])) {
      return false;
    }
  }

  return true; 
}

// TODO: Use SDAudio library
boolean StreamBlocks(unsigned long block, unsigned long count)
{
  boolean ret = true;
  
  const unsigned long to = block + count;
  uint8_t block_mem[256];
  unsigned long last_micros = micros();
  for(;block < to;++block) {
    OCR2A = 0;
    if(!flash.readPage(block, (uint8_t*)&block_mem[0])) {
      ret = false;
      break;
    }
      
    for(unsigned int idx=0;idx<256;++idx) {   
      // TODO: Tune precisely for new 8Mhz configuration   
      _delay_loop_1(120);
      uint8_t sample = block_mem[idx];
      digitalWrite(pin_spk_dir, (sample & 0b10000000) ? HIGH : LOW);
      uint8_t set = (sample & 0b10000000) ? (sample & 0b01111111) : (127 - sample & 0b01111111);
      // TODO: Volume level
      OCR2A = set << 1;
    }
  }
  return ret;
}

void loop() {
  Serial.println("Erasing 32k..");
  for(int i=0;i<(1+500/128);++i) {
    if(!flash.eraseBlock32K(1+i*128,0)) {
      Serial.println("Error erasing");
      return;
    }
  }
  
  Serial.println("Recording..");
  if(!RecordBlocks(1, 500)) {
    Serial.println("Error recording");
    return;
  }
  
  Serial.println("Playing..");
  if(!StreamBlocks(1, 500)) {
    Serial.println("Error streaming");
    return;
  }
  
}
