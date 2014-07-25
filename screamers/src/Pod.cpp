/*
 * Pod.cpp - podly pod
 *
 * Copyright (C) 2005 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "Pod.h"

TNL_IMPLEMENT_NETOBJECT(Pod);

Pod::Pod ()
{
	mNetFlags.set (Ghostable);
}

Pod::~Pod ()
{
}

TNL::U32 Pod::packUpdate (TNL::GhostConnection *connection, TNL::U32 updateMask, TNL::BitStream *stream)
{
	return 0;
}

void Pod::unpackUpdate (TNL::GhostConnection *connection, TNL::BitStream *stream)
{
}

void Pod::onGhostAvailable (TNL::GhostConnection *connection)
{
}

bool Pod::onGhostAdd (TNL::GhostConnection *connection)
{
	return true;
}
