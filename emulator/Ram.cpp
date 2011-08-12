#include "VirtualMachine.h"

Ram::Ram(VirtualMachine& vm):
    VM(vm)
{
    //ctor
}

Ram::~Ram()
{
    //dtor
}

void Ram::onHardReset()
{

}
void Ram::onReset()
{

}
uint8_t Ram::read(uint16_t addr)
{
    return wram[addr & 0x7ff];
}
void Ram::write(uint16_t addr, uint8_t value)
{
    wram[addr & 0x7ff] = value;
}
