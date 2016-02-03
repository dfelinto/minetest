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

#include "settings.h"

#include "hmd_bridge.h"

#include "HMD.h"
#include "Oculus.h"

#include <iostream>

#define TODO false

HMDManager::HMDManager(
):
	m_driver(NULL),
	m_frame(0),
	m_hmd(NULL)
{
	m_screenSize[0].X = -1;
	m_screenSize[1].Y = -1;
	m_screenSize[0].X = -1;
	m_screenSize[1].Y = -1;
	m_image[0] = NULL;
	m_image[1] = NULL;

	std::string displayBackend = g_settings->get("hmd_display_backend");

	if (displayBackend == "oculus") {
		m_hmd = new Oculus();
	}
	else if (displayBackend == "oculus_legacy") {
		std::cout << "HMD Error: Display backend not supported via DLL (" << displayBackend << ")" << std::endl;
		return;
	}
	else {
		std::cout << "HMD Error: Display backend not supported (" << displayBackend << ")" << std::endl;
		return;
	}

	m_preview = g_settings->getBool("hmd_preview");
}

#if 0
//TODO
EXPORT_LIB void Oculus_projectionMatrixLeft(Oculus *oculus, const float nearz, const float farz, float *r_matrix) { oculus->getProjectionMatrixLeft(nearz, farz, r_matrix); }
EXPORT_LIB void Oculus_projectionMatrixRight(Oculus *oculus, const float nearz, const float farz, float *r_matrix) { oculus->getProjectionMatrixRight(nearz, farz, r_matrix); }
#endif

HMDManager::~HMDManager()
{
	if (m_hmd) {
		delete m_hmd;
	}

	if (m_image[0]) {
		m_driver->removeTexture(m_image[0]);
	}

	if (m_image[1]) {
		m_driver->removeTexture(m_image[1]);
	}
}

bool HMDManager::setup()
{
	assert(m_hmd != NULL);

	if (m_hmd->setup(m_colorTexture[HMD_LEFT], m_colorTexture[HMD_RIGHT])) {
		return true;
	}
	else {
		std::cout << "HMD Bridge: ERROR : HMD failed to initialize textures ("
		    << m_colorTexture[HMD_LEFT] << ", " << m_colorTexture[HMD_RIGHT] << ")" << std::endl;

		delete m_hmd;
		m_hmd = NULL;
		return false;
	}
}

bool HMDManager::init(
	video::IVideoDriver *driver
	)
{
	if (m_hmd == NULL) {
		return false;
	}

	m_screenSize[HMD_LEFT].X = m_hmd->getWidthLeft();
	m_screenSize[HMD_LEFT].Y = m_hmd->getHeightLeft();
	m_screenSize[HMD_RIGHT].X = m_hmd->getWidthRight();
	m_screenSize[HMD_RIGHT].Y = m_hmd->getHeightRight();

	m_driver = driver;

	m_image[HMD_LEFT] = driver->addRenderTargetTexture(
		m_screenSize[HMD_LEFT], "HMD_left",
		irr::video::ECF_R8G8B8);

	m_image[HMD_RIGHT] = driver->addRenderTargetTexture(
		m_screenSize[HMD_RIGHT], "HMD_right",
		irr::video::ECF_R8G8B8);

	//TODO: get m_colorTexture from the m_images

	return this->setup();
}

bool HMDManager::frameReady()
{
	if (m_hmd == NULL) {
		return false;
	}

	if (m_hmd->frameReady()) {
		return true;
	}
	else {
		std::cout << "HMD Bridge: ERROR : HMD failed to process frame ready" << std::endl;
		return false;
	}
}

bool HMDManager::reCenter()
{
	if (m_hmd == NULL) {
		return false;
	}

	if (m_hmd->reCenter()) {
		std::cout << "HMD Bridge: HMD re-centered" << std::endl;
		return true;
	}
	else {
		std::cout << "HMD Bridge: ERROR : HMD failed to re-center" << std::endl;
		return false;
	}
}

/**
* Get fresh tracking data
*/
bool HMDManager::loop()
{
	if (m_hmd == NULL) {
		return false;
	}

	m_hmd->update(m_orientationRaw[HMD_LEFT], m_positionRaw[HMD_LEFT], m_orientationRaw[HMD_RIGHT], m_positionRaw[HMD_RIGHT]);
#if 0
	updateViewClipping()
	updateMatrices()
#endif
	return TODO;
}
