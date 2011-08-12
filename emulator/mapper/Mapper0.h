#ifndef MAPPER0_H
#define MAPPER0_H

#include "../VirtualMachine.h"


class Mapper0 : public Cartridge
{
	public:
		Mapper0(VirtualMachine& vm, const NesFile* nesFile);
		virtual ~Mapper0();
        virtual uint8_t readVideo(uint16_t addr);
        virtual void writeVideo(uint16_t addr, uint8_t value);
        virtual uint8_t readCpu(uint16_t addr);
        virtual void writeCpu(uint16_t addr, uint8_t value);
	protected:
	private:
};

#endif // MAPPER0_H
