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
#include <dbus/dbus-glib.h>
#include <glib.h>
#include <glib/gi18n.h>
#include "xchat-plugin.h"

#define NET_MONITOR_VERSION     "0.1"
#define NET_MONITOR_NAME        "Network Monitor"
#define NET_MONITOR_DESCRIPTION "NetworkManager connection monitor"
#define NM_SERVICE              "org.freedesktop.NetworkManager"
#define NM_OBJECT_PATH          "/org/freedesktop/NetworkManager"
#define NM_INTERFACE            "org.freedesktop.NetworkManager"

#define NM_NO_ACTIVE_DEVICE     "org.freedesktop.NetworkManager.NoActiveDevice"

typedef enum
{
	NETWORK_UP,
	NETWORK_DOWN,
} NetworkStatus;

static xchat_plugin    *ph;
static DBusConnection  *bus;

static void
set_network_mode (NetworkStatus status)
{
	if (status == NETWORK_UP)
		g_print ("network up!\n");
	else
		g_print ("network down!\n");
}

static void
determine_network_status ()
{
	DBusMessage *message, *reply;
	DBusError error;

	message = dbus_message_new_method_call (NM_SERVICE, NM_OBJECT_PATH, NM_INTERFACE, "getActiveDevice");
	if (message == NULL) {
		g_warning (_("Net Monitor: Couldn't allocate dbus message : %s: %s\n"), error.name, error.message);
		/* just guess that we're up */
		set_network_mode (NETWORK_UP);
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (bus, message, -1, &error);

	if (dbus_error_is_set (&error)) {
		if (dbus_error_has_name (&error, NM_NO_ACTIVE_DEVICE)) {
			set_network_mode (NETWORK_DOWN);
		} else {
			g_warning (_("Net Monitor: Can't talk to Network Manager: %s: %s\n"), error.name, error.message);
			set_network_mode (NETWORK_UP);
		}
	} else {
		if (reply == NULL)
			g_warning ("Network Monitor plugin got NULL reply from NetworkManager");
		set_network_mode (NETWORK_UP);
	}

	if (reply)
		dbus_message_unref (reply);

	if (message)
		dbus_message_unref (message);
}

static DBusHandlerResult
filter_func (DBusConnection *connection, DBusMessage *message, void *data)
{
	if (dbus_message_is_signal (message, NM_INTERFACE, "DeviceNoLongerActive")) {
		determine_network_status ();
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	if (dbus_message_is_signal (message, NM_INTERFACE, "DeviceNowActive")) {
		set_network_mode (NETWORK_UP);
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

	dbus_connection_setup_with_g_main (bus, NULL);

	if (dbus_error_is_set (&error)) {
		xchat_printf (ph, _("Net Monitor: Couldn't connect to system bus : %s: %s\n"), error.name, error.message);
		return FALSE;
	}

	dbus_connection_add_filter (bus, filter_func, NULL, NULL);
	dbus_bus_add_match (bus, "type='signal',interface='" NM_INTERFACE "'", &error);

	if (dbus_error_is_set (&error)) {
		xchat_printf (ph, _("Net Monitor: Could not register signal handler: %s: %s\n"), error.name, error.message);
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

	xchat_set_context (ph, xchat_find_context (ph, NULL, NULL));

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
