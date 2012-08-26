/**
 * CycloaNaCl
 * Copyright (C) 2012 psi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NACLVideoFairy.h"
#include <ppapi/cpp/completion_callback.h>
#include "CycloaInstance.h"
#include <string.h>
#include <stdio.h>
#include <sstream>

NACLVideoFairy::NACLVideoFairy(CycloaInstance* cycloa) :
VideoFairy(),
cycloa(cycloa),
gfx(cycloa, pp::Size(VideoFairy::screenWidth, VideoFairy::screenHeight), true),
img(cycloa, PP_IMAGEDATAFORMAT_RGBA_PREMUL, pp::Size(VideoFairy::screenWidth, VideoFairy::screenHeight), true)
{
	cycloa->BindGraphics(this->gfx);
}

NACLVideoFairy::~NACLVideoFairy()
{
}
void NACLVideoFairy::onFlushEnd(void* _self, int32_t val)
{
	NACLVideoFairy* const self = reinterpret_cast<NACLVideoFairy*>(_self);
	pthread_cond_signal(&self->waitSync);
	pthread_mutex_unlock(&self->waitMutex);
}

void NACLVideoFairy::dispatchRenderingRemote(void* _self, int32_t val)
{
	NACLVideoFairy* const self = reinterpret_cast<NACLVideoFairy*>(_self);
	pthread_mutex_lock(&self->waitMutex);
	self->gfx.PaintImageData(self->img, pp::Point(0,0));
	if(self->gfx.Flush(pp::CompletionCallback(NACLVideoFairy::onFlushEnd, self)) != PP_OK){
		pthread_cond_signal(&self->waitSync);
		pthread_mutex_unlock(&self->waitMutex);
	}
}

void NACLVideoFairy::dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask)
{
	const uint32_t stride = this->img.stride();
	uint8_t* pixel8 = reinterpret_cast<uint8_t*>(this->img.data());

	for(uint32_t h = 0;h<VideoFairy::screenHeight; ++h){
		uint32_t* const line = reinterpret_cast<uint32_t*>(pixel8);
		for(uint32_t w = 0;w<VideoFairy::screenWidth; ++w){
			const uint32_t c = VideoFairy::nesPalette[nesBuffer[h][w] & paletteMask];
			line[w] = (c & 0xff) << 16 | (c & 0xff00) | ((c & 0xff0000) >> 16);
		}
		pixel8 += stride;
	}

	pthread_mutex_lock(&this->waitMutex);
	this->cycloa->CallOnMainThread(NACLVideoFairy::dispatchRenderingRemote, this);
	pthread_cond_wait(&this->waitSync, &this->waitMutex);
	pthread_mutex_unlock(&this->waitMutex);

	this->cycloa->frameWait();
}
