#ifndef MAPPER1_H
#define MAPPER1_H

#include "../VirtualMachine.h"
#include "../file/NesFile.h"

class Mapper1 : public Cartridge
{
	public:
		Mapper1(VirtualMachine& vm, const NesFile* nesFile);
		virtual ~Mapper1();
		/* for PPU */
        uint8_t readPatternTableHigh(uint16_t addr) const;
        void writePatternTableHigh(uint16_t addr, uint8_t val);

        uint8_t readPatternTableLow(uint16_t addr) const;
        void writePatternTableLow(uint16_t addr, uint8_t val);

		/* for CPU */
		uint8_t readBankHigh(uint16_t addr);
		void writeBankHigh(uint16_t addr, uint8_t val);
		uint8_t readBankLow(uint16_t addr);
		void writeBankLow(uint16_t addr, uint8_t val);
	protected:
	private:
		const bool hasChrRam;
		uint8_t chrRam[8192];

		const uint16_t lastPrgBank;
		uint8_t chrMode;
		uint8_t prgMode;

		uint8_t highChrBank;
		uint8_t lowChrBank;
		uint8_t prgBank;

		uint32_t highPrgAddrBase;
		uint32_t lowPrgAddrBase;
		uint32_t highChrAddrBase;
		uint32_t lowChrAddrBase;

		uint8_t reg;
		uint8_t regCnt;

		inline void updateBank();
};

#endif // MAPPER1_H
