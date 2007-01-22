/*
 * about.c - utilities for displaying and hiding the about dialog
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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

void
show_about_dialog (void)
{
	char *license_trans;

	const gchar *authors[] = {
		"Andre Dahlqvist <andre.bugs@gmail.com>",
		"Bastien Nocera <hadess@hadess.net>",
		"Brian Pepple <bdpepple@gmail.com>",
		"Christian Persch <chpe@gnome.org>",
		"Dan Kuester <Daniel.Kuester@colorado.edu>",
		"Daniel Drake <dsd@gentoo.org>",
		"David Trowbridge <trowbrds@gmail.com>",
		"Evan Sheehan <Wallace.Sheehan@gmail.com>",
		"Guillaume Desmottes <cass@skynet.be>",
		"Isak Savo <iso01001@student.mdh.se>",
		"Luis Villa <luis.villa@gmail.com>",
		"Peter Železný <zed@xchat.org>",
		"Ramón Rey Vicente <ramon.rey@hispalinux.es>",
		"Rouslan Solomakhin <solomarv@cosi.clarkson.edu>",
		"Xavier Claessens <xclaesse@gmail.com>",
		NULL
	};
	const gchar *documenters[] = {
		"Brian Pepple <bdpepple@gmail.com>",
		NULL
	};
	const gchar *artists[] = {
		"Andreas Nilsson <nisses.mail@home.se>",
		"Jones Lee <joneslee85@gmail.com>",
		"Lapo Calamandrei <lapo@it.gnome.org>",
		"Micah Dowty <micah@navi.cx>",
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

	gtk_show_about_dialog (GTK_WINDOW (gui.main_window),
	                       "name", "XChat-GNOME",
	                       "version", PACKAGE_VERSION,
	                       "copyright", _("Copyright © 2004-2007"),
	                       /* Translators: This is a random, mostly-meaningless
	                        * quotation from Jeeves & Wooster.  Either leave it
	                        * be or replace it with something funny (but inoffensive)
	                        * in your own language, but don't translate it literally.
	                        */
	                       "comments", _("It has been well observed that a trombone is not a suitable instrument for a gentleman"),
	                       "license", license_trans,
	                       "wrap-license", TRUE,
	                       "website", "http://xchat-gnome.navi.cx",
	                       "website-label", _("XChat-GNOME Web Site"),
	                       "authors", authors,
	                       "artists", artists,
	                       "logo-icon-name", "xchat-gnome",
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

	g_free (license_trans);
}
