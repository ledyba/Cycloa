#pragma once
/*
 * EmulatorException.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#include <string>
#include <utility>
#include <fmt/format.h>

class EmulatorException final : public std::exception {
public:
  EmulatorException() = delete;
  template <typename ...Args> explicit EmulatorException(std::string const& fmt, Args ...args)
  :EmulatorException(fmt::format(fmt, std::forward<Args>(args)...))
  {
  }
  explicit EmulatorException(std::string  msg):msg_(std::move(msg)){}
  ~EmulatorException() override = default;
private:
  std::string msg_;
public:
  [[ nodiscard ]] char const* what() const noexcept override;
};
