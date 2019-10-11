#pragma once
/*
 * EmulatorException.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#include <iostream>
#include <string>
#include <sstream>

class EmulatorException final : public std::exception {
public:
  EmulatorException();

  explicit EmulatorException(const char *fmsg);

  explicit EmulatorException(const std::string &fmsg);

  EmulatorException(EmulatorException const &src);

  ~EmulatorException() override = default;

private:
  std::stringstream msg;
public:
  const std::string getMessage() const;

  template<typename T>
  EmulatorException &operator<<(T &val) {
    this->msg << val;
    return *this;
  }
};
