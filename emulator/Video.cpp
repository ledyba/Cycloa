#include "VirtualMachine.h"

Video::Video(VirtualMachine& vm):
    VM(vm)
{
    //ctor
}

Video::~Video()
{
    //dtor
}
void Video::run(uint16_t clockDelta)
{

}
void Video::onHardReset()
{

}
void Video::onReset()
{

}
uint8_t Video::readReg(uint16_t addr)
{
    const uint16_t regAddr = addr & 0x07;
}
void Video::writeReg(uint16_t addr, uint8_t value)
{
    const uint16_t regAddr = addr & 0x07;

}
uint8_t Video::read(uint16_t addr)
{

}
void Video::write(uint16_t addr, uint8_t value)
{

}
