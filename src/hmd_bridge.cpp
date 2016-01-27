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

#include <iostream>

HMDManager::HMDManager(
):
	m_frame(0)
{
	m_width[0] = 1920;
	m_width[1] = 1920;
	m_height[0] = 1080;
	m_height[1] = 1080;
}

HMDManager::~HMDManager()
{
}

void HMDManager::setup()
{
	std::cout << "HMD::setup(): " << m_width << " x " << m_height << std::endl;
}

void HMDManager::loop()
{
	std::cout << "HMD::loop()" << std::endl;
}
