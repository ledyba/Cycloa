/*
 * SDLVideoFairy.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#ifndef SDLVIDEOFAIRY_H_
#define SDLVIDEOFAIRY_H_

#include <SDL2/SDL.h>
#include "../../emulator/VirtualMachine.h"
#include "../../emulator/fairy/VideoFairy.h"

class SDLVideoFairy : public VideoFairy
{
public:
	explicit SDLVideoFairy(std::string windowTitle, int width = Video::screenWidth*2, int height = Video::screenHeight*2);
	virtual ~SDLVideoFairy();
	void dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask);
	int getWidth(){
		return this->width;
	}
	int getHeight(){
		return this->width;
	}
protected:
	virtual void dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Renderer* renderer, SDL_Texture* tex);
	SDL_Renderer* getRenderer(){
		return this->renderer;
	}
	SDL_Window* getWindow(){
		return this->window;
	}
private:
	const int width;
	const int height;
	bool isFullscreen;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* tex;
	uint32_t nextTime;

	uint32_t fpsTime;
	uint32_t fpsCnt;
};


#endif /* SDLVIDEOFAIRY_H_ */
