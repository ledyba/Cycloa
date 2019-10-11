#pragma once
/*
 * SDLVideoFairy.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#include <SDL2/SDL.h>
#include "../../emulator/VirtualMachine.h"
#include "../../emulator/fairy/VideoFairy.h"

class SDLVideoFairy final : public VideoFairy {
public:
  explicit SDLVideoFairy(std::string const &windowTitle, int width = Video::screenWidth * 2,
                         int height = Video::screenHeight * 2);

  ~SDLVideoFairy() override;

  void dispatchRendering(const uint8_t (&nesBuffer)[screenHeight][screenWidth], uint8_t paletteMask) override;

  int getWidth() {
    return this->width;
  }

  int getHeight() {
    return this->width;
  }

private:
  const int width;
  const int height;
  bool isFullscreen;
  uint32_t nextTime;

  uint32_t fpsTime;
  uint32_t fpsCnt;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *tex;
protected:
  void dispatchRenderingImpl(const uint8_t (&nesBuffer)[screenHeight][screenWidth], uint8_t paletteMask,
                             SDL_Renderer *renderer, SDL_Texture *tex);
};
