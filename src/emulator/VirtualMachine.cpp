#include <cstddef>
#include <stdio.h>
#include <fstream>
#include "exception/EmulatorException.h"
#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(VideoFairy &videoFairy, AudioFairy &audioFairy, GamepadFairy *player1,
                               GamepadFairy *player2) :
    ram(*this),
    processor(*this),
    audio(*this, audioFairy),
    video(*this, videoFairy),
    cartridge(NULL),
    ioPort(*this, player1, player2),
    clockDelta(0),
    resetFlag(false),
    hardResetFlag(false),
    irqLine(0) {
  //ctor
}

VirtualMachine::~VirtualMachine() {
  if (this->cartridge != NULL) {
    delete this->cartridge;
  }
}

void VirtualMachine::run() {
  if (this->hardResetFlag) {
    this->clockDelta = 0;
    this->hardResetFlag = false;

    this->processor.onHardReset();

    this->cartridge->onHardReset();

    this->video.onHardReset();

    this->audio.onHardReset();

    return;
  } else if (this->resetFlag) {
    this->clockDelta = 0;
    this->resetFlag = false;

    this->processor.onReset();

    this->cartridge->onReset();

    this->video.onReset();

    this->audio.onReset();

    return;
  }

  const int32_t cpuClockDelta = this->clockDelta / CPU_CLOCK_FACTOR;
  const int32_t videoClockDelta = this->clockDelta / VIDEO_CLOCK_FACTOR;
  this->clockDelta = 0;

  this->processor.run(cpuClockDelta);

  this->video.run(videoClockDelta);

  this->cartridge->run(cpuClockDelta);

  this->audio.run(cpuClockDelta);

}

void VirtualMachine::consumeClock(uint32_t clock) {
  this->clockDelta += clock;
}

void VirtualMachine::sendVBlank() {
  this->ioPort.onVBlank();
}

void VirtualMachine::sendNMI() {
  this->processor.sendNMI();
}

void VirtualMachine::reserveIRQ(uint8_t device) {
  this->irqLine |= device;
  this->processor.reserveIRQ();
}

void VirtualMachine::releaseIRQ(uint8_t device) {
  this->irqLine &= ~(device);
  if (irqLine == 0) {
    this->processor.releaseIRQ();
  }
}

bool VirtualMachine::isIRQpending(uint8_t device) {
  return (irqLine & device) == device;
}

void VirtualMachine::sendHardReset() {
  this->hardResetFlag = true;
}

void VirtualMachine::sendReset() {
  this->resetFlag = true;
}

void VirtualMachine::loadCartridge(const char *filename) {
  std::ifstream in(filename, std::fstream::binary);
  in.seekg(0, std::ifstream::end);
  const std::ifstream::pos_type endPos = in.tellg();
  in.seekg(0, std::ifstream::beg);
  const std::ifstream::pos_type startPos = in.tellg();
  const uint32_t size = static_cast<uint32_t>(endPos - startPos);

  uint8_t *const data = new uint8_t[size];
  try {
    in.read(reinterpret_cast<char *>(data), size);
    if (in.gcount() != static_cast<int32_t>(size)) {
      throw EmulatorException("[FIXME] Invalid file format: ") << filename;
    }
    VirtualMachine::loadCartridge(data, size);
    delete data;
  } catch (...) {
    delete data;
    throw;
  }
}

void VirtualMachine::loadCartridge(const uint8_t *data, const uint32_t size, const std::string &name) {
  if (this->cartridge) {
    delete this->cartridge;
  }
  this->cartridge = Cartridge::loadCartridge(*this, data, size, name);
  this->video.connectCartridge(this->cartridge);
}
