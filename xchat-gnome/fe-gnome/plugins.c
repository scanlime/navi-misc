/*
 * plugins.c - handle (auto)loading and unloading of plugins
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

#include <gconf/gconf-client.h>
#include <gmodule.h>
#include <libgnome/gnome-i18n.h>
#include <string.h>
#include "plugins.h"
#include "gui.h"
#include "../common/xchat-plugin.h"
#include "../common/plugin.h"
#include "../common/util.h"

typedef int (xchat_init_func) (xchat_plugin *, char **, char **, char **, char *);
typedef int (xchat_deinit_func) (xchat_plugin *);
typedef void (xchat_plugin_get_info) (char **, char **, char **, char **);

GSList *enabled_plugins;

static void
autoload_plugin_cb (gchar *filename, gpointer data)
{
	plugin_load (gui.current_session, filename, NULL);
}

void
autoload_plugins ()
{
	g_slist_foreach (enabled_plugins, (GFunc) &autoload_plugin_cb, NULL);
}

void
plugins_initialize ()
{
	GConfClient *client;

	client = gconf_client_get_default ();
	enabled_plugins = gconf_client_get_list (client, "/apps/xchat/plugins/enabled", GCONF_VALUE_STRING, NULL);
}

int
unload_plugin (char *filename)
{
	return plugin_kill (filename, 1);
}

char *
load_plugin (session *sess, char *filename, char *arg)
{
}
