/*
 * SDLVideoFairy.cpp
 *
 *  Created on: 2011/08/23
 *	  Author: psi
 */
#include "../../emulator/VirtualMachine.h"
#include "SDLVideoFairy.h"
#include <stdint.h>
#include <cstddef>
#include <stdio.h>
#include <cstdlib>

SDLVideoFairy::SDLVideoFairy(std::string windowTitle, int width, int height):
width(width),
height(height),
isFullscreen(false),
nextTime(0),
fpsTime(0),
fpsCnt(0)
{
#if defined(CYCLOA_SDL2)
	if(SDL_CreateWindowAndRenderer(width, height, 0, &this->window, &this->renderer) <0)
	{
		throw EmulatorException("Failed to initialize window and renderer.");
	}
	SDL_SetWindowTitle(this->window, windowTitle.c_str());
	SDL_RendererInfo info;
	SDL_GetRendererInfo(this->renderer, &info);
	this->tex = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, Video::screenWidth, Video::screenHeight);
#elif defined(CYCLOA_SDL)
	SDL_SetVideoMode(width, height, 24, 0);
	this->screenSurface = SDL_GetVideoSurface();
	this->nesSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Video::screenWidth, Video::screenHeight, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000
#else
			0x0000ff00, 0x00ff0000, 0xff000000, 0x00000000
#endif
	);
#endif
}

SDLVideoFairy::~SDLVideoFairy()
{
#if defined(CYCLOA_SDL2)
	SDL_DestroyTexture(this->tex);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
#elif defined(CYCLOA_SDL)
	SDL_FreeSurface(this->nesSurface);
	SDL_FreeSurface(this->screenSurface);
#endif
}



void SDLVideoFairy::dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask)
{

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exit(0);
			}else
#if defined(CYCLOA_SDL2)
			if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
				if(SDL_SCANCODE_F == e.key.keysym.scancode){
					isFullscreen = !isFullscreen;
					SDL_SetWindowFullscreen(this->window, isFullscreen ? SDL_TRUE : SDL_FALSE);
				}else if(SDL_SCANCODE_ESCAPE == e.key.keysym.scancode){
					exit(0);
				}
			}
#elif defined(CYCLOA_SDL)
			if(e.type == SDL_KEYDOWN){
				if(SDLK_f == e.key.keysym.sym){
					isFullscreen = !isFullscreen;
					SDL_WM_ToggleFullScreen(this->screenSurface);
				}else if(SDLK_ESCAPE == e.key.keysym.sym){
					exit(0);
				}
			}
#endif
	}


#if defined(CYCLOA_SDL2)
	this->dispatchRenderingImpl(nesBuffer, paletteMask, this->renderer, this->tex);
#elif defined(CYCLOA_SDL)
	this->dispatchRenderingImpl(nesBuffer, paletteMask, this->nesSurface, this->screenSurface);
#endif

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

#if defined(CYCLOA_SDL2)
void SDLVideoFairy::dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Renderer *renderer, SDL_Texture* tex)
{
	uint32_t* line;
	uint8_t* line8;
	int pitch;
	SDL_LockTexture(tex, NULL, reinterpret_cast<void**>(&line8), &pitch);
	for(int y=0;y<screenHeight;y++){
		line = reinterpret_cast<uint32_t*>(line8);
		for(int x=0;x<screenWidth; x++){
			line[x] = nesPalette[nesBuffer[y][x] & paletteMask];
		}
		line8+=pitch;
	}
	SDL_UnlockTexture(tex);

	SDL_RenderClear(renderer);
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = this->getWidth();
	rect.h = this->getHeight();
	SDL_RenderCopy(renderer, tex, &rect, NULL);
	SDL_RenderPresent(renderer);
}
#elif defined(CYCLOA_SDL)
void SDLVideoFairy::dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Surface* nesSurface, SDL_Surface* screenSurface)
{
	uint32_t* line;
	SDL_LockSurface(nesSurface);
	uint8_t* line8 = reinterpret_cast<uint8_t*>(nesSurface->pixels);
	const uint16_t pitch = nesSurface->pitch;
	for(int y=0;y<screenHeight;y++){
		line = reinterpret_cast<uint32_t*>(line8);
		for(int x=0;x<screenWidth; x++){
			line[x] = nesPalette[nesBuffer[y][x] & paletteMask];
		}
		line8+=pitch;
	}
	SDL_UnlockSurface(nesSurface);

	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = this->getWidth();
	rect.h = this->getHeight();
	SDL_BlitSurface(nesSurface, 0, screenSurface, &rect);
	SDL_Flip(this->screenSurface);
}
#endif
