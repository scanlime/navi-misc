/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc. (www.novell.com)
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
#include <libedataserver/e-source.h>
#include <libedataserver/e-source-list.h>
#include <libecal/e-cal.h>
#include "publish-format-ical.h"

void
publish_calendar_as_ical (GnomeVFSHandle *handle, EPublishUri *uri)
{
	GSList *l;
	ESourceList *source_list;
	GConfClient *gconf_client;

	gconf_client = gconf_client_get_default ();

	/* events */
	source_list = e_source_list_new_for_gconf (gconf_client, "/apps/evolution/calendar/sources");
	l = uri->events;
	while (l) {
		gchar *uid = l->data;
		ESource *source;
		ECal *client = NULL;

		source = e_source_list_peek_source_by_uid (source_list, uid);
		if (source)
			client = auth_new_cal_from_source (source, E_CAL_SOURCE_TYPE_EVENT);
		if (!client) {
			g_warning (G_STRLOC ": Could not publish calendar: Calendar backend no longer exists");
			continue;
		}

		e_cal_open (client, NULL, NULL);

		g_object_unref (client);
		l = g_slist_next (l);
	}
	g_object_unref (source_list);

	/* tasks */
	source_list = e_source_list_new_for_gconf (gconf_client, "/apps/evolution/tasks/sources");
	l = uri->tasks;
	while (l) {
		gchar *uid = l->data;
		g_print ("publishing task '%s'\n", uid);

		l = g_slist_next (l);
	}
	g_object_unref (source_list);

	g_object_unref (gconf_client);
}
