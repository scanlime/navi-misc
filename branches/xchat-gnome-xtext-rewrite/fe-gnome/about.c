/*
 * about.c - utilities for displaying and hiding the about dialog
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

#include "about.h"

void on_about_close(GtkWidget *widget, gpointer data);

void initialize_about_dialog() {
	GdkPixbuf *soap;
	const gchar *authors[] = {
		"David Trowbridge",
		"Dan Kuester",
		NULL
	};
	const gchar *documentors[] = {
		NULL
	};

	soap = gdk_pixbuf_new_from_file("data/xchat-gnome-small.png", NULL);
	gui.about = GNOME_ABOUT(gnome_about_new(
		"X-Chat GNOME",
		"x.x",
		"Copyright (c) 2004\nDavid Trowbridge and Daniel Kuester",
		"It has been well observed that a trombone\nis not a suitable instrument for a gentleman",
		authors,
		documentors,
		"",
		soap));
}

void show_about_dialog() {
	initialize_about_dialog();
	gtk_widget_show_all(GTK_WIDGET(gui.about));
}

void on_about_close(GtkWidget *widget, gpointer data) {
	gtk_widget_hide_all(GTK_WIDGET(gui.about));
}
