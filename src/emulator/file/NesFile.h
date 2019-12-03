#pragma once

/*
 * NesFile.h
 *
 *  Created on: 2011/08/23
 *	  Author: psi
 */

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include <vector>

class NesFile {
public:
  enum MirrorType {
    SINGLE0, SINGLE1, HORIZONTAL, VERTICAL, FOUR_SCREEN
  };
  enum {
    NES_FORMAT_SIZE = 16,
    TRAINER_SIZE = 512,
    PRG_ROM_PAGE_SIZE = 16 * 1024,
    CHR_ROM_PAGE_SIZE = 8 * 1024,
  };
public:
  explicit NesFile(std::vector<uint8_t> data, const std::string &name = "MEMORY");

  ~NesFile();

  inline MirrorType getMirrorType() const {
    return this->mirrorType;
  }

  inline bool hasTrainer() const {
    return this->trainerFlag;
  }

  inline bool hasSram() const {
    return this->sramFlag;
  }

  inline uint8_t getMapperNo() const {
    return this->mapperNo;
  }

  inline uint32_t getPrgSize() const {
    return this->prgSize;
  }

  inline uint32_t getPrgPageCnt() const {
    return this->prgPageCnt;
  }

  inline uint32_t getChrSize() const {
    return this->chrSize;
  }

  inline uint32_t getChrPageCnt() const {
    return this->chrPageCnt;
  }

  inline uint8_t readTrainer(uint16_t addr) const {
    return this->trainer[addr];
  }

  inline uint8_t readPrg(uint32_t addr) const {
    return this->prgRom[addr];
  }

  inline uint8_t readChr(uint32_t addr) const {
    return this->chrRom[addr];
  }

private:
  const std::string filename;
  uint8_t mapperNo;
  const uint8_t *prgRom;
  const uint8_t *chrRom;
  uint8_t trainer[512];

  MirrorType mirrorType;
  bool trainerFlag;
  bool sramFlag;
  uint32_t prgSize;
  uint32_t chrSize;
  uint32_t prgPageCnt;
  uint32_t chrPageCnt;

  void analyzeFile(const uint8_t *const header, const uint32_t filesize, const uint8_t *data);
};
