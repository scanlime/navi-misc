/*
 * net-monitor.c - xchat plugin which uses NetworkManager's D-BUS interface
 *                 to control connection state
 *
 * Copyright (C) 2005 David Trowbridge
 * Parts of this code were copied from epiphany extensions.
 * Those parts are Copyright (C) 2005 Jean-François Rameau
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

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <glib.h>
#include <glib/gi18n.h>
#include "xchat-plugin.h"

#define NET_MONITOR_VERSION     "0.1"
#define NET_MONITOR_NAME        "Network Monitor"
#define NET_MONITOR_DESCRIPTION "NetworkManager connection monitor"
#define NM_SERVICE              "org.freedesktop.NetworkManager"
#define NM_OBJECT_PATH          "/org/freedesktop/NetworkManager"
#define NM_INTERFACE            "org.freedesktop.NetworkManager"

static xchat_plugin    *ph;
static DBusConnection  *bus;

static DBusHandlerResult
filter_func (DBusConnection *connection, DBusMessage *message, void *data)
{
	xchat_printf (ph, "filter_func\n");
	if (dbus_message_is_signal (message, NM_INTERFACE, "DeviceNoLongerActive")) {
		xchat_printf (ph, "network down\n");
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (dbus_message_is_signal (message, NM_INTERFACE, "DeviceNowActive")) {
		xchat_printf (ph, "network up\n");
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static gboolean
init_dbus ()
{
	DBusError error;

	dbus_error_init (&error);
	bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);

	if (dbus_error_is_set (&error)) {
		xchat_printf (ph, _("Couldn't connect to system bus : %s: %s\n"), error.name, error.message);
		return FALSE;
	}

	dbus_connection_add_filter (bus, filter_func, NULL, NULL);
	dbus_bus_add_match (bus, "type='signal',interface='" NM_INTERFACE "'", &error);

	if (dbus_error_is_set (&error)) {
		xchat_printf (ph, _("Could not register signal handler: %s: %s\n"), error.name, error.message);
		return FALSE;
	}

	return TRUE;
}

void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved)
{
	*plugin_name    = NET_MONITOR_NAME;
	*plugin_desc    = NET_MONITOR_DESCRIPTION;
	*plugin_version = NET_MONITOR_VERSION;

	if (reserved)
		*reserved = NULL;
}

int
xchat_plugin_init (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	gboolean success;

	*plugin_name    = NET_MONITOR_NAME;
	*plugin_desc    = NET_MONITOR_DESCRIPTION;
	*plugin_version = NET_MONITOR_VERSION;

	ph = plugin_handle;

	success = init_dbus ();
	if (success)
		xchat_printf (ph, _(NET_MONITOR_NAME " loaded successfully\n"));
	return success;
}

int
xchat_plugin_deinit ()
{
	return TRUE;
}
