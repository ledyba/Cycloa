/*
 * SDLVideoFairy.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#ifndef SDLVIDEOFAIRY_H_
#define SDLVIDEOFAIRY_H_

#include <SDL/SDL.h>
#include "../../emulator/fairy/VideoFairy.h"

class SDLVideoFairy : public VideoFairy
{
	public:
		explicit SDLVideoFairy(std::string windowTitle);
		virtual ~SDLVideoFairy();
		void dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask);
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* tex;
		uint32_t nextTime;

		uint32_t fpsTime;
		uint32_t fpsCnt;
};


#endif /* SDLVIDEOFAIRY_H_ */
