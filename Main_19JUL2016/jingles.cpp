
#include "lights.h"
#include "sound.h"

void scan_success_jingle()
{
  // Indicate success
  for(int i=0;i<4;++i) {
    chip_tone(100, 2500 - 500 * i);
    set_glow(0,25 + 75 * i,0);
  }
}

void error_jingle() {
  set_glow(255, 0, 0);
  chip_tone(500, 5000);
}

void reset_jingle() {
  const float tmax = 2.75f;
  for(float t=0;t<tmax;t+=0.1f) {
    const float n = sin(t * 2 * M_PI);
    const float scale = 1.0f - (t / tmax);
    chip_tone(100, 750 + 750 * (0.5f + 0.5f * n), scale * DEFAULT_VOLUME);
    set_glow(max(0, n*scale*255),0,max(0, -n*scale*255));
  }
}

void go_jingle() {
  set_glow(64,64,0);
  chip_tone(300, 1250);
  set_glow(0,0,0);
  delay(200);
  set_glow(255,255,0);
  chip_tone(300, 1250);
  set_glow(0,0,0);
  delay(200);
  set_glow(0,255,0);
  chip_tone(300, 800);
  set_glow(0,0,0);
}
