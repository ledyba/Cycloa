/*
 * GamepadFairy.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#ifndef GAMEPADFAIRY_H_
#define GAMEPADFAIRY_H_

class GamepadFairy
{
public:
	const static uint8_t A=0;
	const static uint8_t B=1;
	const static uint8_t SELECT=2;
	const static uint8_t START=3;
	const static uint8_t UP=4;
	const static uint8_t DOWN=5;
	const static uint8_t LEFT=6;
	const static uint8_t RIGHT=7;
	const static uint8_t TOTAL=8;
	const static uint8_t MASK_A=1;
	const static uint8_t MASK_B=2;
	const static uint8_t MASK_SELECT=4;
	const static uint8_t MASK_START=8;
	const static uint8_t MASK_UP=16;
	const static uint8_t MASK_DOWN=32;
	const static uint8_t MASK_LEFT=64;
	const static uint8_t MASK_RIGHT=128;
	const static uint8_t MASK_ALL=255;
	GamepadFairy(){}
	virtual ~GamepadFairy(){}
	virtual void onVBlank(){};
	virtual void onUpdate(){};
	virtual bool isPressed(uint8_t keyIdx){return false;}
private:
};

class DummyGamepadFairy : public GamepadFairy
{
public:
	DummyGamepadFairy(){}
	virtual ~DummyGamepadFairy(){}
};

#endif /* GAMEPADFAIRY_H_ */
