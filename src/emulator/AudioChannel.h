/*
 * AudioChannel.h
 *
 *  Created on: 2011/08/25
 *      Author: psi
 */

#ifndef AUDIOCHANNEL_H_
#define AUDIOCHANNEL_H_

#include <cstddef>
#include <stdint.h>
#include <stdio.h>

class AudioChannel
{
public:
	static const uint8_t LengthCounterConst[32];
};

class Rectangle
{
private:
	bool isFirstChannel;

	//decay or volume
	uint8_t volumeOrDecayRate;
	bool decayReloaded;
	bool decayEnabled;
	uint8_t dutyRatio;

	uint8_t decayCounter;

	uint8_t decayVolume;

	//sweep
	bool sweepEnabled;
	uint8_t sweepShiftAmount;
	bool sweepIncreased;
	uint8_t sweepUpdateRatio;

	uint8_t sweepCounter;

	//
	uint16_t frequency;
	bool loopEnabled;
	uint8_t lengthCounter;

	//
	uint16_t freqCounter;
	uint16_t dutyCounter;

public:
	explicit Rectangle(bool isFirstChannel):
	isFirstChannel(isFirstChannel)
	{

	}

	inline void analyzeVolumeRegister(uint8_t reg)
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
	inline void analyzeSweepRegister(uint8_t reg)
	{
		sweepShiftAmount = reg & 7;
		sweepIncreased = (reg & 0x8) == 0x0;
		sweepCounter = sweepUpdateRatio = (reg >> 4) & 3;
		sweepEnabled = (reg&0x80) == 0x80;
	}
	inline void analyzeFrequencyRegister(uint8_t reg)
	{
		frequency = (frequency & 0x0700) | (reg);
	}
	inline void analyzeLengthRegister(uint8_t reg)
	{
		frequency = (frequency & 0x00ff) | ((reg & 7) << 8);
		lengthCounter = AudioChannel::LengthCounterConst[reg >> 3];
		//Writing to the length registers restarts the length (obviously),
		//and also restarts the duty cycle (channel 1,2 only),
		dutyCounter = 0;
		//and restarts the decay volume (channel 1,2,4 only).
		decayReloaded = true;
	}
	inline void onQuaterFrame()
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
	inline void onHalfFrame()
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
	inline int16_t createSound(unsigned int deltaClock)
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
			return 0;
		}
	}
	inline void setEnabled(bool enabled)
	{
		if(!enabled){
			lengthCounter = 0;
		}
	}
	inline bool isEnabled()
	{
		return lengthCounter != 0;
	}
	inline void onHardReset(){
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
	inline void onReset(){
		onHardReset();
	}
};

class Triangle
{
private:
	const static uint8_t waveForm[32];

	bool haltFlag;

	bool enableLinearCounter;
	uint16_t frequency;
	uint16_t linearCounterBuffer;
	//
	uint16_t linearCounter;
	uint16_t lengthCounter;
	//
	uint16_t freqCounter;
	uint16_t streamCounter;
public:
	inline void analyzeLinearCounterRegister(uint8_t value)
	{
		enableLinearCounter = ((value & 128) == 128);
		linearCounterBuffer = value & 127;
	}
	inline void analyzeFrequencyRegister(uint8_t value)
	{
		frequency = (frequency & 0x0700) | value;
	}
	inline void analyzeLengthCounter(uint8_t value)
	{
		frequency = (frequency & 0x00ff) | ((value & 7) << 8);
		lengthCounter = AudioChannel::LengthCounterConst[value >> 3];
		//Side effects 	Sets the halt flag
		haltFlag = true;
	}
	inline int16_t createSound(unsigned int deltaClock){
		if(lengthCounter == 0 || linearCounter == 0){
			return 0;
		}
		unsigned int nowCounter = this->freqCounter + deltaClock;
		this->freqCounter = nowCounter % (this->frequency + 1);
		this->streamCounter = (this->streamCounter + (nowCounter  / (this->frequency + 1))) & 31;
		return Triangle::waveForm[this->streamCounter];
	}
	inline void onHardReset(){
		haltFlag = false;
		enableLinearCounter = false;
		frequency = 0;
		linearCounterBuffer = 0;
		linearCounter = 0;
		lengthCounter = 0;
		freqCounter = 0;
		streamCounter = 0;
	}
	inline void onReset(){
		onHardReset();
	}
	inline void onQuaterFrame(){
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
	inline void onHalfFrame(){
		if(lengthCounter != 0 && !enableLinearCounter){
			lengthCounter--;
		}
	}
	inline void setEnabled(bool enabled)
	{
		if(!enabled){
			lengthCounter = 0;
			linearCounter = linearCounterBuffer = 0;
		}
	}
	inline bool isEnabled()
	{
		return lengthCounter != 0 && linearCounter != 0;
	}
};

class Noize
{
public:

};


#endif /* AUDIOCHANNEL_H_ */
