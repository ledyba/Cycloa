#include "VirtualMachine.h"
#include "mapper/Mapper0.h"
#include <string>
#include <sstream>

//ダミー実装
Cartridge::Cartridge(VirtualMachine& vm, const NesFile* nesFile) :
	nesFile(nesFile),
	VM(vm)
{
	if(nesFile == NULL){
		throw EmulatorException("NES FILE CAN'T BE NULL!");
	}
	if(nesFile->getMirrorType() == NesFile::FOUR_SCREEN){
		this->vramMirroring[0] = &this->fourScreenVram[0];
		this->vramMirroring[1] = &this->fourScreenVram[0x400];
		this->vramMirroring[2] = &this->fourScreenVram[0x800];
		this->vramMirroring[3] = &this->fourScreenVram[0xC00];
	}else{
		this->vramMirroring[0] = NULL;
		this->vramMirroring[1] = NULL;
		this->vramMirroring[2] = NULL;
		this->vramMirroring[3] = NULL;
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
void Cartridge::onHSync(uint16_t scanline)
{
	//何もしない
}
void Cartridge::onVSync()
{
	//何もしない
}
void Cartridge::onHardReset()
{
	//何もしない
}
void Cartridge::onReset()
{
	//何もしない
}
uint8_t Cartridge::readVideo(uint16_t addr) const
{
	if((addr & 0x2000) == 0x2000){
		return readNameTable(addr);
	}else{
		return readPatternTable(addr);
	}
}
void Cartridge::writeVideo(uint16_t addr, uint8_t value)
{
	if((addr & 0x2000) == 0x2000){
		return writeNameTable(addr, value);
	}else{
		return writePatternTable(addr, value);
	}
}

uint8_t Cartridge::readPatternTable(uint16_t addr) const
{
	throw EmulatorException("Unimplemented: Cartridge::readPatternTable");
}
void Cartridge::writePatternTable(uint16_t addr, uint8_t val)
{
	throw EmulatorException("Unimplemented: Cartridge::writePatternTable");
}
uint8_t Cartridge::readNameTable(uint16_t addr) const
{
	return vramMirroring[(addr >> 10) & 0x3][addr & 0x3ff];
}
void Cartridge::writeNameTable(uint16_t addr, uint8_t val)
{
	vramMirroring[(addr >> 10) & 0x3][addr & 0x3ff] = val;
}

uint8_t Cartridge::readCpu(uint16_t addr)
{
    switch(addr & 0xE000){
        case 0x4000:
        case 0x6000:
			//http://nocash.emubase.de/everynes.htm#unpredictablethings
			return addr >> 8;
        case 0x8000:
        case 0xA000:
        case 0xC000:
        case 0xE000:
			return 0xff;
        default:
            throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr;
    }
}
void Cartridge::writeCpu(uint16_t addr, uint8_t value)
{
}
uint8_t Cartridge::readSram(uint16_t addr) const
{
	return this->sram[addr];
}
void Cartridge::writeSram(uint16_t addr, uint8_t value)
{
	this->sram[addr] = value;
}

void Cartridge::connectInternalVram(uint8_t* internalVram)
{
	switch(nesFile->getMirrorType())
	{
		case NesFile::FOUR_SCREEN:
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

Cartridge* Cartridge::loadCartridge(VirtualMachine& vm, const char* filename)
{
	NesFile* const nesFile = new NesFile(filename);
	const uint8_t mapperNo = nesFile->getMapperNo();
	switch(mapperNo)
	{
		case 0x00: //mapper 0 = no mapper
			return new Mapper0(vm, nesFile);
		default:
		{
			uint32_t mapperNo32 = static_cast<uint32_t>(mapperNo);
            throw EmulatorException("Not Supported Mapper") << mapperNo32 << "!";
		}
	}
}
