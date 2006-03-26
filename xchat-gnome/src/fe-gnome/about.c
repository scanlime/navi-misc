/*
 * about.c - utilities for displaying and hiding the about dialog
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
#include "about.h"
#include "../common/fe.h"

static void about_email_hook        (GtkAboutDialog *dialog, const gchar *link, gpointer data);
static void about_url_hook          (GtkAboutDialog *dialog, const gchar *link, gpointer data);

void
show_about_dialog (void)
{
	GdkPixbuf *logo;
	char *license_trans;

	const gchar *authors[] =
	{
		"Xavier Claessens <xclaesse@gmail.com>",
		"Andre Dahlqvist",
		"Guillaume Desmottes <cass@skynet.be>",
		"Dan Kuester <Daniel.Kuester@colorado.edu>",
		"Bastien Nocera",
		"Brian Pepple",
		"Isak Savo",
		"Evan Sheehan <Wallace.Sheehan@gmail.com>",
		"Rouslan Solomakhin",
		"David Trowbridge <trowbrds@gmail.com>",
		"Ramón Rey Vicente",
		"Luis Villa",
		"Peter Železný",
		NULL
	};
	const gchar *documenters[] =
	{
		"Brian Pepple",
		NULL
	};
	const gchar *artists[] =
	{
		"Micah Dowty",
		NULL
	};

	const char *license[] = {
		N_("X-Chat GNOME is free software; you can redistribute it and/or modify "
		   "it under the terms of the GNU General Public License as published by "
		   "the Free Software Foundation; either version 2 of the License, or "
		   "(at your option) any later version."),
		N_("X-Chat GNOME is distributed in the hope that it will be useful, "
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of "
		   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		   "GNU General Public License for more details."),
		N_("You should have received a copy of the GNU General Public License "
		   "along with X-Chat GNOME; if not, write to the Free Software Foundation, Inc., "
		   "59 Temple Place, Suite 330, Boston, MA  02111-1307  USA")
	};

	license_trans = g_strjoin ("\n\n", _(license[0]), _(license[1]), _(license[2]), NULL);

	if (g_file_test ("../../data/xchat-gnome-small.png", G_FILE_TEST_EXISTS))
		logo = gdk_pixbuf_new_from_file ("../../data/xchat-gnome-small.png", NULL);
	else
		logo = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome-small.png", NULL);

	gtk_about_dialog_set_url_hook   (about_url_hook,   NULL, NULL);
	gtk_about_dialog_set_email_hook (about_email_hook, NULL, NULL);

	gtk_show_about_dialog (GTK_WINDOW (gui.main_window),
	                       "name",        "XChat-GNOME",
	                       "version",     VERSION,
	                       "copyright",   _("Copyright © 2004-2006"),
	                       "comments",    _("It has been well observed that a trombone is not a suitable instrument for a gentleman"),
	                       "license",     license_trans,
	                       "wrap-license", TRUE,
	                       "website",     "http://xchat-gnome.navi.cx",
	                       "website-label", _("XChat-GNOME Web Site"),
	                       "authors",     authors,
	                       "artists",     artists,
	                       "logo",        logo,
	                       "documenters", documenters,
	                       /* Translators: This is a special message that shouldn't
	                        * be translated literally. It is used in the about box
	                        * to give credits to the translators. Thus, you should
	                        * translate it to your name and email address. You
	                        * should also include other translators who have
	                        * contributed to this translation; in that case, please
	                        * write each of them on a separated line seperated by
	                        * newlines (\n).
	                        */
	                       "translator-credits", _("translator-credits"),
	                       NULL);

	if (logo)
		g_object_unref (logo);

	g_free (license_trans);
}

static void
about_email_hook (GtkAboutDialog *dialog, const gchar *link, gpointer data)
{
}

static void
about_url_hook (GtkAboutDialog *dialog, const gchar *link, gpointer data)
{
	fe_open_url (link);
}
