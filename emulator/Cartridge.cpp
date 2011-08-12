#include "VirtualMachine.h"

Cartridge::Cartridge(VirtualMachine& vm, const NesFile* nesFile) :
VM(vm), nesFile(nesFile)
{

}
Cartridge::~Cartridge()
{
	if(this->nesFile){
		delete this->nesFile;
	}
}
void Cartridge::run(uint16_t clockDelta)
{
	//何もしない。
}
void Cartridge::onHSync(uint16_t scanline)
{
	//何もしない
}
void Cartridge::onVSync()
{
	//何もしない
}
void Cartridge::onHardReset()
{
	//何もしない
}
void Cartridge::onReset()
{
	//何もしない
}
uint8_t Cartridge::readVideo(uint16_t addr)
{
	return this->nesFile->readChr(static_cast<uint32_t>(addr & 0x1fff));
}
void Cartridge::writeVideo(uint16_t addr, uint8_t value)
{
	//何もしない
}
uint8_t Cartridge::readCpu(uint16_t addr)
{
    switch(addr & 0x0E00){
        case 0x4000:
        case 0x6000:
			if(this->nesFile->hasSram()){
				return readSram(addr & 0x1fff);
			}
			//http://nocash.emubase.de/everynes.htm#unpredictablethings
			return addr >> 8;
        case 0x8000:
        case 0xA000:
        case 0xC000:
        case 0xE000:
			return this->nesFile->readPrg(addr & 0x7FFF);
        default:
            throw "FIXME!!";
    }
}
void Cartridge::writeCpu(uint16_t addr, uint8_t value)
{
	if(((addr & 0x0E00) == 0x6000) && this->nesFile->hasSram()){
		writeSram(addr & 0x1fff, value);
	}
}
uint8_t Cartridge::readSram(uint16_t addr) const
{
	return this->sram[addr];
}
void Cartridge::writeSram(uint16_t addr, uint8_t value)
{
	this->sram[addr] = value;
}
