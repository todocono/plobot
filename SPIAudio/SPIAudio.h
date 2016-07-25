
#ifndef SD_AUDIO_H
#define SD_AUDIO_H

namespace SDAudio {

void Setup();

enum AudioRate {
  AudioRate_Null=0,
  AudioRate_16khz,
  AudioRate_32khz,
};

// This is a blocking function
boolean StreamBlocks(SPIFlash &flash, unsigned long fromBlock, unsigned long count, AudioRate rate);

// This is a blocking function
boolean RecordBlocks(SPIFlash &flash, unsigned long fromBlock, unsigned long count, AudioRate rate);

}


#endif//SD_AUDIO_H

