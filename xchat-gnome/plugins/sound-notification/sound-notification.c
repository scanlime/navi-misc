/*
 * sound-notification.c - A sound notification plugin for xchat-gnome.
 *
 * Copyright (C) 2006 Guillaume Desmottes
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
#include <gtk/gtk.h>
#include <libgnome/libgnome.h>

#include "plugins.h"
#include "xchat-plugin.h"
#include "xg-plugin.h"

#define SOUND_NOTIFICATION_VERSION "0.1"

void xchat_plugin_get_info   (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved);
int  xchat_plugin_init       (xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg);
int  xchat_plugin_deinit     (void);
int  xchat_gnome_plugin_init (xchat_gnome_plugin *xg_plugin);

static xchat_gnome_plugin* xgph;                /* xchat gnome plugin handle. */
static xchat_plugin*       ph;                  /* Plugin handle. */

/*** Callbacks ***/
static int
new_msg_cb (char **word, void *msg_lvl)
{
	if (g_file_test ("../../data/message.wav", G_FILE_TEST_EXISTS))
		gnome_sound_play ("../../data/message.wav");
	else
		gnome_sound_play (XCHATSHAREDIR "/message.wav");
	return 0;
}

/*** xchat-gnome plugin functions ***/
void
xchat_plugin_get_info (char **plugin_name, char **plugin_desc, char **plugin_version, void **reserved)
{
	*plugin_name = _("Sound Notification");
	*plugin_desc = _("Play a sound when you receive a private message or your nick is highlighted.");
	*plugin_version = SOUND_NOTIFICATION_VERSION;

	if (reserved)
		*reserved = NULL;
}

int
xchat_gnome_plugin_init (xchat_gnome_plugin * xg_plugin)
{
	xgph = xg_plugin;

	return 1;
}

/*** xchat plugin functions ***/
int
xchat_plugin_init (xchat_plugin * plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	ph = plugin_handle;

	gnome_sound_init ("localhost");

	/* Set the plugin info. */
	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version, NULL);

	/* Hook up our callbacks. */
	xchat_hook_print (ph, "Channel Msg Hilight",		XCHAT_PRI_NORM, new_msg_cb, NULL);
	xchat_hook_print (ph, "Channel Action Hilight",		XCHAT_PRI_NORM, new_msg_cb, NULL);
	xchat_hook_print (ph, "Private Message to Dialog",	XCHAT_PRI_NORM, new_msg_cb, NULL);

	xchat_print (ph, _("Sound-notification plugin loaded.\n"));

	return TRUE;
}

int
xchat_plugin_deinit (void)
{
	gnome_sound_shutdown ();
	xchat_print (ph, _("Sound-notification plugin unloaded.\n"));

	return 1;
}
