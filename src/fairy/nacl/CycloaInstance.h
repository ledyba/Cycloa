/**
 * CYCLOA@Cycloa
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

#ifndef CYCLOA_Cycloa_CYCLOAINSTANCE_H__CPP_
#define CYCLOA_Cycloa_CYCLOAINSTANCE_H__CPP_

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/module.h>
#include "../../emulator/VirtualMachine.h"
#include "NACLAudioFairy.h"
#include "NACLVideoFairy.h"
#include "NACLGamepadFairy.h"

class CycloaInstance : public pp::Instance
{
private:
	static int nInstances;
	static const double kFrameInterval;
	int width;
	int height;
private:
	bool running;
	double lastTime;
	double nextTime;
	pthread_t thread;
	NACLVideoFairy videoFairy;
	NACLAudioFairy audioFairy;
	NACLGamepadFairy gamepadFairy;
	DummyGamepadFairy dummyPlayer;
	VirtualMachine vm;
private:
	pp::Core* core;
	pthread_mutex_t frameMutex;
	pthread_cond_t frameCond;
public:
	explicit CycloaInstance(pp::Core* core, PP_Instance instance);
	virtual ~CycloaInstance();
private:
	void* run();
public:
	void start();
	void stop();
	void loop(int32_t result);
public:
	static void* run(void* self);
	static void loop(void* user_data, int32_t result);
public:
	virtual bool HandleInputEvent(const pp::InputEvent& event);
	virtual void HandleMessage(const pp::Var& var_message);
	void CallOnMainThread(PP_CompletionCallback_Func func, void* data);
private:
	static void frameLoop(void* _self, int32_t val);
public:
	void frameWait();
};



#endif /* INCLUDE_GUARD */
