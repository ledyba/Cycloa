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
		void enter(int32_t waitLimit = -1);
		void leave();
		void dispatchRendering();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* tex;
		uint32_t nextTime;
};


#endif /* SDLVIDEOFAIRY_H_ */
