#pragma once

#include "../VirtualMachine.h"
#include "../file/NesFile.h"

class Mapper0 : public Cartridge {
public:
  Mapper0(VirtualMachine &vm, const NesFile *nesFile);

  virtual ~Mapper0();

  /* for PPU */
  uint8_t readPatternTableHigh(uint16_t addr);

  void writePatternTableHigh(uint16_t addr, uint8_t val);

  uint8_t readPatternTableLow(uint16_t addr);

  void writePatternTableLow(uint16_t addr, uint8_t val);

  /* for CPU */
  uint8_t readBankHigh(uint16_t addr);

  void writeBankHigh(uint16_t addr, uint8_t val);

  uint8_t readBankLow(uint16_t addr);

  void writeBankLow(uint16_t addr, uint8_t val);

protected:
private:
  const uint16_t addrMask;
  const bool hasChrRam;
  uint8_t chrRam[8192];
};
