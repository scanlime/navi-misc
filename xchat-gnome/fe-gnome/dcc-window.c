/*
 * dcc-window.c - GtkWindow subclass for managing file transfers
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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

#include <libgnome/gnome-i18n.h>
#include "dcc-window.h"

static GtkWindowClass *parent_class;

static void
dcc_window_class_init (DccWindowClass *klass)
{
}

static void
dcc_window_init (DccWindow *window)
{
	GladeXML *xml;

	xml = NULL;
	if (g_file_test ("dcc-window.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("dcc-window.glade", "toplevel", NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/dcc-window.glade", "toplevel", NULL);
	if (!xml)
		return;

#define GW(name) ((window->name) = glade_xml_get_widget (xml, #name))
	GW(transfer_list);
	GW(stop_button);
	GW(toplevel);
#undef GW

	g_object_unref (xml);

	window->transfer_store = gtk_list_store_new (7,
		G_TYPE_POINTER,		/* DCC struct */
		G_TYPE_STRING,		/* MIME type */
		G_TYPE_STRING,		/* nickname */
		GDK_TYPE_PIXBUF,	/* File icon */
#ifdef USE_DCC64
		G_TYPE_INT64,		/* Size */
		G_TYPE_INT64,		/* Position */
#else
		G_TYPE_INT,		/* Size */
		G_TYPE_INT,		/* Position */
#endif
		G_TYPE_STRING		/* filename */
		);

	gtk_tree_view_set_model (GTK_TREE_VIEW (window->transfer_list), GTK_TREE_MODEL (window->transfer_store));

	gtk_container_set_border_width (GTK_CONTAINER (window), 12);
	gtk_container_add (GTK_CONTAINER (window), window->toplevel);

	gtk_window_set_default_size (GTK_WINDOW (window), 300, 400);
	gtk_window_set_title (GTK_WINDOW (window), _("File Transfers"));
}

GType
dcc_window_get_type (void)
{
	static GType dcc_window_type = 0;
	if (!dcc_window_type) {
		static const GTypeInfo dcc_window_info = {
			sizeof (DccWindowClass),
			NULL, NULL,
			(GClassInitFunc) dcc_window_class_init,
			NULL, NULL,
			sizeof (DccWindow),
			0,
			(GInstanceInitFunc) dcc_window_init,
		};

		dcc_window_type = g_type_register_static (GTK_TYPE_WINDOW, "DccWindow", &dcc_window_info, 0);

		parent_class = g_type_class_ref (GTK_TYPE_WINDOW);
	}

	return dcc_window_type;
}

static gboolean
dcc_window_delete_event (GtkWidget *window, GdkEvent *event, gpointer data)
{
	gtk_widget_hide (window);
	return TRUE;
}

DccWindow *
dcc_window_new ()
{
	DccWindow *window = g_object_new (dcc_window_get_type (), 0);
	if (window->toplevel == NULL) {
		g_object_unref (window);
		return NULL;
	}

	g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (dcc_window_delete_event), NULL);

	return window;
}

void
dcc_window_add (struct DCC *dcc)
{
}

void
dcc_window_update (struct DCC *dcc)
{
}

void
dcc_window_remove (struct DCC *dcc)
{
}
