#pragma once
/*
 * Mapper25.h
 *
 *  Created on: 2011/09/18
 *      Author: psi
 */

#include "board/VRC4.h"

class Mapper25 : public VRC4 {
public:
  Mapper25(VirtualMachine &vm, const NesFile *nesFile);

  virtual ~Mapper25();

  /* for CPU */
  void writeBankHigh(uint16_t addr, uint8_t val);

  void writeBankLow(uint16_t addr, uint8_t val);
};

#pragma once
