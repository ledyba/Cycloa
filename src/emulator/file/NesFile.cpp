
#include <fstream>
#include <string.h>
#include "./NesFile.h"
#include "../exception/EmulatorException.h"

NesFile::NesFile(const uint8_t* data, const uint32_t size, const std::string& name) :
filename(name),
mapperNo(0),
prgRom(NULL),
chrRom(NULL),
mirrorType(HORIZONTAL),
trainerFlag(false),
sramFlag(false),
prgSize(0),
chrSize(0),
prgPageCnt(0),
chrPageCnt(0)
{
	const uint32_t contentSize = size - NES_FORMAT_SIZE;
	this->analyzeFile(data, contentSize, &data[NES_FORMAT_SIZE]);
}

NesFile::~NesFile()
{
	if(this->chrRom){
		delete [] this->chrRom;
	}
	if(this->prgRom){
		delete [] this->prgRom;
	}
}


void NesFile::analyzeFile(const uint8_t* const header, const uint32_t filesize, const uint8_t* data)
{
	if(!(header[0] == 'N' && header[1]=='E' && header[2]=='S' && header[3] == 0x1a)){
		throw EmulatorException("[FIXME] Invalid header: ") << filename;
	}
	this->prgSize = PRG_ROM_PAGE_SIZE * header[4];
	this->chrSize = CHR_ROM_PAGE_SIZE * header[5];
	this->prgPageCnt = header[4];
	this->chrPageCnt = header[5];
	this->mapperNo = ((header[6] & 0xf0)>>4) | (header[7] & 0xf0);
	this->trainerFlag = (header[6] & 0x4) == 0x4;
	this->sramFlag = (header[6] & 0x2) == 0x2;
	if((header[6] & 0x8) == 0x8){
		this->mirrorType = FOUR_SCREEN;
	}else{
		this->mirrorType = (header[6] & 0x1) == 0x1 ? VERTICAL : HORIZONTAL;
	}

	uint32_t fptr = 0;
	if(this->trainerFlag){
		if(fptr + TRAINER_SIZE > filesize){
			throw EmulatorException("[FIXME] Invalid file size; too short!: ") << filename;
		}
		memcpy(this->trainer, &data[fptr], TRAINER_SIZE);
		fptr += TRAINER_SIZE;
	}
	uint8_t* prgRom = new uint8_t[this->prgSize];
	if(fptr + this->prgSize > filesize){
		throw EmulatorException("[FIXME] Invalid file size; too short!: ") << filename;
	}
	memcpy(prgRom, &data[fptr], this->prgSize);
	fptr += this->prgSize;
	this->prgRom = prgRom;

	uint8_t* chrRom = new uint8_t[this->chrSize];
	if(fptr + this->chrSize > filesize){
		throw EmulatorException("[FIXME] Invalid file size; too short!: ") << filename;
	}else if(fptr + this->chrSize < filesize){
		throw EmulatorException("[FIXME] Invalid file size; too long!: ") << filename;
	}
	memcpy(chrRom, &data[fptr], this->chrSize);
	fptr += this->chrSize;
	this->chrRom = chrRom;
}
