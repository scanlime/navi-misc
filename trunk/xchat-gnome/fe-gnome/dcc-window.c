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

#include <glib.h>
#include <libgnome/gnome-i18n.h>
#include <libgnomevfs/gnome-vfs-mime-utils.h>
#include <libgnomeui/gnome-icon-lookup.h>
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
dcc_window_add (DccWindow *window, struct DCC *dcc)
{
	GtkTreeIter iter;
	gint done = (gint) ((((float)dcc->pos) / ((float) dcc->size)) * 100);
	gchar *done_text, *info_text;

	GtkIconTheme *icon_theme;
	char *icon;
	char *mime;
	GdkPixbuf *mime_pixbuf, *file_icon;
	int mime_w, mime_h;
	int emblem_w, emblem_h;
	gpointer data;

	/* We don't care about DCC chats */
	if (dcc->type == 2)
		return;

	if (window->up_icon == NULL) {
		window->up_icon = gtk_widget_render_icon (GTK_WIDGET (window),
							  GTK_STOCK_GO_UP,
							  GTK_ICON_SIZE_SMALL_TOOLBAR,
							  NULL);
		window->down_icon = gtk_widget_render_icon (GTK_WIDGET (window),
							    GTK_STOCK_GO_DOWN,
							    GTK_ICON_SIZE_SMALL_TOOLBAR,
							    NULL);
	}

	done_text = g_strdup_printf ("%d %%", done);
	info_text = g_strdup_printf ("%s\nfrom %s\n%s of %s", dcc->file, dcc->nick, "x", "x");

	/* Get a nice MIME icon for this file */
	mime = gnome_vfs_get_mime_type (dcc->file);
	icon_theme = gtk_icon_theme_get_default ();
	icon = gnome_icon_lookup (icon_theme, NULL, NULL, NULL, NULL, mime, GNOME_ICON_LOOKUP_FLAGS_NONE, NULL);
	mime_pixbuf = gtk_icon_theme_load_icon (icon_theme, icon, 32, 0, NULL);

	mime_w   = gdk_pixbuf_get_width  (mime_pixbuf);
	mime_h   = gdk_pixbuf_get_height (mime_pixbuf);
	emblem_w = gdk_pixbuf_get_width  (window->up_icon);
	emblem_h = gdk_pixbuf_get_height (window->up_icon);

	data = g_new0 (char, (mime_w + 16) * (mime_h + 16) * 4 * 8);
	file_icon = gdk_pixbuf_new_from_data (data,
			GDK_COLORSPACE_RGB, TRUE, 8,
			mime_w + 16, mime_h + 16, (mime_w + 16) * 4,
			(GdkPixbufDestroyNotify) g_free, NULL);

	gdk_pixbuf_composite (mime_pixbuf, file_icon, 0, 0, mime_w, mime_h, 0.0, 0.0, 1.0, 1.0, GDK_INTERP_BILINEAR, 255);

	if (dcc->type == 1)
		gdk_pixbuf_composite (window->down_icon,
				      file_icon,
				      mime_w + 16 - emblem_w,
				      mime_h + 16 - emblem_h,
				      emblem_w, emblem_h,
				      0.0, 0.0,
				      1.0, 1.0,
				      GDK_INTERP_BILINEAR, 255);
	else
		gdk_pixbuf_composite (window->up_icon,
				      file_icon,
				      mime_w + 16 - emblem_w,
				      mime_h + 16 - emblem_h,
				      emblem_w, emblem_h,
				      0.0, 0.0,
				      1.0, 1.0,
				      GDK_INTERP_BILINEAR, 255);

	g_print ("MIME type is %s\nIcon is %s\n", mime, icon);

	g_free (icon);

	gtk_list_store_prepend (window->transfer_store, &iter);
	gtk_list_store_set (window->transfer_store, &iter,
			    0, dcc,
			    1, info_text,
			    2, file_icon,
			    3, done,
			    4, done_text,
			    5, "unknown",
			    -1);

	g_free (done_text);
	g_free (info_text);
	gdk_pixbuf_unref (mime_pixbuf);
	gdk_pixbuf_unref (file_icon);
}

void
dcc_window_update (DccWindow *window, struct DCC *dcc)
{
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (window->transfer_store), &iter) == FALSE)
		return;
	do {
		gpointer ptr;
		gtk_tree_model_get (GTK_TREE_MODEL (window->transfer_store), &iter, 0, &ptr, -1);
		if (ptr == dcc) {
			gint done = (gint) ((((float)dcc->pos) / ((float) dcc->size)) * 100);
			gchar *done_text = g_strdup_printf ("%d %%", done);
			gchar *info_text = g_strdup_printf ("%s\nfrom %s\n%s of %s", dcc->file, dcc->nick, "x", "x");

			gtk_list_store_set (window->transfer_store, &iter,
					    1, info_text,
					    3, done,
					    4, done_text,
					    -1);
			g_free (done_text);
			g_free (info_text);
			break;
		}
	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (window->transfer_store), &iter));
}

void
dcc_window_remove (DccWindow *window, struct DCC *dcc)
{
}
