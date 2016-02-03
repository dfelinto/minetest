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
#include "..\source\Irrlicht\COpenGLTexture.h"

#include <iostream>

HMDManager::HMDManager(
):
	m_driver(NULL),
	m_frame(0),
	m_hmd(NULL),
	m_nearz(-1),
	m_farz(-1)
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

	static_cast<const video::COpenGLTexture* > (m_image[HMD_LEFT]);
	//m_colorTexture[HMD_LEFT] = static_cast<const video::COpenGLTexture*>(m_image[HMD_LEFT])->getOpenGLTextureName();
	//m_colorTexture[HMD_RIGHT] = static_cast<const video::COpenGLTexture*>(m_image[HMD_RIGHT])->getOpenGLTextureName();

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

void HMDManager::togglePreview(){
	m_preview = !m_preview;
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

	return m_hmd->update(m_orientationRaw[HMD_LEFT], m_positionRaw[HMD_LEFT], m_orientationRaw[HMD_RIGHT], m_positionRaw[HMD_RIGHT]);
}

void HMDManager::getPosition(int eye, irr::core::vector3df& position)
{
	position.X = m_positionRaw[eye][0];
	position.Y = m_positionRaw[eye][1];
	position.Z = m_positionRaw[eye][2];
}

void HMDManager::getOrientation(int eye, core::quaternion& orientation)
{
	orientation.W = m_orientationRaw[eye][0];
	orientation.X = m_orientationRaw[eye][1];
	orientation.Y = m_orientationRaw[eye][2];
	orientation.Z = m_orientationRaw[eye][3];
}

void HMDManager::getProjectionMatrix(int eye, const float nearz, const float farz, irr::core::matrix4& matrix)
{
	if ((nearz != m_nearz) || (farz != m_farz)) {
		updateProjectionMatrix(nearz, farz);
	}
	matrix = m_projectionMatrix[eye];
}

void HMDManager::updateProjectionMatrix(const float nearz, const float farz)
{
	float matrixRaw[2][16];

	m_hmd->getProjectionMatrixLeft(nearz, farz, false, matrixRaw[0]);
	m_hmd->getProjectionMatrixRight(nearz, farz, false, matrixRaw[1]);

	for (int i = 0; i < 16; i++) {
		m_projectionMatrix[HMD_LEFT][i] = matrixRaw[HMD_LEFT][i];
		m_projectionMatrix[HMD_RIGHT][i] = matrixRaw[HMD_RIGHT][i];
	}
}