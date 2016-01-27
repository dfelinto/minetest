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

class HMDManager
{
public:
	HMDManager();
	~HMDManager();

	enum {
		LEFT = 0,
		RIGHT = 1,
	};

	void loop();
	void setup();

	int getWidth(int eye){return m_width[eye];}
	int getHeight(int eye){return m_height[eye];}

private:
	int m_width[2];
	int m_height[2];
	int m_frame;

#if 0
	hmd
	desc
	eyerenderdesc[2]
	hmdtoeyeviewoffset[2]
	eyerendertexture
	fbo
#endif
};

#endif /* HMD_MANAGER_H_ */
