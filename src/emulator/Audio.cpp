#include "VirtualMachine.h"

Audio::Audio(VirtualMachine& vm, AudioFairy& audioFairy):
    VM(vm),
    audioFairy(audioFairy),
    //---
	rectangle1(true),
	rectangle2(false),
	triangle(),
	noize()
{
    //ctor
}

Audio::~Audio()
{
    //dtor
}

void Audio::run(uint16_t clockDelta)
{
	frameCnt += (clockDelta * frameIRQRate);
	while(frameCnt >= Audio::AUDIO_CLOCK){
		frameCnt -=Audio::AUDIO_CLOCK;
		// envelope
		this->rectangle1.onQuaterFrame();
		this->rectangle2.onQuaterFrame();
		this->triangle.onQuaterFrame();
		this->noize.onQuaterFrame();
		//sweep
		sweepProcessed = !sweepProcessed;
		if(!sweepProcessed){
			this->rectangle1.onHalfFrame();
			this->rectangle2.onHalfFrame();
			this->triangle.onHalfFrame();
			this->noize.onHalfFrame();
		}
		if(frameIRQCnt == 0){
			frameIRQCnt = frameIRQInterval;
			// IRQ & length counters
			if(frameIRQenabled){
				this->VM.sendIRQ();
			}
		}
		frameIRQCnt--;
	}
	clockCnt += (clockDelta * Audio::SAMPLE_RATE);
	while(clockCnt >= Audio::AUDIO_CLOCK){
		unsigned int processClock = Audio::AUDIO_CLOCK + leftClock;
		unsigned int delta = processClock / SAMPLE_RATE;
		leftClock = processClock % SAMPLE_RATE;
		clockCnt-=Audio::AUDIO_CLOCK;
		int16_t sound = 0;
		sound += rectangle1.createSound(delta);
		sound += rectangle2.createSound(delta);
		sound += triangle.createSound(delta);
		sound += noize.createSound(delta);

		audioFairy.pushAudio(sound * 500);
	}
}

void Audio::onHardReset()
{
	clockCnt = 0;
	leftClock = 0;
	frameCnt = 0;
	sweepProcessed = false;

	frameIRQenabled = true;
	frameIRQRate = 240;
	frameIRQCnt = frameIRQInterval = 4;
	rectangle1.onHardReset();
	rectangle2.onHardReset();
	triangle.onHardReset();
	noize.onHardReset();
}
void Audio::onReset()
{
	frameIRQenabled = true;
	frameIRQRate = 240;
	frameIRQCnt = frameIRQInterval = 4;
	rectangle1.onReset();
	rectangle2.onReset();
	triangle.onReset();
	noize.onReset();
}

void Audio::onVSync()
{
}



uint8_t Audio::readReg(uint16_t addr)
{
	if(addr != 0x4015){
		throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr << "for APU.";
	}
	return
			(this->rectangle1.isEnabled()		? 0b00000001 : 0)
			|	(this->rectangle2.isEnabled()	? 0b00000010 : 0)
			|	(this->triangle.isEnabled()		? 0b00000100 : 0)
			|	(this->noize.isEnabled()			? 0b00001000 : 0)
			|	(this->frameIRQenabled			? 0b01000000 : 0);
}
void Audio::writeReg(uint16_t addr, uint8_t value)
{
	switch(addr)
	{
	case 0x4000: //4000h - APU Volume/Decay Channel 1 (Rectangle)
		rectangle1.analyzeVolumeRegister(value);
		break;
	case 0x4001: //4001h - APU Sweep Channel 1 (Rectangle)
		rectangle1.analyzeSweepRegister(value);
		break;
	case 0x4002: //4002h - APU Frequency Channel 1 (Rectangle)
		rectangle1.analyzeFrequencyRegister(value);
		break;
	case 0x4003: //4003h - APU Length Channel 1 (Rectangle)
		rectangle1.analyzeLengthRegister(value);
		break;
	case 0x4004: //4004h - APU Volume/Decay Channel 2 (Rectangle)
		rectangle2.analyzeVolumeRegister(value);
		break;
	case 0x4005: //4005h - APU Sweep Channel 2 (Rectangle)
		rectangle2.analyzeSweepRegister(value);
		break;
	case 0x4006: //4006h - APU Frequency Channel 2 (Rectangle)
		rectangle2.analyzeFrequencyRegister(value);
		break;
	case 0x4007: //4007h - APU Length Channel 2 (Rectangle)
		rectangle2.analyzeLengthRegister(value);
		break;
	case 0x4008: //4008h - APU Linear Counter Channel 3 (Triangle)
		triangle.analyzeLinearCounterRegister(value);
		break;
	case 0x4009: //4009h - APU N/A Channel 3 (Triangle)
		//unused
		break;
	case 0x400A: //400Ah - APU Frequency Channel 3 (Triangle)
		triangle.analyzeFrequencyRegister(value);
		break;
	case 0x400B: //400Bh - APU Length Channel 3 (Triangle)
		triangle.analyzeLengthCounter(value);
		break;
	case 0x400C: //400Ch - APU Volume/Decay Channel 4 (Noise)
		noize.analyzeVolumeRegister(value);
		break;
	case 0x400d: //400Dh - APU N/A Channel 4 (Noise)
		//unused
		break;
	case 0x400e: //400Eh - APU Frequency Channel 4 (Noise)
		noize.analyzeFrequencyRegister(value);
		break;
	case 0x400F: //400Fh - APU Length Channel 4 (Noise)
		noize.analyzeLengthRegister(value);
		break;
	// ------------------------------------ DMC -----------------------------------------------------
	case 0x4010: //4010h - DMC Play mode and DMA frequency
		break;
	case 0x4011: //4011h - DMC Delta counter load register
		break;
	case 0x4012: //4012h - DMC address load register
		break;
	case 0x4013: //4013h - DMC length register
		break;
	// ------------------------------ CTRL --------------------------------------------------
	case 0x4015:
		analyzeStatusRegister(value);
		break;
	case 0x4017:
		analyzeLowFrequentryRegister(value);
		break;
	default:
		throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr << "for APU.";
	}
}

void Audio::analyzeStatusRegister(uint8_t value)
{
	rectangle1.setEnabled((value & 1)==1);
	rectangle2.setEnabled((value & 2)==2);
	triangle.setEnabled((value & 4)==4);
	/*
	noiseEnabled = (value & 8)==8;
	dmcEnabled = (value & 16)==16;
	*/
	frameIRQenabled = false;
}
void Audio::analyzeLowFrequentryRegister(uint8_t value)
{
	//Any write to $4017 resets both the frame counter, and the clock divider.
	frameIRQenabled = ((value & 0x40) == 0x00);
	if((value & 0x80) == 0x80){
		frameIRQRate = 192;
		frameIRQCnt = frameIRQInterval = 5;
	}else{
		frameIRQRate = 240;
		frameIRQCnt = frameIRQInterval = 4;
	}
	//frameIRQCnt = 0;
	frameCnt = Audio::AUDIO_CLOCK;
}
