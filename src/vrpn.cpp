/*
Minetest
Copyright (C) 2010-2015

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

#include "vrpn_Tracker.h"
#include "vrpn_Button.h"
#include "vrpn_Analog.h"

#include <iostream>

#include "settings.h"

#include "vrpn.h"

void VRPN_CALLBACK handle_button(void* userData, const vrpn_BUTTONCB b)
{
	// std::cout << "Button '" << b.button << "': " << b.state << std::endl;

	VRPNManager *vrpn = (VRPNManager *)userData;

	switch (b.button) {
		case 0: // Home
			vrpn->resetNavigation();
			break;
		case 5: // minus
			vrpn->setScalingDown(b.state == 1);
			break;
		case 6: // plus
			vrpn->setScalingUp(b.state == 1);
			break;
		case 7: // left
			vrpn->setMovement(VRPNManager::AD_LEFT, b.state == 1);
			break;
		case 8: // right
			vrpn->setMovement(VRPNManager::AD_RIGHT, b.state == 1);
			break;
		case 9: // down
			vrpn->setMovement(VRPNManager::AD_BACKWARD, b.state == 1);
			break;
		case 10: // up
			vrpn->setMovement(VRPNManager::AD_FORWARD, b.state == 1);
			break;
		case 1: // One
			vrpn->setMovement(VRPNManager::AD_UP, b.state == 1);
			break;
		case 2: // Two
			vrpn->setMovement(VRPNManager::AD_DOWN, b.state == 1);
			break;
		case 3: // A
		case 4: // B (back)
		default:
			break;
	}
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	// std::cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," <<  t.pos[1] << "," << t.pos[2] << std::endl;

	VRPNManager *vrpn = (VRPNManager *) userData;
	vrpn->updatePosition(t.pos[0], t.pos[1], t.pos[2]);
}

VRPNManager::VRPNManager(
):
	m_width(0.0f),
	m_height(0.0f),
    m_vrpnTracker(NULL),
	m_vrpnButton(NULL),
    m_scalingUp(false),
	m_scalingDown(false),
	m_activeDirection(0)
{
	for (int i = 0; i < 3; i++) {
		m_headPosition[i] = 0.0f;
	}

	m_scaleScene = g_settings->getFloat("planovision_scene_scale");
	m_speed = g_settings->getFloat("planovision_movement_speed") / m_scaleScene;

	this->resetNavigation();
	this->setupSwizzle();
}

VRPNManager::~VRPNManager()
{
}

void convertSwizzle(std::string swizzle, float *sign, int *id)
{
	if (swizzle == "-X") {
		*sign = -1.0f;
		*id = 0;
	}
	else if (swizzle == "+X") {
		*sign = 1.0f;
		*id = 0;
	}
	else if (swizzle == "-Y") {
		*sign = -1.0f;
		*id = 1;
	}
	else if (swizzle == "+Y") {
		*sign = 1.0f;
		*id = 1;
	}
	else if (swizzle == "-Z") {
		*sign = -1.0f;
		*id = 2;
	}
	else if (swizzle == "+Z") {
		*sign = 1.0f;
		*id = 2;
	}

	else {
		std::cout << "VRPN Error: swizzle value not valid: " << swizzle << std::endl;
	}
}

void VRPNManager::setupSwizzle()
{
	convertSwizzle(g_settings->get("planovision_swizzle_x"),
	               &m_swizzleSign[0], &m_swizzleId[0]);
	convertSwizzle(g_settings->get("planovision_swizzle_y"),
	               &m_swizzleSign[1], &m_swizzleId[1]);
	convertSwizzle(g_settings->get("planovision_swizzle_z"),
	               &m_swizzleSign[2], &m_swizzleId[2]);
}

int VRPNManager::setup()
{
	if (g_settings->get("3d_mode") != "planovision") {
		return 0;
	}

	m_width = g_settings->getFloat("planovision_width");
	m_height = g_settings->getFloat("planovision_height");

	std::string device_head = g_settings->get("planovision_vrpn_device_head");
	std::string device_wii = g_settings->get("planovision_vrpn_device_wii");

	m_vrpnTracker = new vrpn_Tracker_Remote(device_head.c_str());
	m_vrpnButton = new vrpn_Button_Remote(device_wii.c_str());

	m_vrpnTracker->register_change_handler((void *)this, handle_tracker);
	m_vrpnButton->register_change_handler((void *)this, handle_button);

	return 1;
}

int VRPNManager::loop()
{
	if (m_vrpnTracker) {
		m_vrpnTracker->mainloop();
	}

	if (m_vrpnButton) {
		m_vrpnButton->mainloop();
	}

	this->updateVehicle();

	return 1;
}

//! Constant for 64bit PI.
const f64 PI64		= 3.1415926535897932384626433832795028841971693993751;

//! 64bit constant for converting from radians to degrees
const f64 RADTODEG64 = 180.0 / PI64;

float angleSigned(irr::core::vector2df v1, irr::core::vector2df v2)
{
	const float perp_dot = (v1.Y * v2.X) - (v1.X * v2.Y);
	return atan2(perp_dot, v1.dotProduct(v2)) * RADTODEG64;
}

void VRPNManager::updateVehicle()
{
	if (m_scalingUp) {
		m_scale -= 0.01f * m_scaleScene;
	}
	else if (m_scalingDown) {
		m_scale += 0.01f * m_scaleScene;
	}

	if (m_activeDirection != 0) {
		float speed = m_speed * m_scale;
		irr::core::vector3df direction(0.0f, 0.0f, 0.0f);

		if ((m_activeDirection & AD_FORWARD) != 0) {
			direction.Y -= speed;
		}
		else if ((m_activeDirection & AD_BACKWARD) != 0) {
			direction.Y += speed;
		}

		if ((m_activeDirection & AD_LEFT) != 0) {
			direction.X += speed;
		}
		else if ((m_activeDirection & AD_RIGHT) != 0) {
			direction.X -= speed;
		}

		if ((m_activeDirection & AD_UP) != 0) {
			direction.Z += speed;
		}
		else if ((m_activeDirection & AD_DOWN) != 0) {
			direction.Z -= speed;
		}

		// handle the transformation relative to the head position
		irr::core::vector2df headPosition(m_headPosition[0], m_headPosition[1]);
		headPosition.normalize();

		float angle = angleSigned(headPosition, irr::core::vector2df(0.0f, 1.0f));
		direction.rotateXYBy(angle);

		irr::core::matrix4 translation;
		translation.setTranslation(direction);
		m_vehicle = translation * m_vehicle;
	}
}

void VRPNManager::updatePosition(float x, float y, float z)
{
	float positionRaw[3] = {x, y, z};

	for (int i = 0; i < 3; i++) {
		m_headPosition[i] = m_swizzleSign[i] * positionRaw[m_swizzleId[i]];
	}
}

void VRPNManager::getHead(float *x, float *y, float *z)
{
	*x = m_headPosition[0];
	*y = m_headPosition[1];
	*z = m_headPosition[2];
}

void VRPNManager::getVehicle(core::matrix4& matrix)
{
	for (int i = 0; i < 16; i++) {
		matrix[i] = m_vehicle[i];
	}
}
