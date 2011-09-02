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
nextTime(0),
fpsTime(0),
fpsCnt(0)
{
	this->window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Video::screenWidth, Video::screenHeight, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RendererInfo info;
	SDL_GetRendererInfo(this->renderer, &info);
	this->tex = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, Video::screenWidth, Video::screenHeight);
}

SDLVideoFairy::~SDLVideoFairy()
{
	SDL_DestroyTexture(this->tex);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
}


void SDLVideoFairy::dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask)
{
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
            	exit(0);
            }
    }
    uint32_t* line;
    uint8_t* line8;
	int pitch;
	SDL_LockTexture(this->tex, NULL, reinterpret_cast<void**>(&line8), &pitch);
	for(int y=0;y<screenHeight;y++){
		line = reinterpret_cast<uint32_t*>(line8);
		for(int x=0;x<screenWidth; x++){
			line[x] = nesPalette[nesBuffer[y][x] & paletteMask];
		}
		line8+=pitch;
	}
	SDL_UnlockTexture(this->tex);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(this->renderer, this->tex, NULL, NULL);
    SDL_RenderPresent(renderer);

    if(nextTime == 0){
    	uint32_t now = SDL_GetTicks();
    	nextTime = now * 6 + 100;
    	fpsTime = now;
    }else{
    	uint32_t now = SDL_GetTicks();
    	if(now - fpsTime >= 1000){
    		printf("FPS:%d\n",fpsCnt);
    		fpsTime+=1000;
    		fpsCnt = 0;
    	}
    	fpsCnt++;
    	uint32_t nowFactored = now * 6;
    	if(nowFactored < nextTime){
        	SDL_Delay((nextTime-nowFactored)/6);
    	}
    	nextTime+=100;
    }
}
