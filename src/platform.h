/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * platform.h - Platform-specific hacks, should be included
 *              after config.h but before other headers in files
 *              that need it.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef WIN32

/* This is necessary to compile at all, since ole2 defines
 * a DATADIR constant that conflicts with ours.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif /* WIN32 */

#endif /* __PLATFORM_H__ */

/* The End */
