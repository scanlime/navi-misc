/* notification.c
 *
 * A notification area plugin for xchat-gnome.
 *
 * Copyright (C) 2005 W. Evan Sheehan
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

#include <gtk/gtk.h>
#include <gconf/gconf-client.h>

#include "xchat-plugin.h"
#include "eggtrayicon.h"

#define VERSION "0.1"


void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc,
		char **plugin_version, void **reserved)
{
	*plugin_name = "Notification";
	*plugin_desc = "A notification area plugin.";
	*pluing_version = VERSION;

	if (reserved)
		*reserved = NULL;
}

void
xchat_plugin_init (xchat_plugin *plugin_handle, char **plugin_name,
		char **plugin_desc, char **plugin_version, char *arg)
{
	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);
}


/*** The End ***/
