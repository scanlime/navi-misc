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
#include "gui.h"
#include "../common/util.h"
#include "../common/xchat-plugin.h"
#include "../common/plugin.h"
#include "../common/outbound.h"

#ifndef PLUGIN_C
#define PLUGIN_C
#endif

#include "xg-plugin.h"
#include "plugins.h"

typedef int		(xchat_init_func)				(xchat_plugin *, char **, char **, char **, char *);
typedef int		(xchat_deinit_func)			(xchat_plugin *);
typedef void	(xchat_plugin_get_info)		(char **, char **, char **, char **);
typedef int		(xchat_gnome_plugin_init)	(xchat_gnome_plugin *);

GSList *enabled_plugins;

static void
autoload_plugin_cb (gchar * filename, gpointer data)
{
	load_plugin (gui.current_session, filename, NULL);
}

void
autoload_plugins ()
{
	g_slist_foreach (enabled_plugins, (GFunc) & autoload_plugin_cb, NULL);
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

xchat_gnome_plugin *
new_xg_plugin ()
{
	xchat_gnome_plugin *plugin = malloc (sizeof (xchat_gnome_plugin));
	plugin->xg_get_main_window = xg_get_main_window;
	plugin->xg_get_chan_list = xg_get_chan_list;

	return plugin;
}

char *
load_plugin (session * sess, char *filename, char *arg)
{
	int len;
	char *buf;
	void *handle;
	gpointer xg_init_func;
	xchat_gnome_plugin *pl;

	len = strlen (filename);

	if (len > 3 && strcasecmp (filename + len - 3, ".so") == 0) {
		/* Plugin */
		handle = g_module_open (filename, 0);

		if (handle != NULL && g_module_symbol (handle, "xchat_gnome_plugin_init", &xg_init_func)) {
			pl = new_xg_plugin ();
			((xchat_gnome_plugin_init *) xg_init_func) (pl);
		}

		char *err = NULL;
		err = plugin_load (sess, filename, arg);

		if (err != NULL)
			return err;
	} else {
		/* Script */
		buf = (char*) malloc (len + 8);
		if (strchr (filename, ' '))
			sprintf (buf, "LOAD \"%s\"", filename);
		else
			sprintf (buf, "LOAD %s", filename);

		handle_command (gui.current_session, buf, FALSE);
		free (buf);
	}

	return NULL;
}


/*** Functions called by plugins ***/
GtkWidget *
xg_get_main_window ()
{
	return (GTK_WIDGET (gui.main_window));
}

GtkTreeModel *
xg_get_chan_list ()
{
	return gtk_tree_view_get_model (GTK_TREE_VIEW (gui.server_tree));
}

/*** The End ***/
/* vim:ts=3:sw=3
 */
