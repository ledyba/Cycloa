/*
 * SDLAudioFairy.h
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#ifndef SDLAUDIOFAIRY_H_
#define SDLAUDIOFAIRY_H_

#if defined(CYCLOA_SDL2)
#include <SDL2/SDL.h>
#elif defined(CYCLOA_SDL)
#include <SDL/SDL.h>
#endif

#include "../../emulator/fairy/AudioFairy.h"

class SDLAudioFairy: public AudioFairy
{
public:
	SDLAudioFairy();
	~SDLAudioFairy();
	static void callback(void* data, Uint8 *stream, int len);
};

#endif /* SDLAUDIOFAIRY_H_ */
