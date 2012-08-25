/*
 * SDLGamePadFairy.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#ifndef SDLGAMEPADFAIRY_H_
#define SDLGAMEPADFAIRY_H_

#include "../../emulator/fairy/GamepadFairy.h"

class SDLGamepadInfo
{

};

class SDLGamepadFairy: public GamepadFairy
{
public:
	SDLGamepadFairy(SDLGamepadInfo& info);
	virtual ~SDLGamepadFairy();
	virtual void onVBlank();
	virtual void onUpdate();
	virtual bool isPressed(uint8_t keyIdx);
private:
	SDL_Joystick& joystick;
	uint8_t state;
};

#endif /* SDLGAMEPADFAIRY_H_ */
