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

#ifndef VRPN_MANAGER_H_
#define VRPN_MANAGER_H_

#include "vrpn_Tracker.h"
#include "vrpn_Button.h"
#include "vrpn_Analog.h"

class VRPNManager
{
public:
	VRPNManager();
	~VRPNManager();

	// return true if initialization was correct
	int setup();

	// update VRPN callbacks
	int loop();

private:
	// head tracker
	vrpn_Tracker_Remote *m_vrpnTracker;
	// wiimote
	vrpn_Button_Remote *m_vrpnButton;
	// planovision width
	float m_width;
	// planovision height
	float m_height;
};

#endif /* VRPN_MANAGER_H_ */
