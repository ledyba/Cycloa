/*
 * SDLGamePadFairy.cpp
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#include <SDL2/SDL.h>
#include "SDLGamepadFairy.h"

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
	/* Joystick */
	SDL_JoystickUpdate();
	state = 0;
#ifdef _MSC_VER /* for VC++ */
	const int16_t x = SDL_JoystickGetAxis(&joystick, 1);
	const int16_t y = SDL_JoystickGetAxis(&joystick, 0);
#else
	const int16_t x = SDL_JoystickGetAxis(&joystick, 0);
	const int16_t y = SDL_JoystickGetAxis(&joystick, 1);
#endif
	if(x > 100){
		state |= GamepadFairy::MASK_RIGHT;
	}else if(x < -100){
		state |= GamepadFairy::MASK_LEFT;
	}

	if(y > 100){
		state |= GamepadFairy::MASK_DOWN;
	}else if(y < -100){
		state |= GamepadFairy::MASK_UP;
	}

	state |= (SDL_JoystickGetButton(&joystick, 2)) << GamepadFairy::A;
	state |= (SDL_JoystickGetButton(&joystick, 1)) << GamepadFairy::B;
	state |= (SDL_JoystickGetButton(&joystick, 8)) << GamepadFairy::START;
	state |= (SDL_JoystickGetButton(&joystick, 0)) << GamepadFairy::SELECT;

	/* Key Board */
	const uint8_t *keyboard = SDL_GetKeyboardState(NULL);
	if(keyboard[SDL_SCANCODE_UP]){
		state |= GamepadFairy::MASK_UP;
	}
	if(keyboard[SDL_SCANCODE_DOWN]){
		state |= GamepadFairy::MASK_DOWN;
	}
	if(keyboard[SDL_SCANCODE_LEFT]){
		state |= GamepadFairy::MASK_LEFT;
	}
	if(keyboard[SDL_SCANCODE_RIGHT]){
		state |= GamepadFairy::MASK_RIGHT;
	}
	if(keyboard[SDL_SCANCODE_Z]){
		state |= GamepadFairy::MASK_A;
	}
	if(keyboard[SDL_SCANCODE_X]){
		state |= GamepadFairy::MASK_B;
	}
	if(keyboard[SDL_SCANCODE_A]){
		state |= GamepadFairy::MASK_START;
	}
	if(keyboard[SDL_SCANCODE_S]){
		state |= GamepadFairy::MASK_SELECT;
	}
}

bool SDLGamepadFairy::isPressed(uint8_t keyIdx)
{
	return ((state >> keyIdx) & 1) == 1;
}
