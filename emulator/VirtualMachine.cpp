#include "VirtualMachine.h"

VirtualMachine::VirtualMachine() :
ioPort(*this),
ram(*this),
processor(*this)
{
    //ctor
}

VirtualMachine::~VirtualMachine()
{
}
