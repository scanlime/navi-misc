/*
 * preferences_colors_page.c - helpers for the colors preferences page
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

#include "preferences_colors_page.h"
#include "palette.h"

void initialize_preferences_colors_page() {
	GtkWidget *table, *button, *widget;
	GtkWidget *hbox;
	GtkSizeGroup *group;
	int i, j;

	table = glade_xml_get_widget(gui.xml, "palette table");
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 2; j++) {
			button = gtk_color_button_new();
			gtk_widget_show(button);
			gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colors[j * 8 + i]);
			gtk_table_attach_defaults(GTK_TABLE(table), button, i, i+1, j, j+1);
		}
	}
	hbox = glade_xml_get_widget(gui.xml, "text color hbox");
	button = gtk_color_button_new();
	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colors[19]);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "background color hbox");
	button = gtk_color_button_new();
	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colors[20]);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "foreground mark hbox");
	button = gtk_color_button_new();
	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colors[17]);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "background mark hbox");
	button = gtk_color_button_new();
	gtk_color_button_set_color(GTK_COLOR_BUTTON(button), &colors[16]);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "color label 1");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 2");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 3");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 4");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 5");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 6");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "color label 7");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "foreground background schemes");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "palette schemes");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}
