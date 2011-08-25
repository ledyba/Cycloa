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
	enum ColorFormat {
		RGB_888
	};
	static const uint16_t screenWidth = 256;
	static const uint16_t screenHeight = 240;
	explicit VideoFairy(uint8_t* const data,ColorFormat colorFormat, uint16_t width, uint16_t height, uint16_t pitch, uint16_t bytesPerPixel)
	:data(data), width(width), height(height), bytesPerPixel(bytesPerPixel), pitch(pitch)
	{
		this->setColorFormat(colorFormat);
	};
	virtual ~VideoFairy(){};
	inline uint8_t& operator[](uint32_t pos)
	{
		return this->data[pos];
	}
	inline uint8_t* getPtr()
	{
		return this->data;
	}
	inline const uint8_t* getPtr() const
	{
		return this->data;
	}
	inline uint8_t* getPtr(uint16_t x, uint16_t y)
	{
		return &this->data[getPos(x,y)];
	}
	inline uint32_t getPos(uint16_t x, uint16_t y) const
	{
		return (y * pitch)+(this->bytesPerPixel * x);
	}
	inline uint16_t getBytesPerPixel() const
	{
		return this->bytesPerPixel;
	}
	inline uint16_t getPitch() const
	{
		return this->pitch;
	}
	inline uint16_t getWidth() const
	{
		return this->width;
	}
	inline uint16_t getHeight() const
	{
		return this->height;
	}
	virtual void enter(int32_t waitLimit = -1){}
	virtual void leave(){}
	virtual void dispatchRendering(){}
protected:
    inline void setData(uint8_t *data)
    {
        this->data = data;
    }
    inline void setHeight(uint16_t height)
    {
        this->height = height;
    }
    inline void setWidth(uint16_t width)
    {
        this->width = width;
    }
    inline void setColorFormat(ColorFormat colorFormat)
    {
        this->colorFormat = colorFormat;
    	switch(colorFormat)
    	{
    	case RGB_888:
    		break;
    	default:
			throw EmulatorException("Not supporting format:") << colorFormat;
    	}
    }
    inline void setPitch(uint16_t pitch)
    {
        this->pitch = pitch;
    }
    inline void setBytesPerPixel(uint16_t bytesPerPixel)
    {
    	this->bytesPerPixel = bytesPerPixel;
    }

private:
    uint8_t *data;
    ColorFormat colorFormat;
    uint16_t width;
    uint16_t height;
    uint16_t bytesPerPixel;
    uint16_t pitch;
};

class DummyVideoFairy : public VideoFairy
{
private:
	uint8_t dummySurface[VideoFairy::screenHeight][VideoFairy::screenWidth][3];
public:
	explicit DummyVideoFairy() :
	VideoFairy(reinterpret_cast<uint8_t*>(dummySurface), RGB_888, VideoFairy::screenWidth, VideoFairy::screenHeight, 256*3 ,3){}
};

#endif /* VIDEOFAIRY_H_ */
