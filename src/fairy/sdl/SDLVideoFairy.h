/*
 * SDLVideoFairy.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#ifndef SDLVIDEOFAIRY_H_
#define SDLVIDEOFAIRY_H_

#if defined(CYCLOA_SDL2)
#include <SDL2/SDL.h>
#elif defined(CYCLOA_SDL)
#include <SDL/SDL.h>
#endif
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
private:
	const int width;
	const int height;
	bool isFullscreen;
	uint32_t nextTime;

	uint32_t fpsTime;
	uint32_t fpsCnt;
#if defined(CYCLOA_SDL2)
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* tex;
protected:
	virtual void dispatchRenderingImpl(const uint8_t (&nesBuffer)[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Renderer* renderer, SDL_Texture* tex);
#elif defined(CYCLOA_SDL)
	SDL_Surface* screenSurface;
	SDL_Surface* nesSurface;
protected:
	void dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Surface* nesSurface, SDL_Surface* screenSurface);
#endif
};


#endif /* SDLVIDEOFAIRY_H_ */
