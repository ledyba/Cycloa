#pragma once

/*
 * GamepadFairy.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#include <stdint.h>

class GamepadFairy {
public:
  enum {
    A = 0,
    B = 1,
    SELECT = 2,
    START = 3,
    UP = 4,
    DOWN = 5,
    LEFT = 6,
    RIGHT = 7,
    TOTAL = 8,
    MASK_A = 1,
    MASK_B = 2,
    MASK_SELECT = 4,
    MASK_START = 8,
    MASK_UP = 16,
    MASK_DOWN = 32,
    MASK_LEFT = 64,
    MASK_RIGHT = 128,
    MASK_ALL = 255,
  };

  GamepadFairy() = default;

  virtual ~GamepadFairy() noexcept = default;

  virtual void onVBlank() {};

  virtual void onUpdate() = 0;

  virtual bool isPressed(uint8_t keyIdx) = 0;

private:
};

class DummyGamepadFairy final : public GamepadFairy {
public:
  DummyGamepadFairy() = default;

  ~DummyGamepadFairy() noexcept override = default;

  void onUpdate() override {};

  bool isPressed(uint8_t keyIdx) override { return false; };
};
