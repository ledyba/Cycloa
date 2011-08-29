/*
 * SDLAudioFairy.h
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#ifndef SDLAUDIOFAIRY_H_
#define SDLAUDIOFAIRY_H_

#include <SDL/SDL.h>
#include "../../emulator/fairy/AudioFairy.h"

class SDLAudioFairy: public AudioFairy
{
public:
	SDLAudioFairy();
	~SDLAudioFairy();
	static void callback(void* data, Uint8 *stream, int len);
};

#endif /* SDLAUDIOFAIRY_H_ */
