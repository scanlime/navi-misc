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

GdkPixbuf *pix_purple;
GdkPixbuf *pix_red;
GdkPixbuf *pix_op;
GdkPixbuf *pix_hop;
GdkPixbuf *pix_voice;

GdkPixbuf *pix_newdata;
GdkPixbuf *pix_nicksaid;
GdkPixbuf *pix_msgsaid;

void
pixmaps_init (void)
{
	if (g_file_test ("../../data/purple.png", G_FILE_TEST_EXISTS)) {
		pix_purple   = gdk_pixbuf_new_from_file ("../../data/purple.png",         NULL);
		pix_red      = gdk_pixbuf_new_from_file ("../../data/red.png",            NULL);
		pix_op       = gdk_pixbuf_new_from_file ("../../data/op.png",             NULL);
		pix_hop      = gdk_pixbuf_new_from_file ("../../data/hop.png",            NULL);
		pix_voice    = gdk_pixbuf_new_from_file ("../../data/voice.png",          NULL);

		pix_newdata  = gdk_pixbuf_new_from_file ("../../data/newdata.png",        NULL);
		pix_nicksaid = gdk_pixbuf_new_from_file ("../../data/nicksaid.png",       NULL);
		pix_msgsaid  = gdk_pixbuf_new_from_file ("../../data/global-message.png", NULL);
	} else {
		pix_purple   = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/purple.png",         NULL);
		pix_red      = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/red.png",            NULL);
		pix_op       = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/op.png",             NULL);
		pix_hop      = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/hop.png",            NULL);
		pix_voice    = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/voice.png",          NULL);

		pix_newdata  = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/newdata.png",        NULL);
		pix_nicksaid = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/nicksaid.png",       NULL);
		pix_msgsaid  = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/global-message.png", NULL);
	}
}
