#include "VirtualMachine.h"

Audio::Audio(VirtualMachine& vm):
    VM(vm)
{
    //ctor
}

Audio::~Audio()
{
    //dtor
}

void Audio::run(uint16_t clockDelta)
{

}

void Audio::onHardReset()
{

}
void Audio::onReset()
{

}
uint8_t Audio::readReg(uint16_t addr)
{
	#ifdef DEBUG
	if(addr != 0x4015){
		throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr << "for APU.";
	}
	#endif
	return 0;
}
void Audio::writeReg(uint16_t addr, uint8_t value)
{

}
