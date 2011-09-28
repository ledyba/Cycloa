#include "exception/EmulatorException.h"
#include "VirtualMachine.h"
#include "mapper/Mapper0.h"
#include "mapper/Mapper1.h"
#include "mapper/Mapper2.h"
#include "mapper/Mapper3.h"
#include "mapper/Mapper4.h"
#include "mapper/Mapper21.h"
#include "mapper/Mapper23.h"
#include "mapper/Mapper25.h"
#include <string>
#include <sstream>

//ダミー実装
Cartridge::Cartridge(VirtualMachine& vm, const NesFile* nesFile) :
	nesFile(nesFile),
	VM(vm),
	hasSram(nesFile->hasSram()),
	mirrorType(nesFile->getMirrorType()),
    internalVram(NULL)
{
	if(nesFile == NULL){
		throw EmulatorException("NES FILE CAN'T BE NULL!");
	}
}
Cartridge::~Cartridge()
{
	if(this->nesFile){
		delete this->nesFile;
	}
}
void Cartridge::run(uint16_t clockDelta)
{
	//何もしない。
}
void Cartridge::onHardReset()
{
	//何もしない
}
void Cartridge::onReset()
{
	//何もしない
}

uint8_t Cartridge::readRegisterArea(uint16_t addr)
{
	//http://nocash.emubase.de/everynes.htm#unpredictablethings
	return addr >> 8;
}
void Cartridge::writeRegisterArea(uint16_t addr, uint8_t val)
{
	//have no effect.
}

uint8_t Cartridge::readSaveArea(uint16_t addr)
{
	return readSram(addr);
}
void Cartridge::writeSaveArea(uint16_t addr, uint8_t val)
{
	writeSram(addr, val);
}

void Cartridge::writePatternTableHigh(uint16_t addr, uint8_t val)
{
}

void Cartridge::writePatternTableLow(uint16_t addr, uint8_t val)
{
}
void Cartridge::writeBankLow(uint16_t addr, uint8_t val)
{
}
void Cartridge::writeBankHigh(uint16_t addr, uint8_t val)
{
}

uint8_t Cartridge::readNameTable(uint16_t addr)
{
	return vramMirroring[(addr >> 10) & 0x3][addr & 0x3ff];
}
void Cartridge::writeNameTable(uint16_t addr, uint8_t val)
{
	vramMirroring[(addr >> 10) & 0x3][addr & 0x3ff] = val;
}

void Cartridge::connectInternalVram(uint8_t* internalVram)
{
	this->internalVram = internalVram;
	this->changeMirrorType(this->mirrorType);
}

void Cartridge::changeMirrorType(NesFile::MirrorType mirrorType)
{
	this->mirrorType = mirrorType;
	switch(mirrorType)
	{
	case NesFile::SINGLE0:
		this->vramMirroring[0] = &internalVram[0];
		this->vramMirroring[1] = &internalVram[0];
		this->vramMirroring[2] = &internalVram[0];
		this->vramMirroring[3] = &internalVram[0];
		break;
	case NesFile::SINGLE1:
		this->vramMirroring[0] = &internalVram[0x400];
		this->vramMirroring[1] = &internalVram[0x400];
		this->vramMirroring[2] = &internalVram[0x400];
		this->vramMirroring[3] = &internalVram[0x400];
		break;
	case NesFile::FOUR_SCREEN:
		this->vramMirroring[0] = &this->fourScreenVram[0];
		this->vramMirroring[1] = &this->fourScreenVram[0x400];
		this->vramMirroring[2] = &this->fourScreenVram[0x800];
		this->vramMirroring[3] = &this->fourScreenVram[0xC00];
		break;
	case NesFile::HORIZONTAL:
		this->vramMirroring[0] = &internalVram[0];
		this->vramMirroring[1] = &internalVram[0];
		this->vramMirroring[2] = &internalVram[0x400];
		this->vramMirroring[3] = &internalVram[0x400];
		break;
	case NesFile::VERTICAL:
		this->vramMirroring[0] = &internalVram[0];
		this->vramMirroring[1] = &internalVram[0x400];
		this->vramMirroring[2] = &internalVram[0];
		this->vramMirroring[3] = &internalVram[0x400];
		break;
	default:
		throw EmulatorException("Invalid mirroring type!");
	}
}

void Cartridge::reserveIRQ()
{
	VM.reserveIRQ(VirtualMachine::DEVICE_CARTRIDGE);
}
void Cartridge::releaseIRQ()
{
	VM.releaseIRQ(VirtualMachine::DEVICE_CARTRIDGE);
}


Cartridge* Cartridge::loadCartridge(VirtualMachine& vm, const char* filename)
{
	NesFile* nesFile = NULL;
	try{
		nesFile = new NesFile(filename);
		const uint8_t mapperNo = nesFile->getMapperNo();
		switch(mapperNo)
		{
			case 0: //mapper 0 = no mapper
				return new Mapper0(vm, nesFile);
			case 1: //mapper 1 = MMC1
				return new Mapper1(vm, nesFile);
			case 2: //mapper 2 = UxROM
				return new Mapper2(vm, nesFile);
			case 3: //mapper 3 = CNROM
				return new Mapper3(vm, nesFile);
			case 4: //mapper 4 = MMC3
				return new Mapper4(vm, nesFile);
			case 21: //mapper 21 = VRC4ac
				return new Mapper21(vm, nesFile);
			case 23: //mapper 23 = VRC4e
				return new Mapper23(vm, nesFile);
			case 25: //mapper 25 = VRC4bd
				return new Mapper25(vm, nesFile);
			default:
			{
				const uint32_t mapperNo32 = static_cast<uint32_t>(mapperNo);
				throw EmulatorException("Not Supported Mapper: ") << mapperNo32 << "!";
			}
		}
	}catch(...){
		if(nesFile != NULL){
			delete nesFile;
		}
		throw;
	}
	return NULL;
}
