#include <cstddef>
#include <stdio.h>
#include "exception/EmulatorException.h"
#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(VideoFairy& videoFairy) :
videoFairy(videoFairy),
ram(*this),
processor(*this),
audio(*this),
video(*this, videoFairy),
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
			//このへんは込み入ってるので、仕方ないからここで振り分け。
			if(addr == 0x4015){
                return audio.readReg(addr);
			}else if(addr == 0x4016){
				return 0;
			}else if(addr == 0x4017){
				return 0;
            }else if(addr < 0x4018){
            	throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr;
            }else{
				return cartridge->readRegisterArea(addr);
            }
        case 0x6000:
            return cartridge->readSaveArea(addr);
        case 0x8000:
        case 0xA000:
            return cartridge->readBankLow(addr);
        case 0xC000:
        case 0xE000:
            return cartridge->readBankHigh(addr);
        default:
            throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr;
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
			if(addr == 0x4014){
				video.executeDMA(value);
			}else if(addr == 0x4016){
            	//joystick
            }else if(addr < 0x4018){
                audio.writeReg(addr, value);
            }else{
				cartridge->writeRegisterArea(addr, value);
            }
			break;
        case 0x6000:
            cartridge->writeSaveArea(addr, value);
            break;
        case 0x8000:
        case 0xA000:
            cartridge->writeBankLow(addr, value);
            break;
        case 0xC000:
        case 0xE000:
            cartridge->writeBankHigh(addr, value);
            break;
        default:
            throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr;
    }
}

void VirtualMachine::consumeCpuClock(uint32_t clock)
{
    consumeClock(clock * CPU_CLOCK_FACTOR);
}
void VirtualMachine::consumeAudioClock(uint32_t clock)
{
    consumeClock(clock * AUDIO_CLOCK_FACTOR);
}

void VirtualMachine::consumeClock(uint32_t clock)
{
    this->cpuClockDelta += clock;
    this->audioClockDelta += clock;
    this->videoClockDelta += clock;
    this->cartridgeClockDelta += clock;
}

void VirtualMachine::sendHBlank(uint16_t scanline)
{
    this->cartridge->onHBlank(scanline);
}
void VirtualMachine::sendVBlank()
{
	this->cartridge->onVBlank();
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
	this->video.connectCartridge(this->cartridge);
}
