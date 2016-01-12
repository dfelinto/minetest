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
	std::cout << "Button '" << b.button << "': " << b.state << std::endl;
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
			vrpn->setMovement(VRPNManager::AD_DOWN, b.state == 1);
			break;
		case 10: // up
			vrpn->setMovement(VRPNManager::AD_UP, b.state == 1);
			break;
		case 1: // One
		case 2: // Two
		case 3: // A
		case 4: // B (back)
		default:
			break;
	}
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	std::cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," <<  t.pos[1] << "," << t.pos[2] << std::endl;

	// get raw data
	// swizzle the data
	// store head position
	// update matrices
}

VRPNManager::VRPNManager(
):
	m_vrpnTracker(NULL),
	m_vrpnButton(NULL),
	m_width(0.0f),
	m_height(0.0f),
	m_scalingUp(false),
	m_scalingDown(false),
	m_activeDirection(0)
{
	m_headPosition = irr::core::vector3df(0.0f, 0.0f, 0.0f);
	m_scaleScene = g_settings->getFloat("planovision_scene_scale");
	m_speed = g_settings->getFloat("planovision_movement_speed");

	this->resetNavigation();
}

VRPNManager::~VRPNManager()
{
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

#if 0
# planovision_swizzle_x enum -X, -Y-, -Z, +X, +Y, +Z
# planovision_swizzle_y enum -X, -Y-, -Z, +X, +Y, +Z
# planovision_swizzle_z enum -X, -Y-, -Z, +X, +Y, +Z
#endif

	m_vrpnTracker = new vrpn_Tracker_Remote(device_head.c_str());
	m_vrpnButton = new vrpn_Button_Remote(device_wii.c_str());

	m_vrpnTracker->register_change_handler(0, handle_tracker);
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
		irr::core::vector2df headPosition(m_headPosition.X, m_headPosition.Y);
		headPosition.normalize();

		float angle = headPosition.getAngleWith(irr::core::vector2df(0.0f, 1.0f));
		direction.rotateXYBy(angle);

		irr::core::matrix4 translation;
		translation.setTranslation(direction);
		m_vehicle = translation * m_vehicle;
	}
}
