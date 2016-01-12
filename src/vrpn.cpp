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
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	std::cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," <<  t.pos[1] << "," << t.pos[2] << std::endl;
}

VRPNManager::VRPNManager(
):
	m_vrpnTracker(NULL),
	m_vrpnButton(NULL),
	m_width(0),
	m_height(0)
{
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
	m_vrpnButton->register_change_handler(0, handle_button);

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

	return 1;
}
