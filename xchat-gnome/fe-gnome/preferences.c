/*
 * preferences.h - interface to storing preferences
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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
#include "preferences.h"
#include "palette.h"
#include "../common/xchatc.h"

gboolean preferences_exist() {
	GConfClient *client;
	char *text;

	client = gconf_client_get_default();

	text = gconf_client_get_string(client, "/apps/xchat/version", NULL);
	if(text == NULL)
		return FALSE;
	else
		g_free(text);

	/* probably eventually do some checking on the version to migrate from
	   previous versions when new features are added */
	return TRUE;
}

void load_preferences() {
	GConfClient *client;
	int color_scheme, palette_scheme;
	char *text;

	client = gconf_client_get_default();

	text = gconf_client_get_string(client, "/apps/xchat/irc/nickname", NULL);
	strcpy(prefs.nick1, text);
	g_free(text);

	text = gconf_client_get_string(client, "/apps/xchat/irc/realname", NULL);
	strcpy(prefs.realname, text);
	g_free(text);

	text = gconf_client_get_string(client, "/apps/xchat/irc/awaymsg", NULL);
	strcpy(prefs.awayreason, text);
	g_free(text);

	text = gconf_client_get_string(client, "/apps/xchat/irc/quitmsg", NULL);
	strcpy(prefs.quitreason, text);
	g_free(text);

	text = gconf_client_get_string(client, "/apps/xchat/irc/partmsg", NULL);
	strcpy(prefs.partreason, text);
	g_free(text);

	color_scheme = gconf_client_get_int(client, "/apps/xchat/irc/color_scheme", NULL);
	load_colors(color_scheme);
	palette_scheme = gconf_client_get_int(client, "/apps/xchat/irc/palette_scheme", NULL);
	load_palette(palette_scheme);
}

char *preferences_nickname() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_string(client, "/apps/xchat/irc/nickname", NULL);
}

char *preferences_realname() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_string(client, "/apps/xchat/irc/realname", NULL);
}

char *preferences_quitmsg() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_string(client, "/apps/xchat/irc/quitmsg", NULL);
}

char *preferences_partmsg() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_string(client, "/apps/xchat/irc/partmsg", NULL);
}

char *preferences_awaymsg() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_string(client, "/apps/xchat/irc/awaymsg", NULL);
}

gboolean preferences_show_timestamp() {
	GConfClient *client;

	client = gconf_client_get_default();

	return gconf_client_get_bool(client, "/apps/xchat/irc/showtimestamps", NULL);
}

int preferences_get_color_scheme() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_int(client, "/apps/xchat/irc/color_scheme", NULL);
}

int preferences_get_palette_scheme() {
	GConfClient *client;
	client = gconf_client_get_default();
	return gconf_client_get_int(client, "/apps/xchat/irc/palette_scheme", NULL);
}

void preferences_set_color_scheme(int selection) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/irc/color_scheme", selection, NULL);
}

void preferences_set_palette_scheme(int selection) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/irc/palette_scheme", selection, NULL);
}

void preferences_set_main_window_size(int width, int height) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/main_window/height", height, NULL);
	gconf_client_set_int(client, "/apps/xchat/main_window/width", width, NULL);
}

void preferences_set_channel_list_window_size(int width, int height) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/channel_list_window/height", height, NULL);
	gconf_client_set_int(client, "/apps/xchat/channel_list_window/width", width, NULL);
}

void preferences_get_main_window_size(int *width, int *height) {
	GConfClient *client;
	client = gconf_client_get_default();
	*height = gconf_client_get_int(client, "/apps/xchat/main_window/height", NULL);
	*width = gconf_client_get_int(client, "/apps/xchat/main_window/width", NULL);
}

void preferences_get_channel_list_window_size(int *width, int *height) {
	GConfClient *client;
	client = gconf_client_get_default();
	*height = gconf_client_get_int(client, "/apps/xchat/channel_list_window/height", NULL);
	*width = gconf_client_get_int(client, "/apps/xchat/channel_list_window/width", NULL);
}

void preferences_set_main_window_v_position(int v) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/main_window/vpane", v, NULL);
}

void preferences_set_main_window_h_position(int h) {
	GConfClient *client;
	client = gconf_client_get_default();
	gconf_client_set_int(client, "/apps/xchat/main_window/hpane", h, NULL);
}

void preferences_get_main_window_positions(int *v, int *h) {
	GConfClient *client;
	client = gconf_client_get_default();
	*v = gconf_client_get_int(client, "/apps/xchat/main_window/vpane", NULL);
	*h = gconf_client_get_int(client, "/apps/xchat/main_window/hpane", NULL);
}
