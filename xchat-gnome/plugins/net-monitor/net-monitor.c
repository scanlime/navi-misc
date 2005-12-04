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

#include <config.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <string.h>
#include "xchat-plugin.h"

#define NET_MONITOR_VERSION     "0.1"
#define NET_MONITOR_NAME        _("Network Monitor")
#define NET_MONITOR_DESCRIPTION _("NetworkManager connection monitor")
#define NM_SERVICE              "org.freedesktop.NetworkManager"
#define NM_OBJECT_PATH          "/org/freedesktop/NetworkManager"
#define NM_INTERFACE            "org.freedesktop.NetworkManager"

void xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init     (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit   (void);

typedef enum
{
	NETWORK_UP,
	NETWORK_DOWN,
} NetworkStatus;

typedef enum
{
	NM_STATE_UNKNOWN = 0,
	NM_STATE_ASLEEP,
	NM_STATE_CONFIGURE_AP,
	NM_STATE_CONFIGURE_DEV,
	NM_STATE_CONFIGURE_IP,
	NM_STATE_CONNECTED,
	NM_STATE_DISCONNECTED,
} NetworkManagerState;

enum {
	SESSION_TYPE_SERVER = 1,
	SESSION_TYPE_CHANNEL,
	SESSION_TYPE_DIALOG,
};

static xchat_plugin   *ph;
static DBusConnection *bus;
static GHashTable     *networks = NULL;
static GHashTable     *channels = NULL;

static int
connected_cb (char *word[], gpointer data)
{
	GList *chans, *chan;
	const char *server;
	xchat_context *context;

	if (channels == NULL)
		return XCHAT_EAT_NONE;

	server = xchat_get_info (ph, "server");

	context = g_hash_table_lookup (networks, server);
	if (context) {
		xchat_set_context (ph, context);

		chans = g_hash_table_lookup (channels, server);
		for (chan = chans; chan; chan = g_list_next (chan)) {
			xchat_commandf (ph, "JOIN %s", chan->data);
		}

		g_list_foreach (chans, (GFunc) g_free, NULL);
		g_list_free (chans);

		/* Now that we've connected and re-joined, we can remove this server
		 * from the list */
		g_hash_table_remove (networks, server);
		g_hash_table_remove (channels, server);
	}

	return XCHAT_EAT_NONE;
}

static void
connect_to_network (gchar *key, xchat_context *context, gpointer data)
{
	xchat_set_context (ph, context);
	xchat_commandf (ph, "SERVER %s", key);
}

static void
disconnect_from_network (gchar *key, xchat_context *context, gpointer data)
{
	xchat_set_context (ph, context);
	xchat_command (ph, "DISCON");
}

static void
set_network_mode (NetworkStatus status)
{
	xchat_list *channels_list;

	if (status == NETWORK_DOWN) {
		/* Store a list of currently connected networks & channels,
		 * so we can restore the previous state when the network
		 * becomes active again */
		channels_list = xchat_list_get (ph, "channels");
		if (channels_list == NULL)
			return;

		if (channels) g_hash_table_destroy (channels);
		if (networks) g_hash_table_destroy (networks);

		channels = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
		networks = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

		while (xchat_list_next (ph, channels_list)) {
			const gchar *channel, *server;
			gint type;

			channel = xchat_list_str (ph, channels_list, "channel");
			server  = xchat_list_str (ph, channels_list, "server");
			type    = xchat_list_int (ph, channels_list, "type");

			if (type == SESSION_TYPE_SERVER) {
				xchat_context *context = xchat_find_context (ph, server, channel);
				g_hash_table_insert (networks, (gpointer) g_strdup (server), context);
			} else if (type == SESSION_TYPE_CHANNEL) {
				gboolean exists;
				GList *network_channels;

				network_channels = g_hash_table_lookup (channels, server);
				exists = (network_channels != NULL);

				network_channels = g_list_prepend (network_channels, g_strdup (channel));

				if (exists)
					g_hash_table_replace (channels, (gpointer) g_strdup (server), network_channels);
				else
					g_hash_table_insert (channels, (gpointer) g_strdup (server), network_channels);
			}
		}

		g_hash_table_foreach (networks, (GHFunc) disconnect_from_network, NULL);
	} else {
		g_hash_table_foreach (networks, (GHFunc) connect_to_network, NULL);
	}
}

static NetworkStatus
check_device_state (const char *path)
{
	DBusMessage     *message, *reply;
	DBusMessageIter  iter;
	DBusError        error;
	NetworkStatus    network_mode;

	message = dbus_message_new_method_call (NM_SERVICE, path, NM_INTERFACE, "getLinkActive");
	if (message == NULL) {
		g_warning ("Net Monitor: Couldn't allocate dbus message : %s: %s\n", error.name, error.message);
		/* just guess that we're up */
		return NETWORK_UP;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (bus, message, -1, &error);

	if (dbus_error_is_set (&error)) {
		g_warning ("Net Monitor: Error retrieving device state: %s: %s\n", error.name, error.message);
		network_mode = NETWORK_UP;
	} else {
		dbus_message_iter_init (reply, &iter);

		if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_BOOLEAN) {
			dbus_bool_t active;

			dbus_message_iter_get_basic (&iter, &active);
			network_mode = active ? NETWORK_UP : NETWORK_DOWN;
		} else {
			g_warning ("Net Monitor: got bad reply from NetworkManager\n");
			network_mode = NETWORK_UP;
		}
	}

	if (reply)
		dbus_message_unref (reply);

	if (message)
		dbus_message_unref (message);

	return network_mode;
}

static void
determine_network_status ()
{
	DBusMessage     *message, *reply;
	DBusMessageIter  iter;
	DBusError        error;
	NetworkStatus    network_mode = NETWORK_DOWN;

	message = dbus_message_new_method_call (NM_SERVICE, NM_OBJECT_PATH, NM_INTERFACE, "getDevices");
	if (message == NULL) {
		g_warning ("Net Monitor: Couldn't allocate dbus message : %s: %s\n", error.name, error.message);
		/* just guess that we're up */
		set_network_mode (NETWORK_UP);
		return;
	}

	dbus_error_init (&error);
	reply = dbus_connection_send_with_reply_and_block (bus, message, -1, &error);

	if (dbus_error_is_set (&error)) {
		g_warning ("Net Monitor: Error retrieving devices: %s: %s\n", error.name, error.message);
		network_mode = NETWORK_UP;
	} else {
		dbus_message_iter_init (reply, &iter);
		if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_ARRAY) {
			DBusMessageIter child;

			dbus_message_iter_recurse (&iter, &child);

			while (dbus_message_iter_get_arg_type (&child) != DBUS_TYPE_INVALID) {
				const char *path;
				gboolean state;

				/* Check the state of this individual device.  If any one
				 * device is active, the network is up */
				dbus_message_iter_get_basic (&child, &path);
				state = check_device_state (path);
				if (state == NETWORK_UP)
					network_mode = NETWORK_UP;

				dbus_message_iter_next (&child);
			}
		} else {
			g_warning ("Net Monitor: got bad reply from NetworkManager\n");
			network_mode = NETWORK_UP;
		}
	}

	set_network_mode (network_mode);

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
		g_error ("Net Monitor: Couldn't connect to system bus : %s: %s\n", error.name, error.message);
		return FALSE;
	}

	dbus_connection_add_filter (bus, filter_func, NULL, NULL);
	dbus_bus_add_match (bus, "type='signal',interface='" NM_INTERFACE "'", &error);

	if (dbus_error_is_set (&error)) {
		g_error ("Net Monitor: Could not register signal handler: %s: %s\n", error.name, error.message);
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
xchat_plugin_init (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	gboolean success;

	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	ph = plugin_handle;

	success = init_dbus ();
	if (success) {
		xchat_hook_print (ph, "Motd",         XCHAT_PRI_NORM, connected_cb, NULL);
		xchat_hook_print (ph, "MOTD Skipped", XCHAT_PRI_NORM, connected_cb, NULL);

		xchat_set_context (ph, xchat_find_context (ph, NULL, NULL));
		xchat_printf (ph, _("%s loaded successfully\n"), NET_MONITOR_NAME);
	}
	return success;
}

int
xchat_plugin_deinit (void)
{
	return TRUE;
}
