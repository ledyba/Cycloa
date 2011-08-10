#include "VirtualMachine.h"

IOPort::IOPort(VirtualMachine& vm):
    VM(vm)
{
    //ctor
}

IOPort::~IOPort()
{
    //dtor
}

uint8_t IOPort::read(uint16_t addr)
{
    switch(addr & 0x0E00){
        case 0x0000:
            //return this->VM.ram.read8(addr &= 0x7ff);
        default:
            return 0x00;
    }
}
void IOPort::write(uint16_t addr, uint8_t value)
{

}
