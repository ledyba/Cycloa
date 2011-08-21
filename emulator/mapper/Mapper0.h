#ifndef MAPPER0_H
#define MAPPER0_H

#include "../VirtualMachine.h"


class Mapper0 : public Cartridge
{
	public:
		Mapper0(VirtualMachine& vm, const NesFile* nesFile);
		virtual ~Mapper0();
        virtual uint8_t readPatternTable(uint16_t addr) const;
        virtual void writePatternTable(uint16_t addr, uint8_t val);
        virtual uint8_t readCpu(uint16_t addr);
        virtual void writeCpu(uint16_t addr, uint8_t value);
	protected:
	private:
		const uint16_t addrMask;
		const bool hasChrRam;
		uint8_t chrRam[8192];
};

#endif // MAPPER0_H
