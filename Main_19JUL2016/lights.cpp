
#include "lights.h"

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, 21, NEO_GRB + NEO_KHZ400);

uint32_t get_color(byte r, byte g, byte b) {
  return strip.Color(r,g,b);
}

void init_lights() {
  strip.begin();
  set_glow(0);
}

void set_glow(uint8_t r, uint8_t g, uint8_t b) {
  const uint32_t c = strip.Color(r,g,b);
  set_glow(c);
}

void set_glow(uint32_t c) {
  strip.setPixelColor(0, c);
  strip.setPixelColor(1, c);
  strip.show();
}

IdleGlow::IdleGlow(unsigned long glow_period_micros,
                  byte glow_r, 
                  byte glow_g, 
                  byte glow_b) 
 : glow_period_micros(glow_period_micros),
   last_glow_micros(micros()),
   glow_t_micros(0L),
   glow_r(glow_r),
   glow_g(glow_g),
   glow_b(glow_b) {
}

void IdleGlow::do_glow() {
    const unsigned long m = micros();
    const unsigned long glow_t_micros_offset = m - last_glow_micros;
    glow_t_micros += glow_t_micros_offset;
    last_glow_micros = m;
    float glow_t = float(glow_t_micros) / glow_period_micros;
    
    float glow_n = 0.5f - 0.5f*cos(glow_t * 2.0f * M_PI);

    set_glow(glow_n * glow_r, glow_n * glow_g, glow_n * glow_b);
}

void IdleGlow::set(byte glow_r, 
                   byte glow_g, 
                   byte glow_b) {
    this->glow_r = glow_r;
    this->glow_g = glow_g;
    this->glow_b = glow_b;
}

float IdleGlow::get_r()const {
  return glow_r;
}

float IdleGlow::get_g()const {
  return glow_g;
}


float IdleGlow::get_b()const {
  return glow_b;
}




