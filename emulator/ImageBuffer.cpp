#include "VirtualMachine.h"

ImageBuffer::ImageBuffer(uint16_t actualWidth, uint16_t actualHeight, uint16_t width, uint16_t height, uint16_t bytesPerPixel):
data(new uint8_t[bytesPerPixel * width * height]),
bytesPerPixel(bytesPerPixel),
actualWidth(actualWidth),
actualHeight(actualHeight),
height(height),
width(width)
{

}
ImageBuffer::~ImageBuffer()
{
	delete [] this->data;
}
uint8_t& ImageBuffer::operator[](uint32_t pos)
{
	return this->data[pos];
}
uint8_t* ImageBuffer::getPtr()
{
	return this->data;
}
const uint8_t* ImageBuffer::getPtr() const
{
	return this->data;
}
uint8_t* ImageBuffer::getPtr(uint16_t y)
{
	return &this->data[this->width * y * this->bytesPerPixel];
}
uint32_t ImageBuffer::getPos(uint16_t x, uint16_t y) const
{
	return this->bytesPerPixel * (y * this->width + x);
}
uint16_t ImageBuffer::getBytesPerPixel() const
{
	return this->bytesPerPixel;
}
uint32_t ImageBuffer::getTotalSize() const
{
	return this->width * this->height * this->bytesPerPixel;
}
uint16_t ImageBuffer::getWidth() const
{
	return this->width;
}
uint16_t ImageBuffer::getHeight() const
{
	return this->height;
}
uint16_t ImageBuffer::getActualWidth() const
{
	return this->actualWidth;
}
uint16_t ImageBuffer::getActualHeight() const
{
	return this->actualHeight;
}
