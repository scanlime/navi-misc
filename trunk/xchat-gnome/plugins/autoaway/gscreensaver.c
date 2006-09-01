/* gscreensaver.c
 *
 * Copyright (C) 2005 Isak Savo <isak.savo@gmail.com>
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
#include "autoaway.h"
#include "gscreensaver.h"

#ifdef ENABLE_DBUS
#define DBUS_API_SUBJECT_TO_CHANGE
# include <dbus/dbus-glib.h>
# include <dbus/dbus.h>

static DBusGProxy *dbus_proxy = NULL;
#endif /* ENABLE_DBUS */

static gboolean screensaver_is_active = FALSE;


#ifdef ENABLE_DBUS
static void
screensaver_changed_cb (DBusGProxy *proxy, gboolean value, gpointer user_data)
{
	screensaver_is_active = value;
}
#endif

gboolean
get_gs_has_ipc (void)
{
#ifdef ENABLE_DBUS
	gboolean mybool;
	if (!dbus_g_proxy_call (dbus_proxy, "getActive", NULL,
				G_TYPE_INVALID,
				G_TYPE_BOOLEAN, &mybool,
				G_TYPE_INVALID)) {
		return FALSE;
	}
	return TRUE;
#else
	return FALSE;
#endif /* ENABLE_DBUS */
}

void
init_gs_connection (void)
{
#ifdef ENABLE_DBUS
	static DBusGConnection *connection = NULL;
	GError *err = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &err);
	if (!connection) {
		g_printerr ("Error getting bus: %s\n", err->message);
		g_error_free (err);
		return;
	}
	dbus_proxy = dbus_g_proxy_new_for_name (connection,
						"org.gnome.ScreenSaver",
						"/org/gnome/ScreenSaver",
						"org.gnome.ScreenSaver");
	if (!dbus_proxy) {
		g_printerr ("Couldn't create a dbus proxy to gnome-screensaver\n");
		return;
	}

	dbus_g_proxy_add_signal (dbus_proxy, "ActiveChanged", G_TYPE_BOOLEAN, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (dbus_proxy, "ActiveChanged", G_CALLBACK (screensaver_changed_cb), NULL, NULL);

	/* Let's just pretend this will not fail */
	dbus_g_proxy_call (dbus_proxy, "getActive", NULL,
			   G_TYPE_INVALID,
			   G_TYPE_BOOLEAN, &screensaver_is_active,
			   G_TYPE_INVALID);
#endif /* ENABLE_DBUS */
}

void
close_gs_connection (void)
{
#ifdef ENABLE_DBUS
	if (dbus_proxy)
		g_object_unref (dbus_proxy);
#endif
}

gboolean
get_gs_screensaver_active (void)
{
	return screensaver_is_active;
}
