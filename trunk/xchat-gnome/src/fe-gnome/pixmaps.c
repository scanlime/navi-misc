/*
 * pixmaps.c - helper functions for pixmaps
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#include <glib/gi18n.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "pixmaps.h"
#include "util.h"

GdkPixbuf *pix_op;
GdkPixbuf *pix_hop;
GdkPixbuf *pix_purple;
GdkPixbuf *pix_red;
GdkPixbuf *pix_voice;

static GdkPixbuf *
pixbuf_new_from_file (const gchar *file_name)
{
	GdkPixbuf *pixbuf;
	gchar *path;

	path = locate_data_file (file_name);
	pixbuf = gdk_pixbuf_new_from_file (path, NULL);
	g_free (path);

	return pixbuf;
}

void
pixmaps_init (void)
{
	GtkIconTheme *theme = gtk_icon_theme_get_default ();
	gchar *uninstalled_path = g_build_filename (TOPSRCDIR, "data", "icons", NULL);
	if (g_file_test (uninstalled_path, G_FILE_TEST_EXISTS)) {
		gtk_icon_theme_prepend_search_path (theme, uninstalled_path);
	}
	g_free (uninstalled_path);

	pix_op  = gtk_icon_theme_load_icon (theme, "xchat-gnome-status-operator", 16, 0, NULL);
	pix_hop = gtk_icon_theme_load_icon (theme, "xchat-gnome-status-halfop", 16, 0, NULL);
	pix_purple   = pixbuf_new_from_file ("purple.png");
	pix_red      = pixbuf_new_from_file ("red.png");
	pix_voice    = pixbuf_new_from_file ("voice.png");
}
