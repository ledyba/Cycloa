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

#ifndef CycloaNaCl_NACLGAMEPADFAIRY_H__CPP_
#define CycloaNaCl_NACLGAMEPADFAIRY_H__CPP_

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/input_event.h>
#include "../../emulator/fairy/GamepadFairy.h"

class NACLGamepadFairy: public GamepadFairy
{
private:
	uint32_t state;
	NACLGamepadFairy();
	NACLGamepadFairy(const NACLGamepadFairy& other);
	NACLGamepadFairy& operator= (const NACLGamepadFairy& other);
public:
	NACLGamepadFairy(pp::InstanceHandle& instance);
	virtual ~NACLGamepadFairy();
	virtual void onUpdate();
	virtual bool isPressed(uint8_t keyIdx);
public:
	bool transInputEvent(const pp::InputEvent& event);
};

#endif /* INCLUDE_GUARD */
