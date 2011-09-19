/*
 * Mapper25.cpp
 *
 *  Created on: 2011/09/18
 *      Author: psi
 */

#include "Mapper25.h"

Mapper25::Mapper25(VirtualMachine& vm, const NesFile* nesFile):
VRC4(vm, nesFile)
{

}
Mapper25::~Mapper25()
{
}

void Mapper25::writeBankHigh(uint16_t addr, uint8_t val)
{
	switch(addr & 0xf00f)
	{
	case 0xC000:
		setChrBankLow(2,val);
		break;
	case 0xC002:
	case 0xC008:
		setChrBankHigh(2,val);
		break;
	case 0xC001:
	case 0xC004:
		setChrBankLow(3,val);
		break;
	case 0xC003:
	case 0xC00C:
		setChrBankHigh(3,val);
		break;
	case 0xD000:
		setChrBankLow(4,val);
		break;
	case 0xD002:
	case 0xD008:
		setChrBankHigh(4,val);
		break;
	case 0xD001:
	case 0xD004:
		setChrBankLow(5,val);
		break;
	case 0xD003:
	case 0xD00C:
		setChrBankHigh(5,val);
		break;
	case 0xE000:
		setChrBankLow(6,val);
		break;
	case 0xE002:
	case 0xE008:
		setChrBankHigh(6,val);
		break;
	case 0xE001:
	case 0xE004:
		setChrBankLow(7,val);
		break;
	case 0xE003:
	case 0xE00C:
		setChrBankHigh(7,val);
		break;
	case 0xF000:
		setIRQlow(val);
		break;
	case 0xF002:
	case 0xF008:
		setIRQhigh(val);
		break;
	case 0xF001:
	case 0xF004:
		setIRQmode(val);
		break;
	case 0xF003:
	case 0xF00C:
		ackIRQ();
		break;
	default:
		throw EmulatorException("[Mapper23] Invalid addr!! 0x") << std::hex << addr;
	}
}

void Mapper25::writeBankLow(uint16_t addr, uint8_t val)
{
	switch(addr & 0xf00f)
	{
	case 0x8000:
	case 0x8002:
	case 0x8008:
	case 0x8001:
	case 0x8004:
	case 0x8003:
	case 0x800C:
		setPrgBank0(val);
		break;
	case 0x9000:
	case 0x9002:
	case 0x9008:
		setMirroring(val);
		break;
	case 0x9001:
	case 0x9004:
	case 0x9003:
	case 0x900C:
		setSwapMode((val & 2) == 2);
		break;
	case 0xA000:
	case 0xA002:
	case 0xA008:
	case 0xA001:
	case 0xA004:
	case 0xA003:
	case 0xA00C:
		setPrgBank1(val);
		break;
	case 0xB000:
		setChrBankLow(0,val);
		break;
	case 0xB002:
	case 0xB008:
		setChrBankHigh(0,val);
		break;
	case 0xB001:
	case 0xB004:
		setChrBankLow(1,val);
		break;
	case 0xB003:
	case 0xB00C:
		setChrBankHigh(1,val);
		break;
	default:
		throw EmulatorException("[Mapper23] Invalid addr!! 0x") << std::hex << addr;
	}
}
