/*
 * Mapper4.cpp
 *
 *  Created on: 2011/09/20
 *      Author: psi
 */

#include "Mapper4.h"

Mapper4::Mapper4(VirtualMachine &vm, const NesFile *nesFile) :
    Cartridge(vm, nesFile),
    mode(0),
    modePrg(false),
    modeChr(false),
    haveChrRam(nesFile->getChrPageCnt() == 0),
    lastPrgBank((nesFile->getPrgSize() / Mapper4::PRG_BANK_SIZE) - 1),
    irqEnabled(false),
    irqLatchValue(0),
    irqCounter(0),
    beforeLow(false) {
  memset(chrRam, 0, 8192);
  modeBanks[0] = 0;
  modeBanks[1] = 2;
  modeBanks[2] = 4;
  modeBanks[3] = 5;
  modeBanks[4] = 6;
  modeBanks[5] = 7;

  modeBanks[6] = 0;
  modeBanks[7] = 1;

  fixBank();
}

Mapper4::~Mapper4() {
}

uint8_t Mapper4::readPatternTableHigh(uint16_t addr) {
  if (beforeLow) {
    processIRQ();
    beforeLow = false;
  }
  if (haveChrRam) {
    return chrRam[addr & 0x1fff];
  } else {
    return nesFile->readChr(chrAddrMask[(addr >> 10) & 7] | (addr & 0x3ff));
  }
}

void Mapper4::writePatternTableHigh(uint16_t addr, uint8_t val) {
  chrRam[addr & 0x1fff] = val;
}

uint8_t Mapper4::readPatternTableLow(uint16_t addr) {
  beforeLow = true;
  if (haveChrRam) {
    return chrRam[addr & 0x1fff];
  } else {
    return nesFile->readChr(chrAddrMask[(addr >> 10) & 7] | (addr & 0x3ff));
  }
}

void Mapper4::writePatternTableLow(uint16_t addr, uint8_t val) {
  chrRam[addr & 0x1fff] = val;
}


uint8_t Mapper4::readBankHigh(uint16_t addr) {
  return nesFile->readPrg(prgAddrMask[(addr >> 13) & 3] | (addr & 0x1fff));
}

void Mapper4::writeBankHigh(uint16_t addr, uint8_t val) {
  switch (addr & 0xe001) {
    case 0xC000:
      irqLatchValue = val;
      break;
    case 0xC001:
      irqCounter = 0;
      break;
    case 0xE000:
      irqEnabled = false;
      releaseIRQ();
      break;
    case 0xE001:
      irqEnabled = true;
      break;
  }
}

uint8_t Mapper4::readBankLow(uint16_t addr) {
  return nesFile->readPrg(prgAddrMask[(addr >> 13) & 3] | (addr & 0x1fff));
}

void Mapper4::writeBankLow(uint16_t addr, uint8_t val) {
  switch (addr & 0xe001) {
    case 0x8000:
      mode = val & 7;
      modePrg = (val & 0x40) == 0x40;
      modeChr = (val & 0x80) == 0x80;
      fixBank();
      break;
    case 0x8001:
      modeBanks[mode] = val;
      fixBank();
      break;
    case 0xA000:
      changeMirrorType(((val & 1) == 1) ? NesFile::HORIZONTAL : NesFile::VERTICAL);
      break;
    case 0xA001:
      //PRG RAM protect ($A001-$BFFF, odd)
      //FIXME: not implemented!
      break;
  }
}

void Mapper4::fixBank() {
  if (modeChr) {
    chrAddrMask[0] = Mapper4::CHR_BANK_SIZE * modeBanks[2];
    chrAddrMask[1] = Mapper4::CHR_BANK_SIZE * modeBanks[3];
    chrAddrMask[2] = Mapper4::CHR_BANK_SIZE * modeBanks[4];
    chrAddrMask[3] = Mapper4::CHR_BANK_SIZE * modeBanks[5];

    chrAddrMask[4] = Mapper4::CHR_BANK_SIZE * modeBanks[0];
    chrAddrMask[5] = Mapper4::CHR_BANK_SIZE * (modeBanks[0] + 1);
    chrAddrMask[6] = Mapper4::CHR_BANK_SIZE * modeBanks[1];
    chrAddrMask[7] = Mapper4::CHR_BANK_SIZE * (modeBanks[1] + 1);
  } else {
    chrAddrMask[0] = Mapper4::CHR_BANK_SIZE * modeBanks[0];
    chrAddrMask[1] = Mapper4::CHR_BANK_SIZE * (modeBanks[0] + 1);
    chrAddrMask[2] = Mapper4::CHR_BANK_SIZE * modeBanks[1];
    chrAddrMask[3] = Mapper4::CHR_BANK_SIZE * (modeBanks[1] + 1);

    chrAddrMask[4] = Mapper4::CHR_BANK_SIZE * modeBanks[2];
    chrAddrMask[5] = Mapper4::CHR_BANK_SIZE * modeBanks[3];
    chrAddrMask[6] = Mapper4::CHR_BANK_SIZE * modeBanks[4];
    chrAddrMask[7] = Mapper4::CHR_BANK_SIZE * modeBanks[5];
  }
  if (modePrg) {
    prgAddrMask[0] = Mapper4::PRG_BANK_SIZE * (lastPrgBank - 1);
    prgAddrMask[2] = Mapper4::PRG_BANK_SIZE * modeBanks[6];
  } else {
    prgAddrMask[0] = Mapper4::PRG_BANK_SIZE * modeBanks[6];
    prgAddrMask[2] = Mapper4::PRG_BANK_SIZE * (lastPrgBank - 1);
  }
  prgAddrMask[1] = Mapper4::PRG_BANK_SIZE * modeBanks[7];
  prgAddrMask[3] = Mapper4::PRG_BANK_SIZE * lastPrgBank;
}

void Mapper4::processIRQ() {
  if (irqCounter == 0) {
    irqCounter = irqLatchValue;
  } else {
    irqCounter--;
    if (irqEnabled && irqCounter == 0) {
      reserveIRQ();
    }
  }
}


