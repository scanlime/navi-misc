/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
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

#ifndef __EVO_NOTIFICATION_AREA_ICON_H__
#define __EVO_NOTIFICATION_AREA_ICON_H__

#include <glib.h>

typedef enum {
	NI_STATUS_NORMAL,
	NI_STATUS_NEW_MAIL,
} NIStatus;

gboolean ni_init ();
void     ni_set_status (NIStatus status);

#endif
