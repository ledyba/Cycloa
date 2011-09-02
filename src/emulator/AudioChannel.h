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
	explicit Rectangle(bool isFirstChannel);
	~Rectangle();
	inline void analyzeVolumeRegister(uint8_t reg);
	inline void analyzeSweepRegister(uint8_t reg);
	inline void analyzeFrequencyRegister(uint8_t reg);
	inline void analyzeLengthRegister(uint8_t reg);
	inline void onQuaterFrame();
	inline void onHalfFrame();
	inline int16_t createSound(unsigned int deltaClock);
	inline void setEnabled(bool enabled);
	inline bool isEnabled();
	inline void onHardReset();
	inline void onReset();
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
	Triangle();
	~Triangle();
	inline void analyzeLinearCounterRegister(uint8_t value);
	inline void analyzeFrequencyRegister(uint8_t value);
	inline void analyzeLengthCounter(uint8_t value);
	inline int16_t createSound(unsigned int deltaClock);
	inline void onHardReset();
	inline void onReset();
	inline void onQuaterFrame();
	inline void onHalfFrame();
	inline void setEnabled(bool enabled);
	inline bool isEnabled();
};

class Noize
{
private:
	static const uint16_t FrequencyTable[16];

	//rand
	uint16_t shiftRegister;
	bool modeFlag;

	//decay
	uint8_t volumeOrDecayRate;
	bool decayReloaded;
	bool decayEnabled;

	uint8_t decayCounter;
	uint8_t decayVolume;
	//
	bool loopEnabled;
	uint16_t frequency;
	//
	uint16_t lengthCounter;
	//
	uint16_t freqCounter;
public:
	Noize();
	~Noize();
	inline void analyzeVolumeRegister(uint8_t reg);
	inline void analyzeFrequencyRegister(uint8_t reg);
	inline void analyzeLengthRegister(uint8_t reg);
	inline void onQuaterFrame();
	inline void onHalfFrame();
	inline int16_t createSound(unsigned int deltaClock);
	inline void setEnabled(bool enabled);
	inline bool isEnabled();
	inline void onHardReset();
	inline void onReset();
};

class Digital
{
private:
	VirtualMachine& VM;

	//
	bool irqEnabled;
	bool loopEnabled;
	uint16_t frequency;
	uint8_t deltaCounter;
	uint16_t sampleAddr;
	uint16_t sampleLength;
	uint16_t sampleLengthBuffer;
	const static uint16_t FrequencyTable[16];
	//
	uint8_t sampleBuffer;
	uint8_t sampleBufferLeft;
	//
	uint16_t freqCounter;
	bool irqActive;
public:
	Digital(VirtualMachine& vm);
	~Digital();
	inline void analyzeFrequencyRegister(uint8_t value);
	inline void analyzeDeltaCounterRegister(uint8_t value);
	inline void analyzeSampleAddrRegister(uint8_t value);
	inline void analyzeSampleLengthRegister(uint8_t value);
	inline void next();
	inline int16_t createSound(unsigned int deltaClock);
	inline void setEnabled(bool enabled);
	inline bool isEnabled();
	inline bool isIRQEnabled();
	inline void onHardReset();
	inline void onReset();
};

#endif /* AUDIOCHANNEL_H_ */
