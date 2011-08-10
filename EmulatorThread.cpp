#include "EmulatorThread.h"
#include "EmulatorPanel.h"
#include <wx/log.h>

EmulatorThread::EmulatorThread(EmulatorPanel* const emulatorPanel) :
    emulatorPanel(emulatorPanel)
{
    //ctor
}

EmulatorThread::~EmulatorThread()
{
    //dtor
}

wxThread::ExitCode EmulatorThread::Entry()
{
    while(true){
    }
    return (wxThread::ExitCode)0;
}
