/*
 * Mapper4.h
 *
 *  Created on: 2011/09/20
 *      Author: psi
 */

#ifndef MAPPER4_H_
#define MAPPER4_H_

#include "../VirtualMachine.h"

class Mapper4 : public Cartridge
{
public:
	Mapper4(VirtualMachine& vm, const NesFile* nesFile);
	virtual ~Mapper4();
	/* for PPU */
    uint8_t readPatternTableHigh(uint16_t addr);
	void writePatternTableHigh(uint16_t addr, uint8_t val);
    uint8_t readPatternTableLow(uint16_t addr);
	void writePatternTableLow(uint16_t addr, uint8_t val);

	/* for CPU */
	uint8_t readBankHigh(uint16_t addr);
	void writeBankHigh(uint16_t addr, uint8_t val);
	uint8_t readBankLow(uint16_t addr);
	void writeBankLow(uint16_t addr, uint8_t val);
private:
	enum{
		CHR_BANK_SIZE = 1024,
		PRG_BANK_SIZE = 8192
	};

	uint8_t mode;
	bool modePrg;
	bool modeChr;
	uint16_t modeBanks[8];

	bool haveChrRam;
	uint8_t chrRam[8192];
	uint32_t lastPrgBank;

	bool irqEnabled;
	uint8_t irqLatchValue;
	uint8_t irqCounter;

	uint32_t prgAddrMask[4];
	uint32_t chrAddrMask[8];

	bool beforeLow;
	void fixBank();
	void processIRQ();
};

#endif /* MAPPER4_H_ */
