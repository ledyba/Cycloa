#include <cstddef>
#include "VirtualMachine.h"

VirtualMachine::VirtualMachine() :
ram(*this),
processor(*this),
audio(*this),
video(*this),
cartridge(NULL),
resetFlag(false),
hardResetFlag(false)
{
    //ctor
}

VirtualMachine::~VirtualMachine()
{
    if(this->cartridge != NULL){
        delete this->cartridge;
    }
}
void VirtualMachine::run()
{
    if(this->hardResetFlag){
        this->cpuClockDelta = 0;
        this->cartridgeClockDelta = 0;
        this->videoClockDelta = 0;
        this->audioClockDelta = 0;
        this->hardResetFlag = false;

        this->processor.onHardReset();
        this->cpuClockDelta %= CPU_CLOCK_FACTOR;

        this->cartridge->onHardReset();
        this->cartridgeClockDelta %= CARTRIDGE_CLOCK_FACTOR;

        this->video.onHardReset();
        this->videoClockDelta %= VIDEO_CLOCK_FACTOR;

        this->audio.onHardReset();
        this->audioClockDelta %= AUDIO_CLOCK_FACTOR;

        return;
    }else if(this->resetFlag){
        this->cpuClockDelta = 0;
        this->cartridgeClockDelta = 0;
        this->videoClockDelta = 0;
        this->audioClockDelta = 0;
        this->resetFlag = false;

        this->processor.onReset();
        this->cpuClockDelta %= CPU_CLOCK_FACTOR;

        this->cartridge->onReset();
        this->cartridgeClockDelta %= CARTRIDGE_CLOCK_FACTOR;

        this->video.onReset();
        this->videoClockDelta %= VIDEO_CLOCK_FACTOR;

        this->audio.onReset();
        this->audioClockDelta %= AUDIO_CLOCK_FACTOR;

        return;
    }
    this->processor.run(this->cpuClockDelta / CPU_CLOCK_FACTOR);
    this->cpuClockDelta %= CPU_CLOCK_FACTOR;

    this->cartridge->run(this->cartridgeClockDelta / CARTRIDGE_CLOCK_FACTOR);
    this->cartridgeClockDelta %= CARTRIDGE_CLOCK_FACTOR;

    this->video.run(this->videoClockDelta / VIDEO_CLOCK_FACTOR);
    this->videoClockDelta %= VIDEO_CLOCK_FACTOR;

    this->audio.run(this->audioClockDelta / AUDIO_CLOCK_FACTOR);
    this->audioClockDelta %= AUDIO_CLOCK_FACTOR;
}
uint8_t VirtualMachine::read(uint16_t addr)
{
    switch(addr & 0xE000){
        case 0x0000:
            return ram.read(addr);
        case 0x2000:
            return video.readReg(addr);
        case 0x4000:
            if(addr < 0x4018){
                return audio.readReg(addr);
            }
        case 0x6000:
        case 0x8000:
        case 0xA000:
        case 0xC000:
        case 0xE000:
            //Cartridges
            return cartridge->readCpu(addr);
        default:
            throw "FIXME!!";
    }
}
void VirtualMachine::write(uint16_t addr, uint8_t value)
{
    switch(addr & 0xE000){
        case 0x0000:
            ram.write(addr, value);
            break;
        case 0x2000:
            video.writeReg(addr, value);
            break;
        case 0x4000:
            if(addr < 0x4018){
                audio.writeReg(addr, value);
				break;
            }
        case 0x6000:
        case 0x8000:
        case 0xA000:
        case 0xC000:
        case 0xE000:
            cartridge->writeCpu(addr, value);
            break;
        default:
            throw "FIXME!!";
    }
}

void VirtualMachine::consumeCpuClock(uint8_t clock)
{
    consumeClock(clock * CPU_CLOCK_FACTOR);
}
void VirtualMachine::consumeAudioClock(uint8_t clock)
{
    consumeClock(clock * AUDIO_CLOCK_FACTOR);
}

void VirtualMachine::consumeClock(uint8_t clock)
{
    this->cpuClockDelta += clock;
    this->audioClockDelta += clock;
    this->videoClockDelta += clock;
    this->cartridgeClockDelta += clock;
}

void VirtualMachine::sendHSync(uint16_t scanline)
{
    this->cartridge->onHSync(scanline);
}

void VirtualMachine::sendVideoOut()
{

}
void VirtualMachine::sendNMI()
{
    this->processor.sendNMI();
}

void VirtualMachine::sendIRQ()
{
    this->processor.sendIRQ();
}

void VirtualMachine::sendHardReset()
{
    this->hardResetFlag = true;
}
void VirtualMachine::sendReset()
{
    this->resetFlag = true;
}

void VirtualMachine::loadCartridge(const char* filename)
{
	if(this->cartridge){
		delete this->cartridge;
	}
	this->cartridge = Cartridge::loadCartridge(*this, filename);
}
