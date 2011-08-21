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
        SignalType getSignalType() const;
        const std::string& getFilename() const;
        void setFilename(const std::string& filename);
        EmulatorThreadMessage& operator=(const EmulatorThreadMessage& msg);
    private:
        enum SignalType signalType;
        std::string filename;
};

class EmulatorThread : public wxThread, public EmulatorFairy
{
    public:
        explicit EmulatorThread(EmulatorPanel* const emulatorPanel);
        virtual ~EmulatorThread();

		void sendStartSignal();
		void sendPauseSignal();
		void sendResetSignal();
		void sendHardResetSignal();
		void sendKillSignal();
        void sendLoadCartridgeSignal(const std::string& filename);
		virtual void onVSync(const ImageBuffer& img);
    protected:
        virtual ExitCode Entry();
    private:
        EmulatorPanel* const emulatorPanel;
        VirtualMachine vm;
        bool initialized;
        bool videoOuted;
        wxMessageQueue<EmulatorThreadMessage> msgQueue;
        bool processQueue();
};


#endif // EMULATORTHREAD_H_INCLUDED
