#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include "IOPort.h"
#include "Processor.h"
#include "Ram.h"

class VirtualMachine
{
    public:
        VirtualMachine();
        ~VirtualMachine();
    protected:
    private:
        IOPort ioPort();
        Ram ram();
        Processor processor();
};

#endif // VIRTUALMACHINE_H
