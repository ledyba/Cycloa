#include "VirtualMachine.h"

VirtualMachine::VirtualMachine() :
ram(*this),
processor(*this),
audio(*this),
video(*this),
cartridge(NULL)
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
        this->cpuClockDelta = 0;

        this->cartridge->onHardReset();
        this->cartridgeClockDelta = 0;

        this->video.onHardReset();
        this->videoClockDelta = 0;

        this->audio.onHardReset();
        this->audioClockDelta = 0;

        return;
    }else if(this->resetFlag){
        this->cpuClockDelta = 0;
        this->cartridgeClockDelta = 0;
        this->videoClockDelta = 0;
        this->audioClockDelta = 0;
        this->resetFlag = false;

        this->processor.onReset();
        this->cpuClockDelta = 0;

        this->cartridge->onReset();
        this->cartridgeClockDelta = 0;

        this->video.onReset();
        this->videoClockDelta = 0;

        this->audio.onReset();
        this->audioClockDelta = 0;

        return;
    }
    this->processor.run(this->cpuClockDelta);
    this->cpuClockDelta = 0;

    this->cartridge->run(this->cartridgeClockDelta);
    this->cartridgeClockDelta = 0;

    this->video.run(this->videoClockDelta);
    this->videoClockDelta = 0;
void VirtualMachine::sendHardReset()
    this->audio.run(this->audioClockDelta);
    this->audioClockDelta = 0;
}
uint8_t VirtualMachine::read(uint16_t addr)
{
    switch(addr & 0x0E00){
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
            return cartridge->read(addr);
        default:
            throw "FIXME!!";
    }
}
void VirtualMachine::write(uint16_t addr, uint8_t value)
{
    switch(addr & 0x0E00){
        case 0x0000:
            return ram.write(addr, value);
        case 0x2000:
            return video.writeReg(addr, value);
        case 0x4000:
            if(addr < 0x4018){
                return audio.writeReg(addr, value);
            }
        case 0x6000:
        case 0x8000:
        case 0xA000:
        case 0xC000:
        case 0xE000:
            return cartridge->write(addr, value);
        default:
            throw "FIXME!!";
    }
}

void VirtualMachine::consumeClock(uint8_t clock)
{
    this->cpuClockDelta += clock;
    this->audioClockDelta += clock;
    this->videoClockDelta += clock;
    this->cartridgeClockDelta += clock;
}

void VirtualMachine::sendScanlineEnd(uint16_t scanline)
{
    this->cartridge->onScanlineEnd(scanline);
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
    this->resetFlag = true;
}
void VirtualMachine::sendReset()
{
    this->hardResetFlag = true;
}
