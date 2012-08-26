#include <iostream>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
#include <ppapi/cpp/var_array_buffer.h>
#include <ppapi/utility/completion_callback_factory.h>
#include "NACLVideoFairy.h"
#include "NACLAudioFairy.h"
#include "NACLGamepadFairy.h"
#include "../../emulator/VirtualMachine.h"

class CycloaModule;
class CycloaInstance;

class CycloaInstance : public pp::Instance
{
private:
	static int nInstances;
	pthread_t gameMainThread;
	int width;
	int height;
	bool running;
private:
	pp::InstanceHandle handle;
	NACLVideoFairy videoFairy;
	NACLAudioFairy audioFairy;
	NACLGamepadFairy gamepadFairy;
	DummyGamepadFairy dummyPlayer;
	VirtualMachine vm;
	std::string error;
public:
	explicit CycloaInstance(PP_Instance instance)
	: pp::Instance(instance),
	gameMainThread(NULL),
	width(0), height(0),
	running(false),
	handle(this),
	videoFairy(handle),
	audioFairy(handle),
	gamepadFairy(handle),
	dummyPlayer(),
	vm(this->videoFairy, this->audioFairy, &this->gamepadFairy, &this->dummyPlayer),
	error("")
	{
		this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_KEYBOARD);
		nInstances++;
		assert(nInstances == 1);
	}
	virtual ~CycloaInstance()
	{
		if(gameMainThread) {
			pthread_join(gameMainThread, NULL);
		}
	}
public:
	virtual bool HandleInputEvent(const pp::InputEvent& event)
	{
		return this->gamepadFairy.transInputEvent(event);
	}

	void* start(){
		try{
			this->running = true;
			while(this->running){
				this->vm.run();
			}
			this->error="";
		} catch (EmulatorException& e){
			this->error = e.getMessage();
		}
		return 0;
	}
	static void* start(void* ptr){
		return reinterpret_cast<CycloaInstance*>(ptr)->start();
	}
	virtual void HandleMessage(const pp::Var& var_message)
	{
		if(var_message.is_array_buffer()) {
			// load
			pp::VarArrayBuffer ab(var_message);
			this->vm.loadCartridge(reinterpret_cast<const uint8_t*>(ab.Map()), ab.ByteLength(), "ArrayBuffer");
			this->vm.sendHardReset();
			ab.Unmap();
			this->PostMessage(pp::Var("Loaded."));
		}else if(var_message.is_string()){
			std::string msg = var_message.AsString();
			if(msg == "start"){
				pthread_create(&this->gameMainThread, NULL, CycloaInstance::start,this);
				this->PostMessage(pp::Var("Start running."));
			}else if(msg == "stop"){
				this->running = false;
				pthread_join(gameMainThread, NULL);
				if(!this->error.empty()){
					this->PostMessage(this->error);
				}
				this->gameMainThread = 0;
			}
		}
	}
	virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip)
	{

	}
};

int CycloaInstance::nInstances = 0;

class CycloaModule : public pp::Module {
public:
	CycloaModule() : pp::Module() {}
	virtual ~CycloaModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new CycloaInstance(instance);
	}
};

namespace pp {

Module* CreateModule() {
	return new CycloaModule();
}

}

/*
int game_main(int argc, char** argv) {
	if(argc <= 1){
		std::cout << "Please input filename" << std::endl;
		return -1;
	}
	try{
		SDL_Init(SDL_INIT_VIDEO);
		NACLVideoFairy videoFairy("Cycloa");
		NACLAudioFairy audioFairy;
		NACLGamepadInfo info;
		NACLGamepadFairy player1(info);
		VirtualMachine vm(videoFairy, audioFairy, &player1, 0);
		vm.loadCartridge(argv[1]);
		vm.sendHardReset();
		while(true){
			vm.run();
		}
		SDL_Quit();
	}catch(EmulatorException& e){
		std::cerr << "Error: " << e.getMessage();
		std::cerr << std::endl;
		std::cerr.flush();
		return -1;
	} catch (...) {
		std::cerr << "Unknown Error...";
		std::cerr << std::endl;
		std::cerr.flush();
		return -2;
	}
	return 0;
}
*/
