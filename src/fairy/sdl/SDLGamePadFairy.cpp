/*
 * SDLGamePadFairy.cpp
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#include <SDL/SDL.h>
#include "SDLGamePadFairy.h"

SDLGamepadFairy::SDLGamepadFairy(SDLGamepadInfo& info):
joystick(*SDL_JoystickOpen(0)),
state(0)
{
}

SDLGamepadFairy::~SDLGamepadFairy()
{
	SDL_JoystickClose(&joystick);
}

void SDLGamepadFairy::onVBlank()
{
	SDL_JoystickUpdate();
	state = 0;
	int16_t x = SDL_JoystickGetAxis(&joystick, 0);
	if(x > 0x7fff/2){
		state |= GamepadFairy::MASK_RIGHT;
	}else if(x < (-0x7fff)/2){
		state |= GamepadFairy::MASK_LEFT;
	}

	int16_t y = SDL_JoystickGetAxis(&joystick, 1);
	if(y > 0x7fff/2){
		state |= GamepadFairy::MASK_DOWN;
	}else if(y < (-0x7fff)/2){
		state |= GamepadFairy::MASK_UP;
	}

	state |= (SDL_JoystickGetButton(&joystick, 0)) << GamepadFairy::A;
	state |= (SDL_JoystickGetButton(&joystick, 1)) << GamepadFairy::B;
	state |= (SDL_JoystickGetButton(&joystick, 2)) << GamepadFairy::START;
	state |= (SDL_JoystickGetButton(&joystick, 3)) << GamepadFairy::SELECT;
}

bool SDLGamepadFairy::isPressed(uint8_t keyIdx)
{
	return ((state >> keyIdx) & 1) == 1;
}
