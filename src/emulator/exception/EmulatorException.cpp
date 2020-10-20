#include "EmulatorException.h"

char const* EmulatorException::what() const {
  return this->msg_.c_str();
}
