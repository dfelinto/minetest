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
#include "irr_v3d.h"
#include "matrix4.h"

class VRPNManager
{
public:
	VRPNManager();
	~VRPNManager();

	enum {
		AD_FORWARD  = 1 << 0,
		AD_BACKWARD = 1 << 1,
		AD_LEFT     = 1 << 2,
		AD_RIGHT    = 1 << 3,
		AD_UP       = 1 << 4,
		AD_DOWN     = 1 << 5,
	};

	// return true if initialization was correct
	int setup();

	// update VRPN callbacks
	int loop();

	// called from
	void resetNavigation()
	{
		m_scale = m_scaleScene;
		m_vehicle = irr::core::IdentityMatrix;
	}

	void setScalingDown(const bool value) {m_scalingDown = value;}
	void setScalingUp(const bool value) {m_scalingDown = value;}
	void setMovement(int direction, const bool value) {
		if (value) {
			m_activeDirection |= direction;
		}
		else {
			m_activeDirection &= ~direction;
		}
	}

private:
	void updateVehicle();

	// head tracker
	vrpn_Tracker_Remote *m_vrpnTracker;
	// wiimote
	vrpn_Button_Remote *m_vrpnButton;
	// planovision width
	float m_width;
	// planovision height
	float m_height;
	float m_scaleScene;
	float m_scale;
	float m_speed;

	irr::core::matrix4 m_vehicle;
	bool m_scalingUp;
	bool m_scalingDown;
	int m_activeDirection;
	irr::core::vector3df m_headPosition;
};

#endif /* VRPN_MANAGER_H_ */
