
#ifndef SD_AUDIO_H
#define SD_AUDIO_H

namespace SDAudio {

void Setup();

// This is a blocking function
// This function utilizes Timer 2 and pin 15. 
// Returns false on SD error.
//
// Streams 8 bit, unsigned, mono, 16 ksamples/second audio in 512 byte (32ms) increments. 
// Streams seamlessly, interleaving SD reads with PWM register writes.
boolean StreamBlocks(Sd2Card &card, unsigned long fromBlock, unsigned long count);

}


#endif//SD_AUDIO_H

