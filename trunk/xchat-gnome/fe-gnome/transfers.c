/*
 * transfers.h - dcc transfers window
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

#include <gnome.h>
#include "transfers.h"
#include "preferences.h"

TransferGui transfer_gui;

static void expanded(GtkExpander *exp, GParamSpec *param_spec, gpointer data) {
	GtkWidget *details = glade_xml_get_widget(transfer_gui.xml, "details");

	if(gtk_expander_get_expanded(exp)) {
		gtk_widget_show_all(details);
	} else {
		gtk_widget_hide_all(details);
	}
}

void initialize_transfers_window() {
	GtkWidget *expander, *box, *widget;
	GtkSizeGroup *group;

	transfer_gui.xml = glade_xml_new("transfers.glade", NULL, NULL);
	if(!transfer_gui.xml)
		transfer_gui.xml = glade_xml_new(XCHATSHAREDIR"/transfers.glade", NULL, NULL);
	if(!transfer_gui.xml)
		return;

	expander = gtk_expander_new("Download Details");
	gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
	g_signal_connect(G_OBJECT(expander), "notify::expanded", G_CALLBACK(expanded), NULL);
	box = glade_xml_get_widget(transfer_gui.xml, "expander box");

	gtk_box_pack_start(GTK_BOX(box), expander, FALSE, TRUE, 0);
	gtk_box_reorder_child(GTK_BOX(box), expander, 0);
	gtk_widget_show(expander);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(transfer_gui.xml, "file label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "from label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "status label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "elapsed label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(transfer_gui.xml, "remaining label");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}
