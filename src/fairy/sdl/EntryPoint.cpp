//============================================================================
// Name        : Cycloa.cpp
// Author      : PSI
// Version     :
// Copyright   : 2011 PSI
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <SDL.h>
#include "SDLVideoFairy.h"
#include "SDLAudioFairy.h"
#include "SDLGamepadFairy.h"
#include "../../emulator/VirtualMachine.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cout << "Please input filename" << std::endl;
    return -1;
  }
  try {
    // FIXME: こうしないとここで落ちる
    // https://github.com/SDL-mirror/SDL/blob/9f39b048121dc3dbff573c0b189d4424d514d0c7/src/video/SDL_video.c#L186-L242
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "false");
    //
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDLVideoFairy videoFairy("Cycloa");
    SDLAudioFairy audioFairy;
    SDLGamepadInfo info;
    SDLGamepadFairy player1(info);
    VirtualMachine vm(videoFairy, audioFairy, &player1, nullptr);
    vm.loadCartridge(argv[1]);
    vm.sendHardReset();
    while (true) {
      vm.run();
    }
    SDL_Quit();
  } catch (EmulatorException &e) {
    std::cerr << "Error: " << e.getMessage();
    std::cerr << std::endl;
    std::cerr.flush();
    return -1;
  } catch (std::exception &e) {
    std::cerr << "Standard Error: " << e.what();
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
