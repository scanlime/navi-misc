/*
 * mem-util.h - Utilities for doing speedy and only slightly
 *              unholy things with pointers. This includes
 *              packing/unpacking pointers into relative
 *              addresses that are safe to use on disk,
 *              and forming aligned addresses for addressing
 *              into variable-sized structures.
 *
 * rtgraph real-time graphing package
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __RTG_MEM_UTIL_H__
#define __RTG_MEM_UTIL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Relative memory addressing. This requires a
 * GArray shared by all members of a data structure,
 * and RtgRelPointer types used instead of regular
 * pointers.
 *
 * If the GArray's data pointer is NULL, the relative
 * pointers work just like normal pointers. If not,
 * the pointer values are used to store integer
 * offsets within the GArray.
 *
 * For safe decoding, the caller should also check the
 * number of valid bytes after the RtgRelPointer.
 */
typedef gpointer RtgRelPointer;

#define RTG_REL_TO_POINTER(array, rel)  ((array).data ? (


G_END_DECLS

#endif /* __RTG_MEM_UTIL_H__ */

/* The End */
