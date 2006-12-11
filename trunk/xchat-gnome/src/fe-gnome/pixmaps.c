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
#include <gtk/gtkicontheme.h>
#include "pixmaps.h"

void
pixmaps_init (void)
{
	/* FIXME multi-head! */
	GtkIconTheme *theme = gtk_icon_theme_get_default ();
	gchar *uninstalled_path = g_build_filename (TOPSRCDIR, "data", "icons", NULL);
	if (g_file_test (uninstalled_path, G_FILE_TEST_EXISTS)) {
		gtk_icon_theme_prepend_search_path (theme, uninstalled_path);
	} else {
		gtk_icon_theme_append_search_path (theme, ICONDIR);
	}
	g_free (uninstalled_path);
}
