#include "EmulatorException.h"

EmulatorException::EmulatorException() {
}

EmulatorException::EmulatorException(std::string const& fmsg)
:msg(fmsg)
{
}

EmulatorException::EmulatorException(EmulatorException const& src)
:msg(src.getMessage())
{
}

std::string EmulatorException::getMessage() const {
  return this->msg;
}
