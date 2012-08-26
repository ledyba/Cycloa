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

#include <ppapi/c/ppb_input_event.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/core.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var_array_buffer.h>
#include <ppapi/utility/completion_callback_factory.h>
#include "CycloaInstance.h"
#include <stdio.h>

const double CycloaInstance::kFrameInterval = (1.0f/60.0f);

CycloaInstance::CycloaInstance(pp::Core* core, PP_Instance instance)
: pp::Instance(instance),
width(0), height(0),
running(false),
lastTime(-1),
nextTime(-1),
thread(0),
videoFairy(this),
audioFairy(this),
gamepadFairy(this),
dummyPlayer(),
vm(this->videoFairy, this->audioFairy, &this->gamepadFairy, &this->dummyPlayer),
core(core)
{
	this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_KEYBOARD);
	nInstances++;
	assert(nInstances == 1);
}
CycloaInstance::~CycloaInstance()
{
}
bool CycloaInstance::HandleInputEvent(const pp::InputEvent& event)
{
	return this->gamepadFairy.transInputEvent(event);
}

void CycloaInstance::start(){
	if(this->thread) {
		this->PostMessage(pp::Var("Oops. VM already started."));
		return;
	}
	this->running = true;
	this->audioFairy.start();
	pthread_create(&this->thread, NULL, CycloaInstance::run, this);
	this->PostMessage(pp::Var("Game started."));
	const double now = this->core->GetTimeTicks();
	this->lastTime = now;
	this->nextTime = now+CycloaInstance::kFrameInterval;
	this->core->CallOnMainThread(16, pp::CompletionCallback(CycloaInstance::frameLoop, this), 0);
}
void CycloaInstance::stop(){
	this->PostMessage(pp::Var("Stopped."));
	this->audioFairy.stop();
	this->running = false;
	pthread_join(this->thread, NULL);
	this->thread = 0;
}

void* CycloaInstance::run()
{
	try {
		while(this->running){
			this->vm.run();
		}
	} catch (const EmulatorException& e) {
		this->PostMessage("Error: "+e.getMessage());
		this->stop();
	} catch(const char* dummy) {
		this->stop();
	} catch (...) {
		this->PostMessage("Unknown Exception occured.");
		this->stop();
	}
	return 0;
}

void* CycloaInstance::run(void* self)
{
	return reinterpret_cast<CycloaInstance*>(self)->run();
}

void CycloaInstance::HandleMessage(const pp::Var& var_message)
{
	if(var_message.is_array_buffer()) {
		// load
		this->PostMessage(pp::Var("Game loading."));
		pp::VarArrayBuffer ab(var_message);
		try{
			this->vm.loadCartridge(reinterpret_cast<const uint8_t*>(ab.Map()), ab.ByteLength(), "ArrayBuffer");
			this->vm.sendHardReset();
			ab.Unmap();
			this->PostMessage(pp::Var("Game loaded."));
		} catch (EmulatorException& e) {
			this->PostMessage(pp::Var("Failed to load image: "+e.getMessage()));
			ab.Unmap();
		}
	}else if(var_message.is_string()){
		std::string msg = var_message.AsString();
		if(msg == "start"){
			this->start();
		}else if(msg == "stop"){
			this->stop();
		}
	}
}

void CycloaInstance::CallOnMainThread(PP_CompletionCallback_Func func, void* data)
{
	this->core->CallOnMainThread(0, pp::CompletionCallback(func, data));
}

void CycloaInstance::frameWait()
{
	pthread_mutex_lock(&this->frameMutex);
	pthread_cond_wait(&this->frameCond, &this->frameMutex);
	pthread_mutex_unlock(&this->frameMutex);
}

void CycloaInstance::frameLoop(void* _self, int32_t val)
{
	CycloaInstance* const self = reinterpret_cast<CycloaInstance*>(_self);
	const double now = self->core->GetTimeTicks();
	self->nextTime += CycloaInstance::kFrameInterval;
	if(self->running){
		self->core->CallOnMainThread(static_cast<uint32_t>((self->nextTime-now)*1000), pp::CompletionCallback(CycloaInstance::frameLoop, _self), (val + 1) % 60);
	}
	if(val == 0){
		const double elapsed = now-self->lastTime;
		char buff[8192];
		snprintf(buff, sizeof(buff), "FPS: %02.02f", 60/elapsed);
		self->PostMessage(buff);
		self->lastTime = now;
	}
	pthread_mutex_lock(&self->frameMutex);
	pthread_cond_signal(&self->frameCond);
	pthread_mutex_unlock(&self->frameMutex);
}

int CycloaInstance::nInstances = 0;
