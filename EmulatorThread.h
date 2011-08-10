#ifndef EMULATORTHREAD_H_INCLUDED
#define EMULATORTHREAD_H_INCLUDED

#include "EmulatorPanel.h"
#include "emulator/VirtualMachine.h"

class EmulatorThread : public wxThread
{
    public:
        explicit EmulatorThread(EmulatorPanel* const emulatorPanel);
        virtual ~EmulatorThread();
    protected:
        virtual ExitCode Entry();
    private:
        EmulatorPanel* const emulatorPanel;
        VirtualMachine vm;
};

#endif // EMULATORTHREAD_H_INCLUDED
