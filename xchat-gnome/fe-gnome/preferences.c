/*
 * preferences.h - interface to storing preferences
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include "preferences.h"
#include <gconf/gconf-client.h>

gboolean preferences_exist() {
	GConfClient *client;

	client = gconf_client_get_default();

	if(gconf_client_get_string(client, "/apps/xchat/version", NULL) == NULL)
		return FALSE;
	
	/* probably eventually do some checking on the version to migrate from
	   previous versions when new features are added */
	return TRUE;
}
