#pragma once
/*
 * SDLGamePadFairy.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#include <SDL_joystick.h>
#include "../../emulator/fairy/GamepadFairy.h"

class SDLGamepadInfo {

};

class SDLGamepadFairy final : public GamepadFairy {
public:
  SDLGamepadFairy(SDLGamepadInfo &info);

  virtual ~SDLGamepadFairy() noexcept;

  virtual void onVBlank();

  virtual void onUpdate();

  virtual bool isPressed(uint8_t keyIdx);

private:
  SDL_Joystick &joystick;
  uint8_t state;
};
