#include "Mapper1.h"

Mapper1::Mapper1(VirtualMachine& vm, const NesFile* nesFile) :
Cartridge(vm, nesFile),
hasChrRam(nesFile->getChrPageCnt() == 0),
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
	memset(chrRam, 0, 8192);
}
Mapper1::~Mapper1()
{
}

/* for PPU */
uint8_t Mapper1::readPatternTableHigh(uint16_t addr) const
{
	if(hasChrRam){
		return chrRam[(addr & 0x0fff) | highChrAddrBase];
	}else{
		return this->nesFile->readChr((addr & 0x0fff) | highChrAddrBase);
	}
}

void Mapper1::writePatternTableHigh(uint16_t addr, uint8_t val)
{
	if(hasChrRam){
		chrRam[(addr & 0x0fff) | highChrAddrBase] = val;
	}
}

uint8_t Mapper1::readPatternTableLow(uint16_t addr) const
{
	if(hasChrRam){
		return chrRam[(addr & 0x0fff) | lowChrAddrBase];
	}else{
		return this->nesFile->readChr((addr & 0x0fff) | lowChrAddrBase);
	}
}

void Mapper1::writePatternTableLow(uint16_t addr, uint8_t val)
{
	if(hasChrRam){
		chrRam[(addr & 0x0fff) | lowChrAddrBase] = val;
	}
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
	reg |= (val & 1) << (regCnt);
	regCnt++;
	if(regCnt >= 5){
		switch(addr & 0xe000){
			case 0xc000:
				highChrBank = reg & 31;
				break;
			case 0xe000:
				prgBank = reg & 15;
				break;
			default:
				throw EmulatorException("[Mapper1] FIXME BUG!!");
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
	reg |= (val & 1) << (regCnt);
	regCnt++;
	if(regCnt >= 5){
		switch(addr & 0xe000){
			case 0x8000:
			{
				const uint8_t mirroring = reg & 3;
				if(mirroring == 0){
					changeMirrorType(NesFile::SINGLE0);
				}else if(mirroring == 1){
					changeMirrorType(NesFile::SINGLE1);
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
				this->lowChrBank = reg & 31;
				break;
			default:
				throw EmulatorException("[Mapper1] FIXME BUG!!");
		}
		updateBank();
		regCnt = 0;
		reg = 0;
	}
}

inline void Mapper1::updateBank()
{
	if(chrMode == 1){ //4kb mode
		if(hasChrRam){
			highChrAddrBase = 0x1000 * (highChrBank & 1);
			lowChrAddrBase = 0x1000 * (lowChrBank & 1);
		}else{
			highChrAddrBase = 0x1000 * highChrBank;
			lowChrAddrBase = 0x1000 * lowChrBank;
		}
	}else{ // 8kb mode
		if(hasChrRam){
			highChrAddrBase = 0x1000;
			lowChrAddrBase = 0x0000;
		}else{
			lowChrAddrBase = 0x1000 * (lowChrBank & 30);
			highChrAddrBase = lowChrAddrBase+0x1000;
		}
	}
	if(prgMode < 2){
		lowPrgAddrBase = (this->prgBank & 14) * NesFile::PRG_ROM_PAGE_SIZE;
		highPrgAddrBase = lowPrgAddrBase + NesFile::PRG_ROM_PAGE_SIZE;
	}else if(prgMode == 2){
		lowPrgAddrBase = 0;
		highPrgAddrBase = this->prgBank * NesFile::PRG_ROM_PAGE_SIZE;
	}else if(prgMode == 3){
		lowPrgAddrBase = this->prgBank * NesFile::PRG_ROM_PAGE_SIZE;
		highPrgAddrBase = this->lastPrgBank * NesFile::PRG_ROM_PAGE_SIZE;
	}
}
