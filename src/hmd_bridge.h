/*
Minetest
Copyright (C) 2010-2016

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef HMD_MANAGER_H_
#define HMD_MANAGER_H_

#include "HMD.h"

class HMDManager
{
public:
	HMDManager();
	~HMDManager();

	enum {
		HMD_LEFT = 0,
		HMD_RIGHT = 1,
	};

	int getWidth(int eye){return m_width[eye];}
	int getHeight(int eye){return m_height[eye];}

	bool init();
	bool loop();
	bool frameReady();
	bool reCenter();

private:
	bool setup();

	unsigned int m_colorTexture[2];
	float m_orientationRaw[2][4];
	float m_positionRaw[2][3];
	int m_width[2];
	int m_height[2];
	int m_frame;
	HMD *m_hmd;
};

#endif /* HMD_MANAGER_H_ */
