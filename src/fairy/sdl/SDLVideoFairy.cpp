/*
 * SDLVideoFairy.cpp
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */
#include <SDL/SDL.h>
#include "../../emulator/VirtualMachine.h"
#include "SDLVideoFairy.h"
#include <stdint.h>
#include <cstddef>

SDLVideoFairy::SDLVideoFairy(std::string windowTitle):
VideoFairy(NULL, RGB_888, Video::screenWidth, Video::screenHeight, Video::screenWidth, 4),
nextTime(0)
{
	this->window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Video::screenWidth, Video::screenHeight, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RendererInfo info;
	SDL_GetRendererInfo(this->renderer, &info);
#ifdef SDL_BIG_ENDIAN
	this->tex = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, Video::screenWidth, Video::screenHeight);
	setBytesPerPixel(static_cast<uint16_t>(SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_RGB888)));
#endif
#ifdef SDL_LIL_ENDIAN
	this->tex = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_BGR888, SDL_TEXTUREACCESS_STREAMING, Video::screenWidth, Video::screenHeight);
	setBytesPerPixel(static_cast<uint16_t>(SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_RGB888)));
#endif
}

SDLVideoFairy::~SDLVideoFairy()
{
	SDL_DestroyTexture(this->tex);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
}

void SDLVideoFairy::enter(int32_t waitLimit)
{
	uint8_t* data;
	int pitch;
	SDL_LockTexture(this->tex, NULL, reinterpret_cast<void**>(&data), &pitch);
	setData(data);
	setPitch(pitch);
}

void SDLVideoFairy::leave()
{
	SDL_UnlockTexture(this->tex);
	setData(NULL);
}

void SDLVideoFairy::dispatchRendering()
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
            	exit(0);
            }
    }
    SDL_RenderClear(renderer);
	SDL_RenderCopy(this->renderer, this->tex, NULL, NULL);
    SDL_RenderPresent(renderer);

    if(nextTime == 0){
    	nextTime = SDL_GetTicks()+16;
    }else{
    	uint32_t now = SDL_GetTicks();
    	if(now < nextTime){
        	SDL_Delay(nextTime-now);
    	}
    	nextTime+=16;
    }
}
