/*
 * Authors:
 *  David Trowbridge <trowbrds@cs.colorado.edu>
 *  Gary Ekker <gekker@novell.com>
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
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

#include "publish-location.h"
#include <libxml/tree.h>
#include <string.h>

EPublishUri *
e_publish_uri_from_xml (const gchar *xml)
{
	xmlDocPtr doc;
	xmlNodePtr root, p;
	xmlChar *location, *enabled, *frequency;
	xmlChar *username, *publish_time;
	GSList *l = NULL;
	EPublishUri *uri;

	doc = xmlParseDoc ((char *) xml);
	if (doc == NULL)
		return NULL;

	root = doc->children;
	if (strcmp (root->name, "uri") != 0)
		return NULL;

	uri = g_new0 (EPublishUri, 1);

	location = xmlGetProp (root, "location");
	enabled = xmlGetProp (root, "enabled");
	frequency = xmlGetProp (root, "frequency");
	username = xmlGetProp (root, "username");
	publish_time = xmlGetProp (root, "publish_time");

	if (location != NULL)
		uri->location = location;
	if (enabled != NULL)
		uri->enabled = atoi (enabled);
	if (frequency != NULL)
		uri->publish_frequency = atoi (frequency);
	if (username != NULL)
		uri->username = username;
	if (publish_time != NULL)
		uri->last_pub_time = publish_time;

	uri->password = g_strdup ("");

	for (p = root->children; p != NULL; p = p->next) {
		xmlChar *uid = xmlGetProp (p, "uid");
		l = g_slist_append (l, uid);
	}
	uri->calendars = l;

	xmlFree (enabled);
	xmlFree (frequency);
	xmlFreeDoc (doc);

	return uri;
}

gchar *
e_publish_uri_to_xml (EPublishUri *uri)
{
}
