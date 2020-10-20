#pragma once
/*
 * EmulatorException.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#include <string>
#include <fmt/format.h>

class EmulatorException final : public std::exception {
public:
  EmulatorException();
  template <typename ...Args> explicit EmulatorException(std::string const& fmt, Args ...args)
  : EmulatorException(fmt::format(fmt, std::forward<Args>(args)...)){

  }
  explicit EmulatorException(std::string const& msg):msg_(msg){}
  EmulatorException(EmulatorException const& other):msg_(other.msg_){}

  ~EmulatorException() override = default;

private:
  std::string msg_;
public:
  [[ nodiscard ]] char const* what() const override;
};
