#include "Mapper0.h"
#include <stdio.h>
#include <string.h>

Mapper0::Mapper0(VirtualMachine& vm, const NesFile* nesFile) :
Cartridge(vm, nesFile),
// 16KBなら、同じ内容が繰り返される
addrMask(nesFile->getPrgPageCnt() > 1 ? 0x7fff : 0x3fff),
hasChrRam(nesFile->getChrPageCnt() == 0)
{
	if(hasChrRam){
		memset(chrRam, 0, 8192);
	}
}
Mapper0::~Mapper0()
{
}

/* for PPU */
uint8_t Mapper0::readPatternTableHigh(uint16_t addr) const
{
	if(this->hasChrRam){
		return this->chrRam[addr & 0x1fff];
	}else{
		return this->nesFile->readChr(addr & 0x1fff);
	}
}

void Mapper0::writePatternTableHigh(uint16_t addr, uint8_t val)
{
	if(this->hasChrRam){
		this->chrRam[addr & 0x1fff] = val;
	}
}

uint8_t Mapper0::readPatternTableLow(uint16_t addr) const
{
	if(this->hasChrRam){
		return this->chrRam[addr & 0x1fff];
	}else{
		return this->nesFile->readChr(addr & 0x1fff);
	}
}
void Mapper0::writePatternTableLow(uint16_t addr, uint8_t val)
{
	if(this->hasChrRam){
		this->chrRam[addr & 0x1fff] = val;
	}
}

/* for CPU */
uint8_t Mapper0::readBankHigh(uint16_t addr)
{
	return this->nesFile->readPrg(addr & addrMask);
}
void Mapper0::writeBankHigh(uint16_t addr, uint8_t val)
{
	//have no effect
}
uint8_t Mapper0::readBankLow(uint16_t addr)
{
	return this->nesFile->readPrg(addr & addrMask);
}
void Mapper0::writeBankLow(uint16_t addr, uint8_t val)
{
	//have no effect
}

