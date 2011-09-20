//============================================================================
// Name        : Cycloa.cpp
// Author      : PSI
// Version     :
// Copyright   : 2011 PSI
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "fairy/sdl/SDLVideoFairy.h"
#include "fairy/sdl/SDLAudioFairy.h"
#include "fairy/sdl/SDLGamepadFairy.h"
#include "emulator/VirtualMachine.h"

int main(int argc, char** argv) {
	if(argc <= 1){
		std::cout << "Please input filename" << std::endl;
		return -1;
	}
	try{
		SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
		SDLVideoFairy videoFairy("Cycloa");
		SDLAudioFairy audioFairy;
		SDLGamepadInfo info;
		SDLGamepadFairy player1(info);
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
	}
	return 0;
}
