/*
 * SDL3DVideoFairy.h
 *
 *  Created on: 2011/12/25
 *      Author: psi
 */

#ifndef SDL3DVIDEOFAIRY_H_
#define SDL3DVIDEOFAIRY_H_

#include "SDLVideoFairy.h"
#include <string>

class SDL3DVideoFairy: public SDLVideoFairy
{
public:
	explicit SDL3DVideoFairy(std::string windowTitle, int width = Video::screenWidth*2, int height = Video::screenHeight*2);
	virtual ~SDL3DVideoFairy();
protected:
	virtual void dispatchRenderingImpl(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask, SDL_Renderer* renderer, SDL_Texture* tex);
private:
};

#endif /* SDL3DVIDEOFAIRY_H_ */
