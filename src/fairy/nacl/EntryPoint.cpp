#include <iostream>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/var.h>
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
private:
	NACLVideoFairy* videoFairy;
	NACLAudioFairy* audioFairy;
	NACLGamepadFairy* gamepadFairy;
public:
	explicit CycloaInstance(PP_Instance instance)
	: pp::Instance(instance),
	gameMainThread(NULL),
	width(0), height(0)
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
		if(this->videoFairy){
			delete this->videoFairy;
		}
		this->videoFairy = 0;
		if(this->audioFairy){
			delete this->audioFairy;
		}
		this->audioFairy = 0;
		if(this->gamepadFairy){
			delete this->gamepadFairy;
		}
		this->gamepadFairy = 0;
	}
public:
	virtual bool Init(uint32_t argc, const char* argn[], const char* argv[])
	{
		
	}
	virtual bool HandleInputEvent(const pp::InputEvent& event)
	{

	}
	virtual void HandleMessage(const pp::Var& var_message)
	{
		
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
