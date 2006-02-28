/*
 * plugins.c - handle (auto)loading and unloading of plugins
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include <config.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include <gmodule.h>
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

xchat_gnome_plugin *new_xg_plugin (void);

typedef int  (xchat_init_func)         (xchat_plugin *, char **, char **, char **, char *);
typedef int  (xchat_deinit_func)       (xchat_plugin *);
typedef void (xchat_plugin_get_info)   (char **, char **, char **, char **);
typedef int  (xchat_gnome_plugin_init) (xchat_gnome_plugin *);

GSList *enabled_plugins;
static GSList *loaded_plugins = NULL;

static void
autoload_plugin_cb (gchar * filename, gpointer data)
{
	gboolean script = GPOINTER_TO_INT (data);
	load_plugin (gui.current_session, filename, NULL, script, TRUE);
}

void
autoload_plugins (void)
{
	/* Split plugin loading into two passes - one for plugins, one for
	 * scripts.  This makes sure that any language binding plugins are
	 * loaded before we try to start any scripts.
	 */
	g_slist_foreach (enabled_plugins, (GFunc) & autoload_plugin_cb, GINT_TO_POINTER (FALSE));
	g_slist_foreach (enabled_plugins, (GFunc) & autoload_plugin_cb, GINT_TO_POINTER (TRUE));
}

void
plugins_initialize (void)
{
	GConfClient *client;

	client = gconf_client_get_default ();
	enabled_plugins = gconf_client_get_list (client, "/apps/xchat/plugins/loaded", GCONF_VALUE_STRING, NULL);
}

int
unload_plugin (char *filename)
{
	GSList *item;
	int len = strlen (filename);

	if (len > 3 && strcasecmp (filename + len - 3, ".so") == 0) {
		/* Plugin. */
		return plugin_kill (filename, 1);
	} else {
		/* Script. */
		gchar *command = g_strdup_printf ("UNLOAD \"%s\"", filename);
		handle_command (gui.current_session, command, FALSE);
		g_free (command);
	}

	item = g_slist_find_custom (loaded_plugins, filename, strcmp);
	loaded_plugins = g_slist_remove (loaded_plugins, item->data);
	g_free (item->data);

	return 1;
}

xchat_gnome_plugin *
new_xg_plugin (void)
{
	xchat_gnome_plugin *plugin = g_new0 (xchat_gnome_plugin, 1);
	plugin->xg_get_main_window = xg_get_main_window;
	plugin->xg_get_chan_list   = xg_get_chan_list;
	plugin->xg_get_ui_manager  = xg_get_ui_manager;

	return plugin;
}

char *
load_plugin (session * sess, char *filename, char *arg, gboolean script, gboolean autoload)
{
	int len;
	char *err;
	void *handle;
	gpointer xg_init_func;
	xchat_gnome_plugin *pl;

	len = strlen (filename);

	/* If we've already loaded this plugin, don't do so again.  This
	 * prevents plugins from being loaded several times (which can
	 * happen if the enabled key has something listed more than once.
	 */
	if (g_slist_find_custom (loaded_plugins, filename, strcmp))
		return;

	if (len > 3 && strcasecmp (filename + len - 3, ".so") == 0) {
		if (!(autoload && script)) {
			/* Plugin */
			handle = g_module_open (filename, 0);

			if (handle != NULL && g_module_symbol (handle, "xchat_gnome_plugin_init", &xg_init_func)) {
				pl = new_xg_plugin ();
				((xchat_gnome_plugin_init *) xg_init_func) (pl);
			}

			err = plugin_load (sess, filename, arg);

			if (err != NULL)
				return err;
		}
	} else if (script == TRUE) {
		/* Script */
		gchar *command = g_strdup_printf ("LOAD \"%s\"", filename);
		handle_command (gui.current_session, command, FALSE);
		g_free (command);
	}

	loaded_plugins = g_slist_prepend (loaded_plugins, g_strdup (filename));

	return NULL;
}


/*** Functions called by plugins ***/
GtkWidget *
xg_get_main_window (void)
{
	return (GTK_WIDGET (gui.main_window));
}

GtkTreeModel *
xg_get_chan_list (void)
{
	return gtk_tree_view_get_model (GTK_TREE_VIEW (gui.server_tree));
}

GtkUIManager *
xg_get_ui_manager (void)
{
	return g_object_ref (gui.manager);
}
