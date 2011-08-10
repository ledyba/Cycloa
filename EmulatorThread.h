#ifndef EMULATORTHREAD_H_INCLUDED
#define EMULATORTHREAD_H_INCLUDED

#include "EmulatorPanel.h"
#include "emulator/VirtualMachine.h"
#include <wx/msgqueue.h>

class EmulatorThreadMessage {
    public:
        enum SignalType {
            NONE,
            START,
            LOAD,
            PAUSE,
            RESET,
            HARD_RESET,
            KILL,
        };
        explicit EmulatorThreadMessage(enum SignalType signalType = NONE);
        SignalType getSignalType();
        EmulatorThreadMessage& operator=(const EmulatorThreadMessage& msg);
    private:
        enum SignalType signalType;
};

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
        bool initialized;
        wxMessageQueue<EmulatorThreadMessage> msgQueue;
        bool processQueue();
};

#endif // EMULATORTHREAD_H_INCLUDED
