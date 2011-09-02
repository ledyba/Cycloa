/*
 * SDLAudioFairy.cpp
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#include "SDLAudioFairy.h"
#include <string.h>

SDLAudioFairy::SDLAudioFairy()
{
	SDL_AudioSpec desired, obtained;
	desired.callback = SDLAudioFairy::callback;
	desired.channels = 1;
	desired.format = AUDIO_S16SYS;
	desired.freq=44100;
	desired.samples=8192;
	desired.userdata = this;
	if ( SDL_OpenAudio(&desired, &obtained) < 0 ){
		throw new EmulatorException("Cannot open audio device");
	}
	if(	desired.format != obtained.format
			|| desired.channels != obtained.channels
			|| desired.freq != obtained.freq){
		throw new EmulatorException("Cannot fill audio spec.");
	}
	SDL_PauseAudio(0);
}

SDLAudioFairy::~SDLAudioFairy()
{
	// TODO Auto-generated destructor stub
}

void SDLAudioFairy::callback(void* data, Uint8* stream, int len)
{
	SDLAudioFairy* me = reinterpret_cast<SDLAudioFairy*>(data);
	int16_t* buffer = reinterpret_cast<int16_t*>(stream);
	unsigned int copiedLength = me->popAudio(buffer, len/sizeof(int16_t));
	memset(&buffer[copiedLength], 0, len-(copiedLength * sizeof(int16_t)));
}
