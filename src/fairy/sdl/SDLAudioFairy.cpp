/*
 * SDLAudioFairy.cpp
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#include "SDLAudioFairy.h"

SDLAudioFairy::SDLAudioFairy()
:deviceId_(0)
{
  SDL_AudioSpec desired, obtained;
  desired.callback = SDLAudioFairy::callback;
  desired.channels = 1;
  desired.format = AUDIO_S16SYS;
  desired.freq = 44100;
  desired.samples = 4096;
  desired.userdata = this;
  int const numDevices = SDL_GetNumAudioDevices(SDL_FALSE);
  for(int i = 0; i < numDevices; ++i){
    char const*const name = SDL_GetAudioDeviceName(i, SDL_FALSE);
    this->deviceId_ = SDL_OpenAudioDevice(name, SDL_FALSE, &desired, &obtained, 0);
    if(this->deviceId_ != 0) {
      break;
    }
  }
  if(this->deviceId_ == 0) {
    throw EmulatorException("Cannot open audio device");
  }
  SDL_PauseAudioDevice(this->deviceId_, 0);
}

SDLAudioFairy::~SDLAudioFairy() noexcept {
  SDL_CloseAudioDevice(this->deviceId_);
}

void SDLAudioFairy::callback(void *data, Uint8 *stream, int len) {
  SDLAudioFairy *const me = reinterpret_cast<SDLAudioFairy *>(data);
  int16_t *const buffer = reinterpret_cast<int16_t *>(stream);
  const int maxLen = len / sizeof(int16_t);

  const int copiedLength = me->popAudio(buffer, maxLen);
  const int16_t fill = copiedLength > 0 ? buffer[copiedLength - 1] : 0;
  for (int i = copiedLength; i < maxLen; i++) {
    buffer[i] = fill;
  }
}
