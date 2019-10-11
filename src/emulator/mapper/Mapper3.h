#pragma once

/*
 * Mapper3.h
 *
 *  Created on: 2011/09/05
 *	  Author: psi
 */

#include "../VirtualMachine.h"
#include "../file/NesFile.h"

class Mapper3 : public Cartridge {
public:
  Mapper3(VirtualMachine &vm, const NesFile *nesFile);

  virtual ~Mapper3();

  /* for PPU */
  uint8_t readPatternTableHigh(uint16_t addr);

  uint8_t readPatternTableLow(uint16_t addr);

  /* for CPU */
  uint8_t readBankHigh(uint16_t addr);

  void writeBankHigh(uint16_t addr, uint8_t val);

  uint8_t readBankLow(uint16_t addr);

  void writeBankLow(uint16_t addr, uint8_t val);

private:
  inline void writeReg(uint16_t addr, uint8_t val);

  const uint32_t prgHighBankAddrBase;
  uint32_t chrBankAddrBase;
};
