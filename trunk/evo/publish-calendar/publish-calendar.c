/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc. (www.novell.com)
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

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <calendar/gui/e-cal-popup.h>
#include <calendar/gui/e-cal-config.h>

static GtkListStore *store = NULL;

enum {
	URL_LIST_ENABLED_COLUMN,
	URL_LIST_LOCATION_COLUMN,
	URL_LIST_URL_COLUMN,
	URL_LIST_N_COLUMNS,
};

void
publish_calendar_context_activate (EPlugin *ep, ECalPopupTargetSource *target)
{
	g_print ("publish!\n");
}

GtkWidget *
publish_calendar_locations (EPlugin *epl, EConfigHookItemFactoryData *data)
{
	GladeXML *xml;
	GtkWidget *widget;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;

	/* FIXME: create model */

	xml = glade_xml_new (PLUGINDIR "/publish-calendar.glade", "toplevel", NULL);

	widget = glade_xml_get_widget (xml, "url list");
	renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (G_OBJECT (renderer), "activatable", TRUE, NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (widget), -1, _("Enabled"),
			                             renderer, "active", URL_LIST_ENABLED_COLUMN, NULL);
	g_signal_connect (G_OBJECT (renderer), "toggled", G_CALLBACK (url_list_enable_toggled), xml);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (widget), -1, _("Location"),
			                             renderer, "text", URL_LIST_LOCATION_COLUMN, NULL);
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (selection_changed), xml);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (widget), TRUE);
	g_signal_connect (G_OBJECT (widget), "row-activated", G_CALLBACK (url_list_double_click), xml);

	widget = glade_xml_get_widget (xml, "toplevel");
	gtk_widget_show_all (widget);
	gtk_box_pack_start (GTK_BOX (data->parent), widget, FALSE, TRUE, 0);
}
