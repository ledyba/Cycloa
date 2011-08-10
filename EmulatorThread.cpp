#include "EmulatorThread.h"
#include "EmulatorPanel.h"
#include <wx/log.h>

EmulatorThread::EmulatorThread(EmulatorPanel* const emulatorPanel) :
    emulatorPanel(emulatorPanel),
    initialized(false)
{
    //ctor
}

EmulatorThread::~EmulatorThread()
{
    //dtor
}

wxThread::ExitCode EmulatorThread::Entry()
{
    while(processQueue()){

    }
    return (wxThread::ExitCode)0;
}


bool EmulatorThread::processQueue(){
    EmulatorThreadMessage msg;
    bool paused = !this->initialized;
    while((paused ? msgQueue.Receive(msg) : msgQueue.ReceiveTimeout(0, msg)) != wxMSGQUEUE_TIMEOUT){
        paused = false;
        switch(msg.getSignalType()){
            case EmulatorThreadMessage::NONE:
                break;
            case EmulatorThreadMessage::START:
                this->initialized = true;
                break;
            case EmulatorThreadMessage::LOAD:
                break;
            case EmulatorThreadMessage::PAUSE:
                paused = true;
                break;
            case EmulatorThreadMessage::RESET:
                break;
            case EmulatorThreadMessage::HARD_RESET:
                break;
            case EmulatorThreadMessage::KILL:
                return false;
            default:
                throw "Invalid operation.";
        }
    }
    return true;
}


//----------------------------------------------------------

EmulatorThreadMessage::EmulatorThreadMessage(enum SignalType signalType):
    signalType(signalType)
{

}

EmulatorThreadMessage& EmulatorThreadMessage::operator=(const EmulatorThreadMessage& msg)
{
    this->signalType = msg.signalType;
    return (*this);
}

EmulatorThreadMessage::SignalType EmulatorThreadMessage::getSignalType()
{
    return this->signalType;
}

