/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
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

#include "notification-area-icon.h"
#include "eggtrayicon.h"
#include <gtk/gtkimage.h>
#include <libgnomeui/gnome-stock-icons.h>

static EggTrayIcon *icon = NULL;
static GtkWidget *image;

gboolean
ni_init ()
{
	icon = egg_tray_icon_new ("evolution mail status");
	image = gtk_image_new ();

	gtk_container_add (GTK_CONTAINER (icon), image);
	gtk_widget_show_all (GTK_WIDGET (icon));

	return (icon != NULL);
}

void
ni_set_status (NIStatus status)
{
	switch (status) {
	case NI_STATUS_NORMAL:
		gtk_image_set_from_stock (image, GNOME_STOCK_MAIL, GTK_ICON_SIZE_SMALL_TOOLBAR);
		break;
	case NI_STATUS_NEW_MAIL:
		gtk_image_set_from_stock (image, GNOME_STOCK_MAIL_NEW, GTK_ICON_SIZE_SMALL_TOOLBAR);
		break;
	}
}
