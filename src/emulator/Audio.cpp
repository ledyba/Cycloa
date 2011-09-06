#include "VirtualMachine.h"

Audio::Audio(VirtualMachine& vm, AudioFairy& audioFairy):
    VM(vm),
    audioFairy(audioFairy),
    //---
	rectangle1(true),
	rectangle2(false),
	triangle(),
	noize(),
	digital(vm)
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
				this->frameIRQactive = true;
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
		sound += digital.createSound(delta);

		audioFairy.pushAudio(sound * 250);
	}
}

void Audio::onHardReset()
{
	clockCnt = 0;
	leftClock = 0;
	frameCnt = 0;
	sweepProcessed = false;

	frameIRQenabled = true;
	frameIRQactive = false;
	frameIRQRate = 240;
	frameIRQCnt = frameIRQInterval = 4;
	rectangle1.onHardReset();
	rectangle2.onHardReset();
	triangle.onHardReset();
	noize.onHardReset();
	digital.onHardReset();
}
void Audio::onReset()
{
	frameIRQenabled = true;
	frameIRQactive = false;
	frameIRQRate = 240;
	frameIRQCnt = frameIRQInterval = 4;
	rectangle1.onReset();
	rectangle2.onReset();
	triangle.onReset();
	noize.onReset();
	digital.onReset();
}

void Audio::onVSync()
{
}



uint8_t Audio::readReg(uint16_t addr)
{
	if(addr != 0x4015){
		throw EmulatorException("[FIXME] Invalid addr: 0x") << std::hex << addr << "for APU.";
	}
 	// Clears the frame interrupt flag after being read (but not the DMC interrupt flag).
	// If an interrupt flag was set at the same moment of the read, it will read back as 1 but it will not be cleared.
	uint8_t ret =
			(this->rectangle1.isEnabled()		? 1 : 0)
			|	(this->rectangle2.isEnabled()	? 2 : 0)
			|	(this->triangle.isEnabled()		? 4 : 0)
			|	(this->noize.isEnabled()			? 8 : 0)
			|	(this->digital.isEnabled()		? 16 : 0)
			|	(this->frameIRQactive				? 64 : 0)
			|	(this->digital.isIRQEnabled()	? 128 : 0);
	frameIRQactive = false;
	return ret;
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
		digital.analyzeFrequencyRegister(value);
		break;
	case 0x4011: //4011h - DMC Delta counter load register
		digital.analyzeDeltaCounterRegister(value);
		break;
	case 0x4012: //4012h - DMC address load register
		digital.analyzeSampleAddrRegister(value);
		break;
	case 0x4013: //4013h - DMC length register
		digital.analyzeSampleLengthRegister(value);
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
	noize.setEnabled((value & 8)==8);
	digital.setEnabled((value & 16)==16);

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


//---------------------------------------------------------------------------------------------------------------
const uint8_t AudioChannel::LengthCounterConst[32] = {
		0x0A,0xFE,0x14,0x02,0x28,0x04,0x50,0x06,
		0xA0,0x08,0x3C,0x0A,0x0E,0x0C,0x1A,0x0E,
		0x0C,0x10,0x18,0x12,0x30,0x14,0x60,0x16,
		0xC0,0x18,0x48,0x1A,0x10,0x1C,0x20,0x1E,
};



Rectangle::Rectangle(bool isFirstChannel):
isFirstChannel(isFirstChannel)
{

}

Rectangle::~Rectangle()
{

}

inline void Rectangle::analyzeVolumeRegister(uint8_t reg)
{
	decayCounter = volumeOrDecayRate = reg & 15;
	decayEnabled = (reg & 16) == 0;
	loopEnabled = (reg & 32) == 32;
	switch(reg >> 6)
	{
	case 0:
		dutyRatio = 2;
		break;
	case 1:
		dutyRatio = 4;
		break;
	case 2:
		dutyRatio = 8;
		break;
	case 3:
		dutyRatio = 12;
		break;
	}
}
inline void Rectangle::analyzeSweepRegister(uint8_t reg)
{
	sweepShiftAmount = reg & 7;
	sweepIncreased = (reg & 0x8) == 0x0;
	sweepCounter = sweepUpdateRatio = (reg >> 4) & 3;
	sweepEnabled = (reg&0x80) == 0x80;
}
inline void Rectangle::analyzeFrequencyRegister(uint8_t reg)
{
	frequency = (frequency & 0x0700) | (reg);
}
inline void Rectangle::analyzeLengthRegister(uint8_t reg)
{
	frequency = (frequency & 0x00ff) | ((reg & 7) << 8);
	lengthCounter = AudioChannel::LengthCounterConst[reg >> 3];
	//Writing to the length registers restarts the length (obviously),
	//and also restarts the duty cycle (channel 1,2 only),
	dutyCounter = 0;
	//and restarts the decay volume (channel 1,2,4 only).
	decayReloaded = true;
}
inline void Rectangle::onQuaterFrame()
{
	if(decayCounter == 0){
		decayCounter = this->volumeOrDecayRate;
		if(decayVolume == 0){
			if(loopEnabled){
				decayVolume = 0xf;
			}
		}else{
			decayVolume--;
		}
	}else{
		this->decayCounter--;
	}
	if(decayReloaded){
		decayReloaded = false;
		decayVolume = 0xf;
	}
}
inline void Rectangle::onHalfFrame()
{
	if(lengthCounter != 0 && !loopEnabled){
		lengthCounter--;
	}
	const uint16_t nowFreq = this->frequency;
	if(sweepEnabled){
		if(sweepCounter == 0){
			this->sweepCounter = sweepUpdateRatio;
			if(lengthCounter != 0 && this->sweepShiftAmount != 0){
				uint16_t shift = (this->frequency >> this->sweepShiftAmount);
				if(this->sweepIncreased){
					this->frequency += shift;
				}else{
					this->frequency -= shift;
					if(this->isFirstChannel){
						this->frequency--;
					}
				}
			}
		}else{
			this->sweepCounter--;
		}
	}
	if(nowFreq < 0x8 || this->frequency  > 0x7ff){
		sweepEnabled = false;
		lengthCounter = 0;
	}
}
inline int16_t Rectangle::createSound(unsigned int deltaClock)
{
	if(lengthCounter == 0){
		return 0;
	}
	unsigned int nowCounter = this->freqCounter + deltaClock;
	this->freqCounter = nowCounter % (this->frequency + 1);
	this->dutyCounter = (this->dutyCounter + (nowCounter  / (this->frequency + 1))) & 15;
	if(dutyCounter < dutyRatio){
		return decayEnabled ? decayVolume : volumeOrDecayRate;
	}else{
		return decayEnabled ? -decayVolume : -volumeOrDecayRate;
	}
}
inline void Rectangle::setEnabled(bool enabled)
{
	if(!enabled){
		lengthCounter = 0;
	}
}
inline bool Rectangle::isEnabled()
{
	return lengthCounter != 0;
}
inline void Rectangle::onHardReset()
{
	volumeOrDecayRate = 0;
	decayReloaded = false;
	decayEnabled = false;
	dutyRatio = 0;
	decayCounter = 0;
	sweepEnabled = 0;
	sweepShiftAmount = 0;
	sweepIncreased = false;
	sweepUpdateRatio = 0;
	sweepCounter = 0;
	frequency = 0;
	loopEnabled = false;
	lengthCounter = 0;
}
inline void Rectangle::onReset()
{
	onHardReset();
}

const uint8_t Triangle::waveForm[32] ={
		  0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF,
		  0xF,0xE,0xD,0xC,0xB,0xA,0x9,0x8,0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0
};

Triangle::Triangle()
{

}
Triangle::~Triangle()
{

}

inline void Triangle::analyzeLinearCounterRegister(uint8_t value)
{
	enableLinearCounter = ((value & 128) == 128);
	linearCounterBuffer = value & 127;
}
inline void Triangle::analyzeFrequencyRegister(uint8_t value)
{
	frequency = (frequency & 0x0700) | value;
}
inline void Triangle::analyzeLengthCounter(uint8_t value)
{
	frequency = (frequency & 0x00ff) | ((value & 7) << 8);
	lengthCounter = AudioChannel::LengthCounterConst[value >> 3];
	//Side effects 	Sets the halt flag
	haltFlag = true;
}
inline int16_t Triangle::createSound(unsigned int deltaClock)
{
	if(lengthCounter == 0 || linearCounter == 0){
		return 0;
	}
	unsigned int nowCounter = this->freqCounter + deltaClock;
	this->freqCounter = nowCounter % (this->frequency + 1);
	this->streamCounter = (this->streamCounter + (nowCounter  / (this->frequency + 1))) & 31;
	return Triangle::waveForm[this->streamCounter];
}
inline void Triangle::onHardReset(){
	haltFlag = false;
	enableLinearCounter = false;
	frequency = 0;
	linearCounterBuffer = 0;
	linearCounter = 0;
	lengthCounter = 0;
	freqCounter = 0;
	streamCounter = 0;
}
inline void Triangle::onReset()
{
	onHardReset();
}
inline void Triangle::onQuaterFrame()
{
	if(haltFlag){
		linearCounter = linearCounterBuffer;
	}else{
		if(linearCounter != 0){
			linearCounter--;
		}
	}
	if(!enableLinearCounter){
		haltFlag = false;
	}
}
inline void Triangle::onHalfFrame()
{
	if(lengthCounter != 0 && !enableLinearCounter){
		lengthCounter--;
	}
}
inline void Triangle::setEnabled(bool enabled)
{
	if(!enabled){
		lengthCounter = 0;
		linearCounter = linearCounterBuffer = 0;
	}
}
inline bool Triangle::isEnabled()
{
	return lengthCounter != 0 && linearCounter != 0;
}

const uint16_t Noize::FrequencyTable[16] = {
		4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068 //NTSC
		//4, 7, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778 //PAL
};

Noize::Noize()
{
}
Noize::~Noize()
{
}

inline void Noize::analyzeVolumeRegister(uint8_t reg)
{
	decayCounter = volumeOrDecayRate = reg & 15;
	decayEnabled = (reg & 16) == 0;
	loopEnabled = (reg & 32) == 32;
}
inline void Noize::analyzeFrequencyRegister(uint8_t reg)
{
	modeFlag = (reg & 128) == 128;
	frequency = Noize::FrequencyTable[reg & 15];
}
inline void Noize::analyzeLengthRegister(uint8_t reg)
{
	//Writing to the length registers restarts the length (obviously),
	lengthCounter = AudioChannel::LengthCounterConst[reg >> 3];
	//and restarts the decay volume (channel 1,2,4 only).
	decayReloaded = true;
}
inline void Noize::onQuaterFrame()
{
	if(decayCounter == 0){
		decayCounter = this->volumeOrDecayRate;
		if(decayVolume == 0){
			if(loopEnabled){
				decayVolume = 0xf;
			}
		}else{
			decayVolume--;
		}
	}else{
		this->decayCounter--;
	}
	if(decayReloaded){
		decayReloaded = false;
		decayVolume = 0xf;
	}
}
inline void Noize::onHalfFrame()
{
	if(lengthCounter != 0 && !loopEnabled){
		lengthCounter--;
	}
}
inline int16_t Noize::createSound(unsigned int deltaClock)
{
	if(lengthCounter == 0){
		return 0;
	}
	unsigned int nowCounter = this->freqCounter + deltaClock;
	const uint16_t divFreq = this->frequency + 1;
	const uint8_t shiftAmount = modeFlag ? 6 : 1;
	//FIXME: frequencyが小さい時に此のモデルが破綻する
	while(nowCounter >= divFreq){
		nowCounter -= divFreq;
		shiftRegister =(shiftRegister >> 1) | (((shiftRegister ^ (shiftRegister >> shiftAmount))  & 1) << 14);
	}
	this->freqCounter = nowCounter;
	if(((shiftRegister & 1) == 1)){
		return decayEnabled ? -decayVolume : -volumeOrDecayRate;
	}else{
		return decayEnabled ? decayVolume : volumeOrDecayRate;
	}
}
inline void Noize::setEnabled(bool enabled)
{
	if(!enabled){
		lengthCounter = 0;
	}
}
inline bool Noize::isEnabled()
{
	return lengthCounter != 0;
}
inline void Noize::onHardReset()
{
	//rand
	shiftRegister = 1<<14;
	modeFlag = false;

	//decay
	volumeOrDecayRate = false;
	decayReloaded = false;
	decayEnabled = false;

	decayCounter = 0;
	decayVolume = 0;
	//
	loopEnabled = false;
	frequency = 0;
	//
	lengthCounter = 0;
	//
	freqCounter = 0;
}
inline void Noize::onReset()
{
	onHardReset();
}

const uint16_t Digital::FrequencyTable[16] = {
		428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54 //NTSC
		//398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98,  78,  66,  50
};

Digital::Digital(VirtualMachine& vm) :
VM(vm)
{

}
Digital::~Digital(){

}
inline void Digital::analyzeFrequencyRegister(uint8_t value)
{
	irqEnabled = (value & 128) == 128;
	if(!irqEnabled){
		irqActive = false;
	}
	loopEnabled = (value & 64) == 64;
	frequency = Digital::FrequencyTable[value & 0xf];
}
inline void Digital::analyzeDeltaCounterRegister(uint8_t value)
{
	deltaCounter = value & 0x7f;
}
inline void Digital::analyzeSampleAddrRegister(uint8_t value)
{
	sampleAddr = 0xc000 | (value << 6);
}
inline void Digital::analyzeSampleLengthRegister(uint8_t value)
{
	sampleLength = sampleLengthBuffer = (value << 4) | 1;
}
inline void Digital::next()
{
	if(sampleBufferLeft == 0){
		sampleLength--;
		sampleBuffer=VM.read(sampleAddr);

		if(sampleAddr == 0xffff){
			sampleAddr = 0x8000;
		}else{
			sampleAddr++;
		}
		sampleBufferLeft = 7;
		VM.consumeCpuClock(4);
		if(sampleLength == 0){
			if(loopEnabled){
				sampleLength = sampleLengthBuffer;
			}else if(irqEnabled){
				VM.sendIRQ();
				irqActive = true;
			}else{
				return;
			}
		}
	}
	if(((sampleBuffer >>= 1) & 1) == 1){
		if(deltaCounter < 126){
			deltaCounter+=2;
		}
	}else{
		if(deltaCounter > 1){
			deltaCounter-=2;
		}
	}
	sampleBufferLeft--;
}
inline int16_t Digital::createSound(unsigned int deltaClock)
{
	if(sampleLength == 0 && !loopEnabled){
		return deltaCounter;
	}
	unsigned int nowCounter = this->freqCounter + deltaClock;
	const uint16_t divFreq = this->frequency + 1;
	while(nowCounter -= divFreq){
		next();
	}
	this->freqCounter = nowCounter;
	return deltaCounter;
}
inline void Digital::setEnabled(bool enabled)
{
	if(enabled){
		sampleLength = 0;
	}else if(sampleLength == 0){
		sampleLength = sampleLengthBuffer;
	}
	//Side effects 	After the write, the DMC's interrupt flag is cleared
	irqActive = false;
}
inline bool Digital::isEnabled()
{
	return sampleLength != 0;
}
inline bool Digital::isIRQEnabled()
{
	return irqEnabled;
}
inline void Digital::onHardReset()
{
	irqEnabled = false;
	irqActive = false;
	loopEnabled = false;
	frequency = 0;
	deltaCounter = 0;
	sampleAddr = 0xc000;
	sampleLength = 0;
	sampleLengthBuffer = 0;
	sampleBuffer = 0;
	sampleBufferLeft = 0;

	freqCounter = 0;
}
inline void Digital::onReset()
{
	onHardReset();
}

