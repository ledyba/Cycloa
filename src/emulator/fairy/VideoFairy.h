/*
 * VideoFairy.h
 *
 *  Created on: 2011/08/23
 *      Author: psi
 */

#ifndef VIDEOFAIRY_H_
#define VIDEOFAIRY_H_

#include <stdint.h>
#include <cstddef>
#include "../exception/EmulatorException.h"

class VideoFairy
{
public:
	enum{
		screenWidth = 256,
		screenHeight = 240
	};
	static const uint32_t nesPaletteARGB[64];
	explicit VideoFairy(){}
	virtual ~VideoFairy(){}
	virtual void dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask) = 0;
};

class DummyVideoFairy : public VideoFairy
{
public:
	explicit DummyVideoFairy(){}
	virtual ~DummyVideoFairy(){}
};

#endif /* VIDEOFAIRY_H_ */
