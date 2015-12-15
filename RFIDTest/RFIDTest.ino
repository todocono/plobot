
#include "TimerOne.h"
#include <util/delay_basic.h>>

const int carrier_pin = 9;
const int out_pin = 13;
const int bit_in = 2;

void setup() {
  Serial.begin(9600);
  
  pinMode(carrier_pin, OUTPUT);
  pinMode(out_pin, OUTPUT);
  pinMode(bit_in, INPUT);         

  // 9 microsecond period (actually sets to 125khz)
  Timer1.initialize(9);
  Timer1.pwm(carrier_pin, 512);
  
  Serial.println("---- Reset");

  //attachInterrupt(digitalPinToInterrupt(bit_in), bit_boundary, CHANGE);
}

volatile unsigned short last_io = 0;

void try_decode_buffer(uint8_t const*buf)
{
  int initial_ones = 0;
  for(int i=0;(i<64) && (buf[i]);++i, ++initial_ones);
  // Check start and stop bits
  if(initial_ones < 9 || buf[63])
    return;
    
 uint32_t value = 0;

 for(int i=0;i<32;++i) {
   value |= (buf[(19 + i % 4 + 5 * (i / 4))] << i);
 }
 Serial.println(value);
  
    return;
    
  // Check parity
  // Odd parity for version rows, even for the rest
  uint8_t col_par[4] = {0,0,0,0}, 
          row_par[10] = {0,0,1,1,1,1,1,1,1,1};
  for(int row=0;row<10;++row) {
    for(int col=0;col<4;++col) {
      const uint8_t val = buf[9 + row * 5 + col];
      if(val) {
        ++col_par[col];
        ++row_par[row];
      }
    }
  }
  for(int row=0;row<10;++row) {
    if((row_par[row] & 1) != buf[14 + 5 * row]) {
      return;
    }
  }
  Serial.println("par row");
  
  // Even parity
  for(int col=0;col<4;++col) {
    if((col_par[col] & 1) != buf[59 + col])
      return;
  }
  Serial.println("par col");
    
  Serial.println("Could have one");
}

void bit_boundary(const uint8_t last_state) {
  static uint8_t decode_buffer[64];
  static uint8_t buf[128];
  static unsigned short buf_idx = 0;
  static unsigned long last_ran = 0;
  
  const unsigned long sm = micros();
  const unsigned long time_since_last = (sm - last_ran);
  last_ran = sm;
  uint8_t last_rf_cycles = 1;

  if(time_since_last < 100)
    return;
  
  if(last_state && time_since_last > 350)
    last_rf_cycles = 2;
  else if(!last_state && time_since_last > 425)
    last_rf_cycles = 2;

  last_io = last_rf_cycles;
  
  boolean manchester = true;
  for(uint8_t i=0;i<last_rf_cycles;++i) {
    buf[buf_idx] = last_state ? 1 : 0;
    buf_idx = (buf_idx+1)%128;
    for(unsigned short idx=0;idx<64;++idx) {
      const uint8_t pair[2] = {buf[(idx*2+126+buf_idx)%128],
                               buf[(idx*2+127+buf_idx)%128]};
      if(pair[0] == pair[1]) {
        // Not Manchester coded
        manchester = false;
      }
      decode_buffer[idx] = (pair[0] == HIGH) ? 1 : 0;
    }
    
    if(manchester)
      try_decode_buffer(&decode_buffer[0]);
  }
}

#if 0


void bit_boundary() {
  static uint8_t decode_buffer[64];
  static uint8_t buf[128];
  static unsigned short buf_idx = 0;
  static unsigned long last_ran = 0;
  
//  noInterrupts();
  
  const unsigned long sm = micros();
  const unsigned long time_since_last = (sm - last_ran);
  last_ran = sm;
  
  if(time_since_last < 100) {
    // Ignore bounce
    buf_idx = (buf_idx - 1) % 128;
  }
  else if(time_since_last > 1000) {
    // Just started, discard
    return;
  }
  const int repeat_times = (time_since_last + 125) / 250;
    
//  _delay_loop_2(40);
  _delay_loop_2(200);
  
  buf[buf_idx] = digitalRead(bit_in);
  digitalWrite(out_pin, buf[buf_idx]);
  
  boolean manchester = true;
  for(int i = 0;i<repeat_times;++i) {
    buf_idx = (buf_idx+1)%128;
    for(unsigned short idx=0;idx<64;++idx) {
      const uint8_t pair[2] = {buf[(idx*2+126+buf_idx)%128],
                               buf[(idx*2+127+buf_idx)%128]};
      if(pair[0] == pair[1]) {
        // Not Manchester coded
        manchester = false;
      }
      // 0 to 1 transition is a 1
      decode_buffer[idx] = (pair[0] == LOW) ? 1 : 0;
    }
    
    if(manchester)
      try_decode_buffer(&decode_buffer[0]);
  }

  //interrupts();
}
#endif

void loop() {
  //noInterrupts(); 
  
  // micros() needs interrupts
  interrupts();
  uint8_t last_state = LOW;
  while(true) {
    
    const uint8_t state = digitalRead(bit_in);
    if(state != last_state) {
      bit_boundary(last_state);
      last_state = state;
    }
  }
  //Serial.println(last_io);
}
