/*
 * setup_dialog.c - Initial setup dialog for first-time users
 *
 * Copyright (C) 2004 xchat-gnome team
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

#include <gconf/gconf-client.h>
#include <glade/glade-xml.h>
#include <gtk/gtk.h>
#include "setup_dialog.h"

static GladeXML *xml;

void run_setup_dialog ()
{
	GtkSizeGroup *group;
	GtkWidget *window;
	GtkWidget *nick_entry;
	GtkWidget *real_entry;

	xml = glade_xml_new ("setup-dialog.glade", NULL, NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/setup-dialog.glade", NULL, NULL);
	if (!xml)
		/* FIXME - if we can't do this, we've got major problems */
		return;

	window = glade_xml_get_widget (xml, "setup window");

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	nick_entry = glade_xml_get_widget (xml, "nick name entry");
	gtk_size_group_add_widget (group, nick_entry);
	real_entry = glade_xml_get_widget (xml, "real name entry");
	gtk_size_group_add_widget (group, real_entry);
	g_object_unref (group);

}
