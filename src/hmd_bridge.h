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

#include "ITexture.h"
#include "IVideoDriver.h"
#include "HMD.h"
#include "quaternion.h"

class HMDManager
{
public:
	HMDManager();
	~HMDManager();

	enum eye {
		HMD_LEFT = 0,
		HMD_RIGHT = 1,
	};

	video::ITexture *getImage(int eye){ return m_image[eye];}

	v2u32 getScreenSize(int eye){ return m_screenSize[eye]; }
	bool getPreview(){ return m_preview; }
	void togglePreview();

	void getProjectionMatrix(int eye, float nearz, float farz, irr::core::matrix4& matrix);
	void getPosition(int eye, irr::core::vector3df& position);
	void getOrientation(int eye, core::quaternion& orientation);
	void getEuler(int eye, float *r_yaw, float *r_pitch, float *r_roll);

	bool init(video::IVideoDriver *driver);
	bool loop();
	bool frameReady();
	bool reCenter();

private:
	bool setup();
	void updateProjectionMatrix(const float nearz, const float farz);

	video::IVideoDriver *m_driver;
	video::ITexture* m_image[2];

	unsigned int m_colorTexture[2];
	float m_yaw[2];
	float m_pitch[2];
	float m_roll[2];
	float m_orientationRaw[2][4];
	float m_positionRaw[2][3];
	v2u32 m_screenSize[2];
	bool m_preview;
	int m_frame;
	HMD *m_hmd;
	float m_nearz;
	float m_farz;
	irr::core::matrix4 m_projectionMatrix[2];
};

#endif /* HMD_MANAGER_H_ */
