/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * gui-util.c - Small generic GUI utilities used by other modules
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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
#include "gui-util.h"
#include "prefix.h"

/* Create a list of all icons we have, in several different sizes,
 * and let GDK pick the best one(s) to use for this window.
 */
void fyre_set_icon(GtkWidget* window)
{
    static GList *icons = NULL;
    GdkPixbuf *pixbuf;

#define LOAD_ICON(name) \
    pixbuf = gdk_pixbuf_new_from_file(name, NULL); \
    if (pixbuf) \
	icons = g_list_append(icons, pixbuf);

    /* Only load the icons once */
    if (!icons) {

	LOAD_ICON(FYRE_DATADIR "/fyre-48x48.png");
	LOAD_ICON(FYRE_DATADIR "/fyre-32x32.png");
	LOAD_ICON(FYRE_DATADIR "/fyre-16x16.png");
    }

    if (!icons) {
	LOAD_ICON(BR_DATADIR( "/fyre-48x48.png" ));
	LOAD_ICON(BR_DATADIR( "/fyre-32x32.png" ));
	LOAD_ICON(BR_DATADIR( "/fyre-16x16.png" ));
    }
#undef LOAD_ICON

    gdk_window_set_icon_list(window->window, icons);
}


/* Set the icon once the window has been mapped */
void fyre_set_icon_later(GtkWidget* window)
{
    g_signal_connect(G_OBJECT(window), "map", G_CALLBACK(fyre_set_icon), NULL);
}

/* The End */
