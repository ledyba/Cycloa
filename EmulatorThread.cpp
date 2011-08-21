#include "EmulatorThread.h"
#include "EmulatorPanel.h"
#include <wx/log.h>

EmulatorThread::EmulatorThread(EmulatorPanel* const emulatorPanel) :
	wxThread(),
	EmulatorFairy(),
    emulatorPanel(emulatorPanel),
    initialized(false),
    videoOuted(false)
{
    this->vm.setFairy(this);
}

EmulatorThread::~EmulatorThread()
{
}

wxThread::ExitCode EmulatorThread::Entry()
{
//	wxLog* logger = new wxLogStream(&std::cout);
//    wxLog::SetActiveTarget(logger);
	try {
		while(processQueue()){
			wxDateTime beforeRun = wxDateTime::Now();
			if(this->initialized){
				while(!this->videoOuted){
					vm.run();
				}
				wxLongLong timeDelta = (wxDateTime::Now()-beforeRun).GetMilliseconds();
				if(timeDelta < 16){
					wxThread::Sleep(15-timeDelta.ToLong());
				}
				this->videoOuted = false;
			}
		}
		return (wxThread::ExitCode)0;
	} catch(EmulatorException& msg) {
        std::string emsg = msg.getMessage();
		wxMessageBox(emsg, "Error");
		return (wxThread::ExitCode)-1;
	} catch (...){
		wxMessageBox("Uncaught exception", "Error");
		return (wxThread::ExitCode)-1;
	}
	return (wxThread::ExitCode)0;
}


bool EmulatorThread::processQueue(){
    EmulatorThreadMessage msg;
    bool paused = !this->initialized;
    while((paused ? msgQueue.Receive(msg) : msgQueue.ReceiveTimeout(0, msg)) != wxMSGQUEUE_TIMEOUT){
        paused = false;
        EmulatorThreadMessage::SignalType signalType = msg.getSignalType();
        switch(signalType){
            case EmulatorThreadMessage::NONE:
                break;
            case EmulatorThreadMessage::START:
                this->initialized = true;
                this->vm.sendHardReset();
                break;
            case EmulatorThreadMessage::LOAD:
				this->vm.loadCartridge(msg.getFilename().c_str());
                break;
            case EmulatorThreadMessage::PAUSE:
                paused = true;
                break;
            case EmulatorThreadMessage::RESET:
                this->vm.sendReset();
                break;
            case EmulatorThreadMessage::HARD_RESET:
                this->vm.sendHardReset();
                break;
            case EmulatorThreadMessage::KILL:
                return false;
            default:
                throw EmulatorException("[FIXME] Invalid message: 0x") << std::hex << signalType;
        }
    }
    return true;
}

void EmulatorThread::sendLoadCartridgeSignal(const std::string& filename)
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::LOAD);
	msg.setFilename(filename);
	this->msgQueue.Post(msg);
}

void EmulatorThread::sendStartSignal()
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::START);
	this->msgQueue.Post(msg);
}
void EmulatorThread::sendPauseSignal()
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::PAUSE);
	this->msgQueue.Post(msg);
}
void EmulatorThread::sendResetSignal()
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::RESET);
	this->msgQueue.Post(msg);
}
void EmulatorThread::sendHardResetSignal()
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::HARD_RESET);
	this->msgQueue.Post(msg);
}
void EmulatorThread::sendKillSignal()
{
	EmulatorThreadMessage msg(EmulatorThreadMessage::KILL);
	this->msgQueue.Post(msg);
}

void EmulatorThread::onVSync(const ImageBuffer& img)
{
	this->emulatorPanel->update(img.getPtr(), img.getActualWidth(), img.getActualHeight(), img.getWidth(), img.getHeight(), img.getBytesPerPixel());
	this->videoOuted = true;
}

//----------------------------------------------------------

EmulatorThreadMessage::EmulatorThreadMessage(enum SignalType signalType):
    signalType(signalType),
    filename()
{

}

EmulatorThreadMessage& EmulatorThreadMessage::operator=(const EmulatorThreadMessage& msg)
{
    this->signalType = msg.signalType;
    this->filename = msg.filename;
    return (*this);
}

EmulatorThreadMessage::SignalType EmulatorThreadMessage::getSignalType() const
{
    return this->signalType;
}

const std::string& EmulatorThreadMessage::getFilename() const
{
	return this->filename;
}

void EmulatorThreadMessage::setFilename(const std::string& filename){
	this->filename = filename;
}

