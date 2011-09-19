/*
 * Mapper21.cpp
 *
 *  Created on: 2011/09/18
 *      Author: psi
 */

#include "Mapper21.h"

Mapper21::Mapper21(VirtualMachine& vm, const NesFile* nesFile):
VRC4(vm, nesFile)
{
}

Mapper21::~Mapper21()
{
}

void Mapper21::writeBankHigh(uint16_t addr, uint8_t val)
{
	switch(addr & 0xf0C6)
	{
	case 0xC000:
		setChrBankLow(2,val);
		break;
	case 0xC002:
	case 0xC040:
		setChrBankHigh(2,val);
		break;
	case 0xC004:
	case 0xC080:
		setChrBankLow(3,val);
		break;
	case 0xC006:
	case 0xC0C0:
		setChrBankHigh(3,val);
		break;
	case 0xD000:
		setChrBankLow(4,val);
		break;
	case 0xD002:
	case 0xD040:
		setChrBankHigh(4,val);
		break;
	case 0xD004:
	case 0xD080:
		setChrBankLow(5,val);
		break;
	case 0xD006:
	case 0xD0C0:
		setChrBankHigh(5,val);
		break;
	case 0xE000:
		setChrBankLow(6,val);
		break;
	case 0xE002:
	case 0xE040:
		setChrBankHigh(6,val);
		break;
	case 0xE004:
	case 0xE080:
		setChrBankLow(7,val);
		break;
	case 0xE006:
	case 0xE0C0:
		setChrBankHigh(7,val);
		break;
	case 0xF000:
		setIRQlow(val);
		break;
	case 0xF002:
	case 0xF040:
		setIRQhigh(val);
		break;
	case 0xF004:
	case 0xF080:
		setIRQmode(val);
		break;
	case 0xF006:
	case 0xF0C0:
		ackIRQ();
		break;
	default:
		throw EmulatorException("[Mapper23] Invalid addr!! 0x") << std::hex << addr;
	}
}

void Mapper21::writeBankLow(uint16_t addr, uint8_t val)
{
	switch(addr & 0xf00f)
	{
	case 0x8000:
	case 0x8002:
	case 0x8040:
	case 0x8004:
	case 0x8080:
	case 0x8006:
	case 0x80C0:
		setPrgBank0(val);
		break;
	case 0x9000:
	case 0x9002:
	case 0x9040:
		setMirroring(val);
		break;
	case 0x9004:
	case 0x9080:
	case 0x9006:
	case 0x90C0:
		setSwapMode((val & 2) == 2);
		break;
	case 0xA000:
	case 0xA002:
	case 0xA040:
	case 0xA004:
	case 0xA080:
	case 0xA006:
	case 0xA0C0:
		setPrgBank1(val);
		break;
	case 0xB000:
		setChrBankLow(0,val);
		break;
	case 0xB002:
	case 0xB040:
		setChrBankHigh(0,val);
		break;
	case 0xB004:
	case 0xB080:
		setChrBankLow(1,val);
		break;
	case 0xB006:
	case 0xB0C0:
		setChrBankHigh(1,val);
		break;
	default:
		throw EmulatorException("[Mapper23] Invalid addr!! 0x") << std::hex << addr;
	}
}
