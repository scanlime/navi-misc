/*
 * remote.h - Remote control mode, an interface for automating
 *            Fyre rendering. Among other things, this is used
 *            to implement slave nodes in a cluster.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#ifndef __REMOTE_H__
#define __REMOTE_H__

#include "animation.h"
#include "iterative-map.h"

G_BEGIN_DECLS

void    remote_main (IterativeMap*  map,
		     Animation*     animation,
		     gboolean       have_gtk);

G_END_DECLS

#endif /* __REMOTE_H__ */

/* The End */
