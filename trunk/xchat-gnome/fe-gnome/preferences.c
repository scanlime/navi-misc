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
#include "../common/xchatc.h"

gboolean preferences_exist() {
	GConfClient *client;

	client = gconf_client_get_default();

	if(gconf_client_get_string(client, "/apps/xchat/version", NULL) == NULL)
		return FALSE;
	
	/* probably eventually do some checking on the version to migrate from
	   previous versions when new features are added */
	return TRUE;
}

void load_preferences() {
	GConfClient *client;

	client = gconf_client_get_default();

	strcpy(prefs.nick1, gconf_client_get_string(client, "/apps/xchat/irc/nickname", NULL));
	strcpy(prefs.realname, gconf_client_get_string(client, "/apps/xchat/irc/realname", NULL));
	strcpy(prefs.awayreason, gconf_client_get_string(client, "/apps/xchat/irc/awaymsg", NULL));
	strcpy(prefs.quitreason, gconf_client_get_string(client, "/apps/xchat/irc/quitmsg", NULL));
	strcpy(prefs.partreason, gconf_client_get_string(client, "/apps/xchat/irc/partmsg", NULL));
}

char *preferences_nickname(struct ircnet *net) {
	GConfClient *client;

	client = gconf_client_get_default();

	if(net == NULL) {
		return gconf_client_get_string(client, "/apps/xchat/irc/nickname", NULL);
	}
	/* FIXME: check to see if there's a per-server pref for this */
	return gconf_client_get_string(client, "/apps/xchat/irc/nickname", NULL);
}

gboolean preferences_show_timestamp() {
	GConfClient *client;

	client = gconf_client_get_default();

	return gconf_client_get_bool(client, "/apps/xchat/irc/showtimestamps", NULL);
}
