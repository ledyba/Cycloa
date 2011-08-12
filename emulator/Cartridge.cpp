#include "VirtualMachine.h"
#include "mapper/Mapper0.h"
#include <string>
#include <sstream>

//ダミー実装
Cartridge::Cartridge(VirtualMachine& vm, const NesFile* nesFile) :
	nesFile(nesFile),
	VM(vm)
{

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
uint8_t Cartridge::readVideo(uint16_t addr)
{
	return 0xff;
}
void Cartridge::writeVideo(uint16_t addr, uint8_t value)
{
	//何もしない
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
            throw "FIXME!!";
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
			std::stringstream ss;
			ss << "Not Supported Mapper" << mapperNo << "!";
			throw ss.str();
		}
	}
}
