/*
 * migration.c - migration functions
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include "migration.h"
#include "util.h"

static void dbus_migration (void);

void
run_migrations (void)
{
	dbus_migration ();
}

/* Compare installed version with the one in argument and returns:
 * -1 : if the arg is lesser
 *  0 : if it's the same
 *  1 : if the arg is greater */
static gint
check_version (guint major, guint minor, guint micro)
{

	GConfClient *client = gconf_client_get_default ();
	gchar *version = gconf_client_get_string (client, "/apps/xchat/version", NULL);
	g_object_unref (client);

	if (version == NULL) {
		return 0;
	}

	guint effective[3] = {0, 0, 0};
	guint result;

	gchar **nbs = g_strsplit (version, ".", 0);
	for (int i = 0; i < 3; i++) {
		if (nbs[i]) {
			if (g_str_has_suffix (nbs[i], "svn")) {
				/* Remove "svn" */
				gchar *tmp;
				tmp = g_strndup (nbs[i], strlen (nbs[i]) - 3);
				effective[i] = atoi (tmp);
				g_free (tmp);
			} else {
				effective[i] = atoi (nbs[i]);
			}
		} else {
			break;
		}
	}
	g_strfreev (nbs);
	g_free (version);

	if (major < effective[0]) {
		return -1;
	} else if (major > effective[0]) {
		return 1;
	}

	if (minor < effective[1]) {
		return -1;
	} else if (minor > effective[1]) {
		return 1;
	}

	if (micro < effective[2]) {
		return -1;
	} else if (micro > effective[2]) {
		return 1;
	}

	return 0;
}

static void
dbus_migration_remove_autoload (void)
{
	GConfClient *client;
	GSList *enabled_plugins, *l;

	client = gconf_client_get_default ();
	enabled_plugins = gconf_client_get_list (client, "/apps/xchat/plugins/loaded", GCONF_VALUE_STRING, NULL);

	l = enabled_plugins;
	while (l != NULL) {
		if (g_str_has_suffix (l->data, "dbus.so")) {
			/* Remove the dbus plugin from the autoload list */
			GSList *tmp = l->next;
			enabled_plugins = g_slist_delete_link (enabled_plugins, l);
			l = tmp;
		} else {
			l = l->next;
		}
	}
	gconf_client_set_list (client, "/apps/xchat/plugins/loaded", GCONF_VALUE_STRING, enabled_plugins, NULL);

	g_object_unref (client);
}

static void
dbus_migration_check_file (void)
{
	gchar *msg;

	msg = g_strdup_printf (_("The way the D-Bus plugin works has changed.\nTo avoid problems, you should remove the old plugin file.\n\n<b>Please delete %s</b>"), XCHATLIBDIR "/plugins/dbus.so");
	if (g_file_test (XCHATLIBDIR "/plugins/dbus.so", G_FILE_TEST_EXISTS)) {
		error_dialog (_("D-Bus plugin is still installed"), msg);
	}
	g_free (msg);
}

static void
dbus_migration (void)
{
	/* Since version 0.14 dbus is statically linked and not a plugin anymore.
	 * We have to remove it from the autoload list to avoid a crash.
	 * The file has to be removed too so it won't be displayed in the plugins list. */
	if (check_version (0, 14, 0) == -1) {
		dbus_migration_remove_autoload ();
		dbus_migration_check_file ();
	}
}
