/*
 * SDLGamePadFairy.cpp
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */


#if defined(CYCLOA_SDL2)
#include <SDL2/SDL.h>
#elif defined(CYCLOA_SDL)
#include <SDL/SDL.h>
#endif

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
	if(x > 16384){
		state |= GamepadFairy::MASK_RIGHT;
	}else if(x < -16384){
		state |= GamepadFairy::MASK_LEFT;
	}

	if(y > 16384){
		state |= GamepadFairy::MASK_DOWN;
	}else if(y < -16384){
		state |= GamepadFairy::MASK_UP;
	}

	state |= (SDL_JoystickGetButton(&joystick, 0)) << GamepadFairy::A;
	state |= (SDL_JoystickGetButton(&joystick, 1)) << GamepadFairy::B;
	state |= (SDL_JoystickGetButton(&joystick, 7)) << GamepadFairy::START;
	state |= (SDL_JoystickGetButton(&joystick, 6)) << GamepadFairy::SELECT;

	/* Key Board */
#if defined(CYCLOA_SDL2)
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
#elif defined(CYCLOA_SDL)
	const uint8_t* keyboard = SDL_GetKeyState(NULL);
	if(keyboard[SDLK_UP]){
		state |= GamepadFairy::MASK_UP;
	}
	if(keyboard[SDLK_DOWN]){
		state |= GamepadFairy::MASK_DOWN;
	}
	if(keyboard[SDLK_LEFT]){
		state |= GamepadFairy::MASK_LEFT;
	}
	if(keyboard[SDLK_RIGHT]){
		state |= GamepadFairy::MASK_RIGHT;
	}
	if(keyboard[SDLK_z]){
		state |= GamepadFairy::MASK_A;
	}
	if(keyboard[SDLK_x]){
		state |= GamepadFairy::MASK_B;
	}
	if(keyboard[SDLK_a]){
		state |= GamepadFairy::MASK_START;
	}
	if(keyboard[SDLK_s]){
		state |= GamepadFairy::MASK_SELECT;
	}
#endif
}

bool SDLGamepadFairy::isPressed(uint8_t keyIdx)
{
	return ((state >> keyIdx) & 1) == 1;
}

void SDLGamepadFairy::onUpdate(){
}
