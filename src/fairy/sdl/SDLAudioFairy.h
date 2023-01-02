#pragma once
/*
 * SDLAudioFairy.h
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#include <SDL3/SDL.h>
#include "../../emulator/fairy/AudioFairy.h"

class SDLAudioFairy final : public AudioFairy {
private:
  SDL_AudioDeviceID deviceId_;
public:
  SDLAudioFairy();
  ~SDLAudioFairy() noexcept override;

  static void callback(void *data, Uint8 *stream, int len);
};