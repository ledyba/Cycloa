//============================================================================
// Name        : Cycloa.cpp
// Author      : PSI
// Version     :
// Copyright   : 2011 PSI
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "fairy/sdl/SDLVideoFairy.h"
#include "emulator/VirtualMachine.h"

int main(int argc, char** argv) {
	if(argc <= 1){
		std::cout << "Please input filename";
		return -1;
	}
	try{
		SDLVideoFairy videoFairy("Cycloa");
		VirtualMachine vm(videoFairy);
		vm.loadCartridge(argv[1]);
		vm.sendHardReset();
		while(true){
			vm.run();
		}
	}catch(EmulatorException& e){
		std::cerr << "Error: " << e.getMessage() << std::endl;
	}
	return 0;
}
