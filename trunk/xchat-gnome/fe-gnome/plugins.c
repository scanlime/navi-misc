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
fe_plugin_add (char *filename)
{
	void *handle;
	gpointer info_func;
	char *name, *desc, *version;

	handle = g_module_open (filename, 0);

	/* FIXME: The problem with all of this is that it doesn't do any checking to make
	 * sure the file is even a valid type. Should add some stuff to maybe check the
	 * extension?
	 */
	if (handle != NULL && g_module_symbol (handle, "xchat_plugin_get_info", &info_func)) {
		/* Create a new plugin instance and add it to our list of known plugins. */
		/* FIXME: zed added a 'reserved' field, but i'm not sure what it is */
		((xchat_plugin_get_info*) info_func) (&name, &desc, &version, NULL);
	} else {
		/* In the event that this foolish plugin has no get_info function we'll just use
		 * the file name. */
		name = strrchr (filename, '/') + 1;
		version = _("unknown");
		desc = _("unknown");
	}
}

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
	const gchar *homedir;
	gchar *xchatdir;

	homedir = g_get_home_dir ();
	xchatdir = g_strdup_printf ("%s/.xchat2/plugins", homedir);

	for_files (XCHATLIBDIR "/plugins", "*.so", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.sl", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.py", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.pl", fe_plugin_add);
	for_files (xchatdir, "*.so", fe_plugin_add);
	for_files (xchatdir, "*.sl", fe_plugin_add);
	for_files (xchatdir, "*.py", fe_plugin_add);
	for_files (xchatdir, "*.pl", fe_plugin_add);

	g_free (xchatdir);

	client = gconf_client_get_default ();
	enabled_plugins = gconf_client_get_list (client, "/apps/xchat/plugins/enabled", GCONF_VALUE_STRING, NULL);
}
