/*
 * plugins.h - Load all of the plugins in the plugins/ directory
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
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

#include "plugins.h"
#include <gmodule.h>
#include <glib.h>

void
load_plugins(void)
{
	GDir *dir;
	const gchar *file;
	GModule *plugin;

	if (!g_module_supported())
		return;

	dir = g_dir_open ("src/plugins", 0, NULL);
	if (dir == NULL)
		return;

	while (file = g_dir_read_name (dir))
	{
		gchar *full = g_strdup_printf("src/plugins/%s", file);
		plugin = g_module_open (full, G_MODULE_BIND_LOCAL);
		if (plugin == NULL)
			g_print("failure on '%s'\n", file);
		else
			g_print("success on '%s'\n", file);
		g_free(full);
	}

	g_dir_close (dir);
}
