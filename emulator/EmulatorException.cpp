#include "VirtualMachine.h"

EmulatorException::EmulatorException(const char* msg):
msg(msg)
{

}

EmulatorException::EmulatorException(const std::string msg):
msg(msg)
{

}

EmulatorException::~EmulatorException()
{

}

const std::string& EmulatorException::getMessage() const
{
	return this->msg;
}
