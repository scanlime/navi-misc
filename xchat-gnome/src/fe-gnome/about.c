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

static void initialize_about_dialog (void);
static void about_email_hook        (GtkAboutDialog *dialog, const gchar *link, gpointer data);
static void about_url_hook          (GtkAboutDialog *dialog, const gchar *link, gpointer data);

static void
initialize_about_dialog (void)
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
		N_("X-Chat GNOME is free software; you can redistribute it and/or modify\n"
		   "it under the terms of the GNU General Public License as published by\n"
		   "the Free Software Foundation; either version 2 of the License, or\n"
		   "(at your option) any later version.\n"),
		N_("X-Chat GNOME is distributed in the hope that it will be useful,\n"
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		   "GNU General Public License for more details.\n"),
		N_("You should have received a copy of the GNU General Public License\n"
		   "along with X-Chat GNOME; if not, write to the Free Software Foundation, Inc.,\n"
		   "59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n")
	};

	license_trans = g_strconcat (_(license[0]), "\n", _(license[1]), "\n", _(license[2]), "\n", NULL);

	if (g_file_test ("../../data/xchat-gnome-small.png", G_FILE_TEST_EXISTS))
		logo = gdk_pixbuf_new_from_file ("../../data/xchat-gnome-small.png", NULL);
	else
		logo = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/xchat-gnome-small.png", NULL);

	gui.about = gtk_about_dialog_new ();

	gtk_about_dialog_set_url_hook   (about_url_hook,   NULL, NULL);
	gtk_about_dialog_set_email_hook (about_email_hook, NULL, NULL);

	g_object_set (G_OBJECT (gui.about),
		      "name",        "XChat-GNOME",
		      "version",     VERSION,
		      "copyright",   _("Copyright © 2004-2006"),
		      "comments",    _("It has been well observed that a trombone is not a suitable instrument for a gentleman"),
		      "license",     license_trans,
		      "website",     "http://xchat-gnome.navi.cx",
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

	gtk_window_set_transient_for (GTK_WINDOW (gui.about), GTK_WINDOW (gui.main_window));
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

void
show_about_dialog (void)
{
	if (gui.about == NULL)
		initialize_about_dialog ();
	gtk_widget_show_all (GTK_WIDGET (gui.about));
}
