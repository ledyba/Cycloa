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

#ifndef CycloaNaCl_NACLVIDEOFAIRY_H__CPP_
#define CycloaNaCl_NACLVIDEOFAIRY_H__CPP_

#include "../../emulator/fairy/VideoFairy.h"

#include <ppapi/cpp/graphics_2d.h>
#include <ppapi/cpp/image_data.h>
#include <pthread.h>

class CycloaInstance;

class NACLVideoFairy : public VideoFairy
{
private:
	CycloaInstance* cycloa;
	pp::Graphics2D gfx;
	pp::ImageData img;
	pthread_cond_t waitSync;
	pthread_mutex_t waitMutex;
private:
	NACLVideoFairy();
	NACLVideoFairy(const NACLVideoFairy& other);
	NACLVideoFairy& operator = (const NACLVideoFairy& other);
public:
	NACLVideoFairy(CycloaInstance* cycloa);
public:
	virtual ~NACLVideoFairy();
	virtual void dispatchRendering(const uint8_t nesBuffer[screenHeight][screenWidth], const uint8_t paletteMask);
	pp::Graphics2D& getGraphic2D(){return gfx;};
public:
	static void onFlushEnd(void* _self, int32_t val);
	static void dispatchRenderingRemote(void* _self, int32_t val);
};

#endif /* INCLUDE_GUARD */
