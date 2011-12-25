/*
 * SDL3DVideoFairy.cpp
 *
 *  Created on: 2011/12/25
 *	  Author: psi
 */

#include "SDL3DVideoFairy.h"

SDL3DVideoFairy::SDL3DVideoFairy(const std::string windowTitle, int width, int height):
SDLVideoFairy(windowTitle, width, height)
{
}

SDL3DVideoFairy::~SDL3DVideoFairy()
{
}

void SDL3DVideoFairy::dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Renderer *renderer, SDL_Texture* tex)
{
	uint32_t* line;
	uint8_t* line8Base;
	uint8_t* line8;
	int pitch;
	SDL_LockTexture(tex, NULL, reinterpret_cast<void**>(&line8Base), &pitch);
	line8 = line8Base;
	for(int y=0;y<screenHeight;y++){
		line = reinterpret_cast<uint32_t*>(line8);
		for(int x=0;x<screenWidth; x++){
			const uint32_t color32 = nesPalette[nesBuffer[y][x] & 0x3f];
			line[x] = color32;
		}
		line8+=pitch;
	}
	line8 = line8Base;
	for(int y=0;y<screenHeight;y++){
		line = reinterpret_cast<uint32_t*>(line8);
		for(int x=0;x<screenWidth; x++){
			const uint8_t buffer = nesBuffer[y][x];
			const uint32_t color32 = nesPalette[buffer & 0x3f];
			const uint32_t color = static_cast<uint8_t>(((color32 & 0xff) + ((color32 >> 8) & 0xff) + ((color32 >> 16) & 0xff))/3);
			switch(buffer & Video::LayerBitMask){
			case Video::FrontSpriteBit:
			{
				if(0<= x-10){
					line[x-10] |= color << 16;
				}
				if(x+10 < screenWidth){
					line[x+10] |= color;
				}
			}
				break;
			case Video::BackSpriteBit:
			{
				if(0<= x-10){
					line[x-10] |= color;
				}
				if(x+10 < screenWidth){
					line[x+10] |= color << 16;
				}
			}
				break;
			case Video::BackgroundBit:
			{
				if(0<=x-5){
					line[x-5] |= color;
				}
				if(x+5 < screenWidth){
					line[x+5] |= color << 16;
				}
			}
				break;
			}
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



