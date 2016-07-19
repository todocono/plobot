
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

