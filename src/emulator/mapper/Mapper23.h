#pragma once
/*
 * Mapper23.h
 *
 *  Created on: 2011/09/18
 *      Author: psi
 */

#include "board/VRC4.h"

class Mapper23 : public VRC4 {
public:
  Mapper23(VirtualMachine &vm, const NesFile *nesFile);

  virtual ~Mapper23();

  /* for CPU */
  void writeBankHigh(uint16_t addr, uint8_t val);

  void writeBankLow(uint16_t addr, uint8_t val);
};
