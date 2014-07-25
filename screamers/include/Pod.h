/*
 * Pod.h - podly pod
 *
 * Copyright (C) 2005 Screamers Group (see AUTHORS)
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

#include <tnl.h>
#include <tnlNetObject.h>
#include <tnlGhostConnection.h>

#ifndef _POD_H_
#define _POD_H_

class Pod : public TNL::NetObject
{
public:
	enum MaskBits {
		InitialMask = (1 << 0), // never set explicitly, used for initialization
	};

			 Pod              ();
			~Pod              ();

	TNL::U32	 packUpdate       (TNL::GhostConnection *connection, TNL::U32 updateMask, TNL::BitStream *stream);
	void		 unpackUpdate     (TNL::GhostConnection *connection, TNL::BitStream *stream);

	void		 onGhostAvailable (TNL::GhostConnection *connection);
	bool		 onGhostAdd       (TNL::GhostConnection *connection);

	TNL_DECLARE_CLASS(Pod);

/* This is the data I've thought up to represent a pod, but I'm not sure
 * how much of it we'll actually keep here.
	float	position[3];
	float	velocity[3];
	float	orientation[4];		// wow, a quaternion!
	float	moment[4];		// and another!

	float	shields;		// [0,1]
	int	rounds;
*/
};

#endif
