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
  explicit EmulatorException(const std::string &fmsg);
  explicit EmulatorException(EmulatorException const &src);

  ~EmulatorException() override = default;

private:
  std::string msg;
public:
  [[ nodiscard ]] std::string getMessage() const;

  template<typename T>
  EmulatorException &operator<<(T &val) {
    this->msg << val;
    return *this;
  }
};
