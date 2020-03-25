#include <cstddef>
#include <stdio.h>
#include <filesystem>
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

namespace {

std::vector<uint8_t> readAllFromFile(std::string const &fileName) noexcept(false) {
  std::uintmax_t const fileSize = std::filesystem::file_size(fileName);
  FILE *const file = fopen(fileName.c_str(), "rb");
  if (!file) {
    std::error_code err = std::make_error_code(static_cast<std::errc>(errno));
    throw std::filesystem::filesystem_error("Error to open file", fileName, err);
  }
  std::vector<uint8_t> dat;
  dat.resize(fileSize);
  size_t readed = fread(dat.data(), 1, fileSize, file);
  if (readed < fileSize) {
    std::error_code err = std::make_error_code(static_cast<std::errc>(errno));
    fclose(file);
    throw std::filesystem::filesystem_error("Error to read all contents from the file", fileName, err);
  }
  fclose(file);
  return dat;
}

}

void VirtualMachine::loadCartridge(std::string const& filename) {
  VirtualMachine::loadCartridge(std::move(readAllFromFile(filename)), filename);
}

void VirtualMachine::loadCartridge(std::vector<uint8_t> data, const std::string &name) {
  if(this->cartridge) {
    delete this->cartridge;
  }
  this->cartridge = Cartridge::loadCartridge(*this, std::move(data), name);
  this->video.connectCartridge(this->cartridge);
}
