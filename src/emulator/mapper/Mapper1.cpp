#include "Mapper1.h"

Mapper1::Mapper1(VirtualMachine& vm, const NesFile* nesFile) :
Cartridge(vm, nesFile),
lastPrgBank(nesFile->getPrgPageCnt()-1),
chrMode(0),
prgMode(3),

highChrBank(1),
lowChrBank(0),
prgBank(lastPrgBank),

highPrgAddrBase(lastPrgBank * NesFile::PRG_ROM_PAGE_SIZE),
lowPrgAddrBase(0),
highChrAddrBase(0x1000),
lowChrAddrBase(0x0000),
reg(0),
regCnt(0)
{
}
Mapper1::~Mapper1()
{
}

/* for PPU */
uint8_t Mapper1::readPatternTableHigh(uint16_t addr) const
{
	return this->nesFile->readChr((addr & 0x0fff) | highChrAddrBase);
}

uint8_t Mapper1::readPatternTableLow(uint16_t addr) const
{
	return this->nesFile->readChr((addr & 0x0fff) | lowChrAddrBase);
}

/* for CPU */
uint8_t Mapper1::readBankHigh(uint16_t addr)
{
	return this->nesFile->readPrg(highPrgAddrBase | (addr & 0x3fff));
}
void Mapper1::writeBankHigh(uint16_t addr, uint8_t val)
{
	if((val & 0x80) == 0x80){
		regCnt = 0;
		reg = 0;
		return;
	}
	reg = (val & 1) << (regCnt);
	regCnt++;
	if(regCnt >= 5){
		switch(addr & 0xe000){
			case 0xc000:
				lowChrBank = reg & 31;
				break;
			case 0xe000:
				highChrBank = reg & 31;
				break;
		}
		updateBank();
		regCnt = 0;
		reg = 0;
	}
}
uint8_t Mapper1::readBankLow(uint16_t addr)
{
	return this->nesFile->readPrg(lowPrgAddrBase | (addr & 0x3fff));
}
void Mapper1::writeBankLow(uint16_t addr, uint8_t val)
{
	if((val & 0x80) == 0x80){
		regCnt = 0;
		reg = 0;
		return;
	}
	reg = (val & 1) << (regCnt);
	regCnt++;
	if(regCnt >= 5){
		switch(addr & 0xe000){
			case 0x8000:
			{
				const uint8_t mirroring = reg & 3;
				if(mirroring < 2){
					changeMirrorType(NesFile::SINGLE);
				}else if(mirroring == 2){
					changeMirrorType(NesFile::VERTICAL);
				}else if(mirroring == 3){
					changeMirrorType(NesFile::HORIZONTAL);
				}
				this->prgMode = (reg>>2) & 3;
				this->chrMode = (reg >> 4) & 1;
				break;
			}
			case 0xa000:
				this->prgBank = reg & 31;
				break;
		}
		updateBank();
		regCnt = 0;
		reg = 0;
	}
}

inline void Mapper1::updateBank()
{
	if(chrMode == 1){
		highChrAddrBase = 0x1000 * highChrBank;
		lowChrAddrBase = 0x1000 * lowChrBank;
	}else{
		lowChrAddrBase = 0x1000 * lowChrBank;
		highChrAddrBase += 0x1000;
	}
	if(prgMode < 2){
		lowPrgAddrBase = this->prgBank * NesFile::PRG_ROM_PAGE_SIZE;
		highPrgAddrBase = lowPrgAddrBase + NesFile::PRG_ROM_PAGE_SIZE;
	}else if(prgMode == 2){
		lowPrgAddrBase = this->prgBank * NesFile::PRG_ROM_PAGE_SIZE;
		highPrgAddrBase = lastPrgBank * NesFile::PRG_ROM_PAGE_SIZE;
	}else if(prgMode == 3){
		lowPrgAddrBase = 0;
		highPrgAddrBase = this->prgBank * NesFile::PRG_ROM_PAGE_SIZE;
	}

}
