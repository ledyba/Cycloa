/*
 * Mapper2.h
 *
 *  Created on: 2011/09/13
 *      Author: psi
 */

#ifndef MAPPER2_H_
#define MAPPER2_H_

#include "../VirtualMachine.h"
#include "../file/NesFile.h"

class Mapper2 : public Cartridge
{
public:
	Mapper2(VirtualMachine& vm, const NesFile* nesFile);
	virtual ~Mapper2();
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
	uint8_t vram[8192];
	uint32_t lowBankAddrBase;
	const uint32_t highBankAddrBase;
};

#endif /* MAPPER2_H_ */
