/*
 * VRC4.cpp
 *
 *  Created on: 2011/09/16
 *      Author: psi
 */

#include "VRC4.h"

VRC4::VRC4(VirtualMachine& vm, const NesFile* nesFile):
Cartridge(vm,nesFile),
prgMask(nesFile->getPrgPageCnt() > 8 ? 31 : 15),
chrMask(nesFile->getChrPageCnt() > 16 ? 0xff : 0x7f),
prgBank0(0),
prgBank1(1),
lastPage((nesFile->getPrgSize()/PRG_BANK_SIZE)-1),
swapMode(false),
irqEnabled(false),
irqLoop(false),
irqReloadValue(0),
irqCounter(0),
clockDeltaLeft(0),
irqNextClock(0)
{
	for(int i=0;i<8;i++){
		chrBank[i] = i;
		fixChrBank(i);
	}
	fixPrgBank();
	memset(prgram, 0, 8192);
}

VRC4::~VRC4() {
}

void VRC4::run(uint16_t clockDelta)
{
	if(!irqEnabled){
		return;
	}
	clockDeltaLeft+=(clockDelta * 3);
	while(clockDeltaLeft >= irqNextClock){
		clockDeltaLeft -= irqNextClock;
		if(irqCounter == 0xff){
			irqCounter = irqReloadValue;
			this->reserveIRQ();
		}else{
			irqCounter++;
		}
	}
}

uint8_t VRC4::readPatternTableHigh(uint16_t addr) const
{
	return nesFile->readChr(chrAddrBase[(addr >> 10) & 7] | (addr & 0x3ff));
}

uint8_t VRC4::readPatternTableLow(uint16_t addr) const
{
	return nesFile->readChr(chrAddrBase[(addr >> 10) & 7] | (addr & 0x3ff));
}

uint8_t VRC4::readBankHigh(uint16_t addr)
{
	return nesFile->readPrg(prgAddrBase[(addr >> 13) & 3] | (addr & 0x1fff));
}

uint8_t VRC4::readBankLow(uint16_t addr)
{
	return nesFile->readPrg(prgAddrBase[(addr >> 13) & 3] | (addr & 0x1fff));
}

uint8_t VRC4::readSaveArea(uint16_t addr)
{
	return prgram[addr & 0x1fff];
}

void VRC4::writeSaveArea(uint16_t addr, uint8_t val)
{
	prgram[addr & 0x1fff] = val;
}
