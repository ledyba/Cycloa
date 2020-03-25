/*
 * Mapper23.cpp
 *
 *  Created on: 2011/09/18
 *      Author: psi
 */

#include "Mapper23.h"

Mapper23::Mapper23(VirtualMachine &vm, const NesFile *nesFile) :
    VRC4(vm, nesFile) {

}

Mapper23::~Mapper23() {
}

void Mapper23::writeBankHigh(uint16_t addr, uint8_t val) {
  switch (addr & 0xf00f) {
    case 0xC000:
      setChrBankLow(2, val);
      break;
    case 0xC001:
    case 0xC004:
      setChrBankHigh(2, val);
      break;
    case 0xC002:
    case 0xC008:
      setChrBankLow(3, val);
      break;
    case 0xC003:
    case 0xC00C:
      setChrBankHigh(3, val);
      break;
    case 0xD000:
      setChrBankLow(4, val);
      break;
    case 0xD001:
    case 0xD004:
      setChrBankHigh(4, val);
      break;
    case 0xD002:
    case 0xD008:
      setChrBankLow(5, val);
      break;
    case 0xD003:
    case 0xD00C:
      setChrBankHigh(5, val);
      break;
    case 0xE000:
      setChrBankLow(6, val);
      break;
    case 0xE001:
    case 0xE004:
      setChrBankHigh(6, val);
      break;
    case 0xE002:
    case 0xE008:
      setChrBankLow(7, val);
      break;
    case 0xE003:
    case 0xE00C:
      setChrBankHigh(7, val);
      break;
    case 0xF000:
      setIRQlow(val);
      break;
    case 0xF001:
    case 0xF004:
      setIRQhigh(val);
      break;
    case 0xF002:
    case 0xF008:
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

void Mapper23::writeBankLow(uint16_t addr, uint8_t val) {
  switch (addr & 0xf000) {
    case 0x8000:
      setPrgBank0(val);
      return;
    case 0xA000:
      setPrgBank1(val);
      return;
  }

  switch (addr & 0xf00f) {
    case 0x9000:
    case 0x9001:
    case 0x9004:
      setMirroring(val);
      break;
    case 0x9002:
    case 0x9008:
    case 0x9003:
    case 0x900C:
      setSwapMode((val & 2) == 2);
      break;
    case 0xB000:
      setChrBankLow(0, val);
      break;
    case 0xB001:
    case 0xB004:
      setChrBankHigh(0, val);
      break;
    case 0xB002:
    case 0xB008:
      setChrBankLow(1, val);
      break;
    case 0xB003:
    case 0xB00C:
      setChrBankHigh(1, val);
      break;
    default:
      throw EmulatorException("[Mapper23] Invalid addr!! 0x") << std::hex << addr;
  }
}
