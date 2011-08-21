#include "VirtualMachine.h"

EmulatorException::EmulatorException()
{
}
EmulatorException::EmulatorException(const char* fmsg)
{
	this->msg << fmsg;
}
EmulatorException::EmulatorException(const std::string& fmsg)
{
	this->msg << fmsg;
}

EmulatorException::EmulatorException(const EmulatorException& src)
{
	this->msg << src.msg.str();
}
EmulatorException::~EmulatorException()
{

}

const std::string EmulatorException::getMessage() const
{
	return this->msg.str();
}
