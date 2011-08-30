/*
 * AudioFairy.h
 *
 *  Created on: 2011/08/28
 *      Author: psi
 */

#ifndef AUDIOFAIRY_H_
#define AUDIOFAIRY_H_

#include "../exception/EmulatorException.h"
#include <stdio.h>

class AudioFairy
{
private:
	static const int BUFFER_SIZE = 0x40000;
	int16_t soundBuffer[BUFFER_SIZE];
	int available;
	int lastIndex;
	int firstIndex;
public:
	AudioFairy():
		available(BUFFER_SIZE),
		lastIndex(0),
		firstIndex(0)
	{
	}
	virtual ~AudioFairy()
	{

	}
	inline bool pushAudio(int16_t sound)
	{
		if(available > 0){
			soundBuffer[lastIndex] = sound;
			lastIndex = (lastIndex+1) & (BUFFER_SIZE-1);
			available--;
			return true;
		}else{
			return false;
		}
	}
protected:
	inline int popAudio(int16_t* buff, int maxLength)
	{
		int copiedLength = std::min(BUFFER_SIZE-available, maxLength);
		available+=copiedLength;
		if(firstIndex + copiedLength < BUFFER_SIZE){
			memcpy(buff, &soundBuffer[firstIndex], sizeof(int16_t) * copiedLength);
			firstIndex += copiedLength;
		}else{
			int first = BUFFER_SIZE-firstIndex;
			int last = copiedLength-first;
			memcpy(buff, &soundBuffer[firstIndex], sizeof(int16_t) * first);
			memcpy(&buff[first], &soundBuffer[0], sizeof(int16_t) * last);
			firstIndex = last;
		}

		return copiedLength;
	}
	inline int checkAvailableSize()
	{
		return available;
	}
};


#endif /* AUDIOFAIRY_H_ */
