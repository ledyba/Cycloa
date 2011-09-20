/*
 * VRC4.h
 *
 *  Created on: 2011/09/16
 *      Author: psi
 */

#ifndef VRC4_H_
#define VRC4_H_

#include "../../VirtualMachine.h"

class VRC4 : public Cartridge
{
public:
	VRC4(VirtualMachine& vm, const NesFile* nesFile);
	virtual ~VRC4();
protected:
	enum PageSize {
		PRG_BANK_SIZE = 8192,
		CHR_BANK_SIZE = 1024
	};
	inline void setSwapMode(bool laterBankIsControlled)
	{
		swapMode = laterBankIsControlled;
		fixPrgBank();
	}
	inline void setPrgBank0(uint8_t val)
	{
		prgBank0 = val & prgMask;
		fixPrgBank();
	}
	inline void setPrgBank1(uint8_t val)
	{
		prgBank1 = val & prgMask;
		fixPrgBank();
	}
	inline void setChrBankLow(uint8_t idx, uint8_t val)
	{
		chrBank[idx] = (chrBank[idx] & 0xf0) | (val & 0xf);
		fixChrBank(idx);
	}
	inline void setChrBankHigh(uint8_t idx, uint8_t val)
	{
		chrBank[idx] = (chrBank[idx] & 0xf) | ((val & 0xf) << 4);
		fixChrBank(idx);
	}
	inline void setMirroring(uint8_t val){
		switch(val & 3){
		case 0:
			this->changeMirrorType(NesFile::VERTICAL);
			break;
		case 1:
			this->changeMirrorType(NesFile::HORIZONTAL);
			break;
		case 2:
			this->changeMirrorType(NesFile::SINGLE0);
			break;
		case 3:
			this->changeMirrorType(NesFile::SINGLE1);
			break;
		}
	}

	inline void setIRQlow(uint8_t val)
	{
		irqReloadValue = (irqReloadValue & 0xf0) | (val & 0xf);
	}
	inline void setIRQhigh(uint8_t val)
	{
		irqReloadValue = (irqReloadValue & 0xf) | ((val & 0xf) << 4);
	}
	inline void setIRQmode(uint8_t val)
	{
		irqLoop = (val & 1) == 1;
		irqEnabled = (val & 2) == 2;
		if(irqEnabled){
			irqCounter = irqReloadValue;
			clockDeltaLeft = 0;
		}
		irqNextClock = (val & 4) == 4 ? 3 : 341;
		//
		this->releaseIRQ();
	}
	inline void ackIRQ()
	{
		this->releaseIRQ();
		irqEnabled = irqLoop;
	}

	void run(uint16_t clockDelta);

	/* for PPU */
    uint8_t readPatternTableHigh(uint16_t addr) const;
    uint8_t readPatternTableLow(uint16_t addr) const;

	/* for CPU */
	uint8_t readBankHigh(uint16_t addr);
	uint8_t readBankLow(uint16_t addr);

	uint8_t readSaveArea(uint16_t addr);
	void writeSaveArea(uint16_t addr, uint8_t val);

private:
	inline void fixPrgBank()
	{
		if(swapMode){
			prgAddrBase[0] = (lastPage-1) * PRG_BANK_SIZE;
			prgAddrBase[2] = prgBank0 * PRG_BANK_SIZE;
		}else{
			prgAddrBase[0] = prgBank0 * PRG_BANK_SIZE;
			prgAddrBase[2] = (lastPage-1) * PRG_BANK_SIZE;
		}
		prgAddrBase[1] = prgBank1 * PRG_BANK_SIZE;
		prgAddrBase[3] = lastPage * PRG_BANK_SIZE;
	}
	inline void fixChrBank(uint8_t idx)
	{
		chrAddrBase[idx] = (chrBank[idx] & chrMask) * CHR_BANK_SIZE;
	}
	uint32_t chrAddrBase[8];
	uint32_t prgAddrBase[4];
	uint8_t chrBank[8];
	const uint8_t prgMask;
	const uint8_t chrMask;
	uint8_t prgBank0;
	uint8_t prgBank1;
	const uint8_t lastPage;
	bool swapMode;

	uint8_t prgram[8192];

	bool irqEnabled;
	bool irqLoop;
	uint8_t irqReloadValue;
	uint8_t irqCounter;

	int32_t clockDeltaLeft;
	int32_t irqNextClock;
};

#endif /* VRC4_H_ */
