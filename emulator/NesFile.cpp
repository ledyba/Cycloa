
#include <cstddef>
#include <fstream>
#include <string.h>
#include "VirtualMachine.h"

NesFile::NesFile(const char* filename) :
filename(filename),
prgRom(NULL),
chrRom(NULL)
{
    std::ifstream in(filename, std::fstream::binary);
    in.seekg(0, std::ifstream::end);
    const std::ifstream::pos_type endPos = in.tellg();
    in.seekg(0, std::ifstream::beg);
    const std::ifstream::pos_type startPos = in.tellg();
    const uint32_t size = static_cast<uint32_t>(endPos - startPos);

    if(size <= NES_FORMAT_SIZE){
        throw EmulatorException("[FIXME] Invalid file format: ") << filename;
    }

    //read header
    uint8_t header[NES_FORMAT_SIZE];
    in.read(reinterpret_cast<char*>(header), NES_FORMAT_SIZE);
    if(in.gcount() != NES_FORMAT_SIZE){
        throw EmulatorException("[FIXME] Invalid file: ") << filename;
    }
    //left
    const int32_t contentSize = size-NES_FORMAT_SIZE;
    uint8_t data[contentSize];
    in.read(reinterpret_cast<char*>(data), contentSize);
    if(in.gcount() != contentSize){
        throw EmulatorException("[FIXME] Invalid file: ") << filename;
    }

    this->analyzeFile(header, contentSize, data);
}
NesFile::NesFile(const uint32_t filesize, const uint8_t* data) :
filename("\\ON_MEMORY"),
prgRom(NULL),
chrRom(NULL)
{
    const uint32_t contentSize = filesize - NES_FORMAT_SIZE;
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

NesFile::MirrorType NesFile::getMirrorType() const
{
    return this->mirrorType;
}

bool NesFile::hasTrainer() const
{
    return this->trainerFlag;
}
bool NesFile::hasSram() const
{
    return this->sramFlag;
}
uint8_t NesFile::getMapperNo() const
{
    return this->mapperNo;
}
uint32_t NesFile::getPrgSize() const
{
    return this->prgSize;
}
uint32_t NesFile::getChrSize() const
{
    return this->chrSize;
}
uint8_t NesFile::readTrainer(uint16_t addr) const
{
    return this->trainer[addr & (TRAINER_SIZE-1)];
}
uint8_t NesFile::readPrg(uint16_t page, uint16_t addr) const
{
    return this->readPrg(page * PRG_ROM_PAGE_SIZE + addr);
}
uint8_t NesFile::readPrg(uint32_t addr) const
{
    return this->prgRom[addr];
}
uint8_t NesFile::readChr(uint16_t page, uint16_t addr) const
{
    return this->readChr(page * PRG_ROM_PAGE_SIZE + addr);
}
uint8_t NesFile::readChr(uint32_t addr) const
{
    return this->chrRom[addr];
}

void NesFile::analyzeFile(const uint8_t* const header, const uint32_t filesize, const uint8_t* data)
{
    if(!(header[0] == 'N' && header[1]=='E' && header[2]=='S' && header[3] == 0x1a)){
        throw EmulatorException("[FIXME] Invalid header: ") << filename;
    }
    this->prgSize = PRG_ROM_PAGE_SIZE * header[4];
    this->chrSize = CHR_ROM_PAGE_SIZE * header[5];
    this->mapperNo = (header[6] & 0xf) | (header[7] & 0xf0);
    this->trainerFlag = (header[6] & 0b100) == 0b100;
    this->sramFlag = (header[6] & 0b10) == 0b10;
    if((header[6] & 0b1000) == 0b1000){
        this->mirrorType = FOUR_SCREEN;
    }else{
        this->mirrorType = (header[6] & 0b1) == 0b1 ? VERTICAL : HORIZONTAL;
    }

    uint32_t fptr = 0;
    if(this->trainerFlag){
        if(fptr + TRAINER_SIZE > filesize){
            throw EmulatorException("[FIXME] Invalid file: ") << filename;
        }
        memcpy(this->trainer, &data[fptr], TRAINER_SIZE);
        fptr += TRAINER_SIZE;
    }
    uint8_t* prgRom = new uint8_t[this->prgSize];
    if(fptr + this->prgSize > filesize){
        throw EmulatorException("[FIXME] Invalid file: ") << filename;
    }
    memcpy(prgRom, &data[fptr], this->prgSize);
    fptr += this->prgSize;
    this->prgRom = prgRom;

    uint8_t* chrRom = new uint8_t[this->prgSize];
    if(fptr + this->chrSize > filesize){
        throw EmulatorException("[FIXME] Invalid file: ") << filename;
    }
    memcpy(chrRom, &data[fptr], this->chrSize);
    fptr += this->chrSize;
    this->chrRom = chrRom;
}
