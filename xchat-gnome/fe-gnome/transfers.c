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

static gboolean transfers_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
	preferences_set_transfers_window_size(event->width, event->height);
	return FALSE;
}

void initialize_transfers_window() {
	GtkWidget *expander, *box, *widget, *details, *treeview;
	GtkSizeGroup *group;
	GtkCellRenderer *percent_r, *filename_r, *size_r, *eta_r;
	GtkTreeViewColumn *percent_c, *filename_c, *size_c, *eta_c;
	GtkTreeSelection *select;
	int width, height;

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

	transfer_gui.store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
	treeview = glade_xml_get_widget(transfer_gui.xml, "treeview1");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(transfer_gui.store));
	gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(treeview), TRUE);

	percent_r = gtk_cell_renderer_text_new();
	percent_c = gtk_tree_view_column_new_with_attributes("%", percent_r, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(percent_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(percent_c, 5);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), percent_c);
	filename_r = gtk_cell_renderer_text_new();
	filename_c = gtk_tree_view_column_new_with_attributes("Filename", filename_r, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(filename_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(filename_c, 1);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), filename_c);
	size_r = gtk_cell_renderer_text_new();
	size_c = gtk_tree_view_column_new_with_attributes("Size", size_r, "text", 2, NULL);
	gtk_tree_view_column_set_resizable(size_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(size_c, 2);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), size_c);
	eta_r = gtk_cell_renderer_text_new();
	eta_c = gtk_tree_view_column_new_with_attributes("Remaining", eta_r, "text", 3, NULL);
	gtk_tree_view_column_set_resizable(eta_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(eta_c, 1);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), eta_c);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	widget = glade_xml_get_widget(transfer_gui.xml, "window1");
	details = glade_xml_get_widget(transfer_gui.xml, "details");
	preferences_get_transfers_window_size(&width, &height);
	gtk_widget_show_all(widget);
	gtk_widget_hide_all(details);
	if(!(width == 0 || height == 0))
		gtk_window_resize(GTK_WINDOW(widget), width, height);
	g_signal_connect(G_OBJECT(widget), "configure-event", G_CALLBACK(transfers_resize), NULL);
}

void show_transfers_window() {
	GtkWidget *window;

	window = glade_xml_get_widget(transfer_gui.xml, "window1");
	gtk_widget_show_all(window);
}

void hide_transfers_window() {
	GtkWidget *window;

	window = glade_xml_get_widget(transfer_gui.xml, "window1");
	gtk_widget_hide_all(window);
}
