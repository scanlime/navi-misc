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

	window->transfer_store = gtk_list_store_new (6,
		G_TYPE_POINTER,		/* DCC struct */
		G_TYPE_STRING,		/* Info text */
		GDK_TYPE_PIXBUF,	/* File icon */
		G_TYPE_INT,		/* % done */
		G_TYPE_STRING,		/* % done label */
		G_TYPE_STRING		/* time remaining */
		);

	gtk_tree_view_set_model (GTK_TREE_VIEW (window->transfer_list), GTK_TREE_MODEL (window->transfer_store));

	gtk_container_set_border_width (GTK_CONTAINER (window), 12);
	gtk_container_add (GTK_CONTAINER (window), window->toplevel);

	gtk_window_set_default_size (GTK_WINDOW (window), 300, 400);
	gtk_window_set_title (GTK_WINDOW (window), _("File Transfers"));

	window->progress_column = gtk_tree_view_column_new ();
	window->info_column = gtk_tree_view_column_new ();
	window->remaining_column = gtk_tree_view_column_new ();

	window->progress_cell = gtk_cell_renderer_progress_new ();
	gtk_tree_view_column_pack_start (window->progress_column, window->progress_cell, FALSE);
	gtk_tree_view_column_add_attribute (window->progress_column, window->progress_cell, "value", 3);
	gtk_tree_view_column_add_attribute (window->progress_column, window->progress_cell, "text", 4);

	window->icon_cell = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (window->info_column, window->icon_cell, FALSE);
	gtk_tree_view_column_add_attribute (window->info_column, window->icon_cell, "pixbuf", 2);

	window->info_cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (window->info_column, window->info_cell, TRUE);
	gtk_tree_view_column_add_attribute (window->info_column, window->info_cell, "markup", 1);

	window->remaining_cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (window->remaining_column, window->remaining_cell, FALSE);
	gtk_tree_view_column_add_attribute (window->remaining_column, window->remaining_cell, "text", 5);

	gtk_tree_view_column_set_title (window->progress_column, "%");
	gtk_tree_view_column_set_title (window->info_column, "File");
	gtk_tree_view_column_set_title (window->remaining_column, "Remaining");

	gtk_tree_view_append_column (GTK_TREE_VIEW (window->transfer_list), window->progress_column);
	gtk_tree_view_append_column (GTK_TREE_VIEW (window->transfer_list), window->info_column);
	gtk_tree_view_append_column (GTK_TREE_VIEW (window->transfer_list), window->remaining_column);
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
