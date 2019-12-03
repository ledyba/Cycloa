#pragma once
/*
 * SDLAudioFairy.h
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#include <SDL.h>
#include "../../emulator/fairy/AudioFairy.h"

class SDLAudioFairy : public AudioFairy {
public:
  SDLAudioFairy();

  ~SDLAudioFairy();

  static void callback(void *data, Uint8 *stream, int len);
};