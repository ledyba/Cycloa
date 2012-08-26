/**
 * CycloaNaCl
 * Copyright (C) 2012 psi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "./NACLGamepadFairy.h"

NACLGamepadFairy::NACLGamepadFairy(CycloaInstance* cycloa) :
GamepadFairy(),
state(0)
{
	// TODO Auto-generated constructor stub

}

NACLGamepadFairy::~NACLGamepadFairy()
{
	// TODO Auto-generated destructor stub
}

void NACLGamepadFairy::onUpdate()
{

}
bool NACLGamepadFairy::isPressed(uint8_t keyIdx)
{
	return ((this->state >> keyIdx) & 0x1) == 0x1;
}

bool NACLGamepadFairy::transInputEvent(const pp::InputEvent& event)
{
	const PP_InputEvent_Type type = event.GetType();
	if(type != PP_INPUTEVENT_TYPE_KEYUP && type != PP_INPUTEVENT_TYPE_KEYDOWN){
		return false;
	}
	const bool push = type == PP_INPUTEVENT_TYPE_KEYDOWN;
	pp::KeyboardInputEvent evt(event);
	const uint32_t code = evt.GetKeyCode();
	if(push){
		switch (code) {
		case 38:
			this->state |= GamepadFairy::MASK_UP;
			break;
		case 40:
			this->state |= GamepadFairy::MASK_DOWN;
			break;
		case 37:
			this->state |= GamepadFairy::MASK_LEFT;
			break;
		case 39:
			this->state |= GamepadFairy::MASK_RIGHT;
			break;
		case 90:
			this->state |= GamepadFairy::MASK_A;
			break;
		case 88:
			this->state |= GamepadFairy::MASK_B;
			break;
		case 32:
			this->state |= GamepadFairy::MASK_SELECT;
			break;
		case 13:
			this->state |= GamepadFairy::MASK_START;
			break;
		}
	}else{
		switch (code) {
		case 38:
			this->state &= ~GamepadFairy::MASK_UP;
			break;
		case 40:
			this->state &= ~GamepadFairy::MASK_DOWN;
			break;
		case 37:
			this->state &= ~GamepadFairy::MASK_LEFT;
			break;
		case 39:
			this->state &= ~GamepadFairy::MASK_RIGHT;
			break;
		case 90:
			this->state &= ~GamepadFairy::MASK_A;
			break;
		case 88:
			this->state &= ~GamepadFairy::MASK_B;
			break;
		case 32:
			this->state &= ~GamepadFairy::MASK_SELECT;
			break;
		case 13:
			this->state &= ~GamepadFairy::MASK_START;
			break;
		}
	}
	return true;
}
