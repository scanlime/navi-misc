/*
 * about.c - utilities for displaying and hiding the about dialog
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

#include "about.h"
#include <config.h>

void on_about_close (GtkWidget *widget, gpointer data);

void
initialize_about_dialog ()
{
	GdkPixbuf *logo;
	const gchar *authors[] =
	{
		"Andre Dahlqvist",
		"Bastien Nocera",
		"Claessens Xavier",
		"Dan Kuester",
		"David Trowbridge",
		"Evan Sheehan",
		"Guillaume Desmottes",
		"Luis Villa",
		"Peter Železný",
		"Ramon Rey Vicente",
		"Rouslan Solomakhin",
		NULL
	};

	if (gui.about != NULL)
		return;

	logo = gdk_pixbuf_new_from_file ("data/xchat-gnome-small.png", NULL);
	if (!logo)
		logo = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome-small.png", NULL);

	gui.about = gtk_about_dialog_new ();
	gtk_about_dialog_set_name      (GTK_ABOUT_DIALOG (gui.about), "X-Chat GNOME");
	gtk_about_dialog_set_version   (GTK_ABOUT_DIALOG (gui.about), VERSION);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (gui.about), _("Copyright © 2004-2005"));
	gtk_about_dialog_set_comments  (GTK_ABOUT_DIALOG (gui.about), _("It has been well observed that a trombone is not a suitable instrument for a gentleman"));
	gtk_about_dialog_set_website   (GTK_ABOUT_DIALOG (gui.about), "http://xchat-gnome.navi.cx");
	gtk_about_dialog_set_authors   (GTK_ABOUT_DIALOG (gui.about), authors);
	gtk_about_dialog_set_logo      (GTK_ABOUT_DIALOG (gui.about), logo);
}

void
show_about_dialog ()
{
	initialize_about_dialog ();
	gtk_widget_show_all (GTK_WIDGET (gui.about));
}