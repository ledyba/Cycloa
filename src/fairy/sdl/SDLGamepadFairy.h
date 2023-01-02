#pragma once
/*
 * SDLGamePadFairy.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#include <SDL3/SDL_gamepad.h>
#include "../../emulator/fairy/GamepadFairy.h"

class SDLGamepadInfo {

};

class SDLGamepadFairy final : public GamepadFairy {
public:
  explicit SDLGamepadFairy(SDLGamepadInfo &info);

  ~SDLGamepadFairy() noexcept override;

  void onVBlank() override;

  void onUpdate() override;

  bool isPressed(uint8_t keyIdx) override;

private:
  SDL_Joystick &joystick;
  uint8_t state;
};
