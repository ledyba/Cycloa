#include "Mapper0.h"

Mapper0::Mapper0(VirtualMachine& vm, const NesFile* nesFile) :
Cartridge(vm, nesFile),
// 16KBなら、同じ内容が繰り返される
addrMask((nesFile->getPrgSize()/NesFile::PRG_ROM_PAGE_SIZE) > 1 ? 0x7fff : 0x3fff)
{
}
Mapper0::~Mapper0()
{
}
uint8_t Mapper0::readVideo(uint16_t addr)
{
	return this->nesFile->readChr(static_cast<uint32_t>(addr & 0x1fff));
}
void Mapper0::writeVideo(uint16_t addr, uint8_t value)
{
	//何もしない
}
uint8_t Mapper0::readCpu(uint16_t addr)
{
    switch(addr & 0xE000){
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
			return this->nesFile->readPrg(addr & this->addrMask);
        default:
            throw "FIXME!!";
    }
}
void Mapper0::writeCpu(uint16_t addr, uint8_t value)
{
	if(((addr & 0xE000) == 0x6000) && this->nesFile->hasSram()){
		writeSram(addr & 0x1fff, value);
	}
}
