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
	xmlChar *publish_time, *format;
	GSList *events = NULL, *tasks = NULL;
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
	format = xmlGetProp (root, "format");
	publish_time = xmlGetProp (root, "publish_time");

	if (location != NULL)
		uri->location = location;
	if (enabled != NULL)
		uri->enabled = atoi (enabled);
	if (frequency != NULL)
		uri->publish_frequency = atoi (frequency);
	if (format != NULL)
		uri->publish_format = atoi (format);
	if (publish_time != NULL)
		uri->last_pub_time = publish_time;

	uri->password = g_strdup ("");

	for (p = root->children; p != NULL; p = p->next) {
		xmlChar *uid = xmlGetProp (p, "uid");
		if (strcmp (p->name, "event") == 0)
			events = g_slist_append (events, uid);
		else if (strcmp (p->name, "task") == 0)
			tasks = g_slist_append (tasks, uid);
		else
			g_free (uid);
	}
	uri->events = events;
	uri->tasks = tasks;

	xmlFree (enabled);
	xmlFree (frequency);
	xmlFree (format);
	xmlFreeDoc (doc);

	return uri;
}

gchar *
e_publish_uri_to_xml (EPublishUri *uri)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	gchar *enabled, *frequency, *format;
	GSList *calendars = NULL;
	xmlChar *xml_buffer;
	char *returned_buffer;
	int xml_buffer_size;

	g_return_val_if_fail (uri != NULL, NULL);
	g_return_val_if_fail (uri->location != NULL, NULL);

	doc = xmlNewDoc ("1.0");

	root = xmlNewDocNode (doc, NULL, "uri", NULL);
	enabled = g_strdup_printf ("%d", uri->enabled);
	frequency = g_strdup_printf ("%d", uri->publish_frequency);
	format = g_strdup_printf ("%d", uri->publish_format);
	xmlSetProp (root, "location", uri->location);
	xmlSetProp (root, "enabled", enabled);
	xmlSetProp (root, "frequency", frequency);
	xmlSetProp (root, "format", format);
	xmlSetProp (root, "publish_time", uri->last_pub_time);

	for (calendars = uri->events; calendars != NULL; calendars = g_slist_next (calendars)) {
		xmlNodePtr node;
		node = xmlNewChild (root, NULL, "event", NULL);
		xmlSetProp (node, "uid", calendars->data);
	}
	for (calendars = uri->tasks; calendars != NULL; calendars = g_slist_next (calendars)) {
		xmlNodePtr node;
		node = xmlNewChild (root, NULL, "task", NULL);
		xmlSetProp (node, "uid", calendars->data);
	}
	xmlDocSetRootElement (doc, root);

	xmlDocDumpMemory (doc, &xml_buffer, &xml_buffer_size);
	xmlFreeDoc (doc);

	returned_buffer = g_malloc (xml_buffer_size + 1);
	memcpy (returned_buffer, xml_buffer, xml_buffer_size);
	returned_buffer[xml_buffer_size] = '\0';
	xmlFree (xml_buffer);
	g_free (enabled);
	g_free (frequency);
	g_free (format);

	return returned_buffer;
}
