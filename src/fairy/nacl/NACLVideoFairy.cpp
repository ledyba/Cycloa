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
#include <string.h>
#include <stdio.h>

NACLVideoFairy::NACLVideoFairy(pp::InstanceHandle& instance) :
VideoFairy(),
gfx(instance, pp::Size(VideoFairy::screenWidth, VideoFairy::screenHeight), true),
img(instance, PP_IMAGEDATAFORMAT_RGBA_PREMUL, pp::Size(VideoFairy::screenWidth, VideoFairy::screenHeight), true)
{
	const uint32_t stride = this->img.stride();
	uint8_t* pixel8 = reinterpret_cast<uint8_t*>(this->img.data());
	memset(pixel8, 0, this->img.size().height() * stride);

	this->gfx.ReplaceContents(&this->img);
	this->gfx.Flush(pp::CompletionCallback());

	printf("initialized video");
}

NACLVideoFairy::~NACLVideoFairy()
{
}

void NACLVideoFairy::dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask)
{
	const uint32_t stride = this->img.stride();
	uint8_t* pixel8 = reinterpret_cast<uint8_t*>(this->img.data());

	for(uint32_t h = 0;h<VideoFairy::screenHeight; ++h){
		uint32_t* const line = reinterpret_cast<uint32_t*>(pixel8);
		for(uint32_t w = 0;w<VideoFairy::screenWidth; ++w){
			line[w] = VideoFairy::nesPalette[nesBuffer[h][w] & paletteMask];
		}
		pixel8 += stride;
	}

	this->gfx.ReplaceContents(&this->img);
	this->gfx.Flush(pp::CompletionCallback());
}
