#include "EmulatorException.h"

char const* EmulatorException::what() const noexcept {
  return this->msg_.c_str();
}
