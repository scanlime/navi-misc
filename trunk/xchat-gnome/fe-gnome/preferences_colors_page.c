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

void initialize_preferences_colors_page() {
	GtkWidget *table, *button;
	GtkWidget *hbox;
	int i, j;

	table = glade_xml_get_widget(gui.xml, "palette table");
	for(i = 0; i < 8; i++) {
		for(j = 0; j < 2; j++) {
			button = gtk_color_button_new();
			gtk_widget_show(button);
			gtk_table_attach_defaults(GTK_TABLE(table), button, i, i+1, j, j+1);
		}
	}
	hbox = glade_xml_get_widget(gui.xml, "text color hbox");
	button = gtk_color_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "background color hbox");
	button = gtk_color_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "foreground mark hbox");
	button = gtk_color_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget(gui.xml, "background mark hbox");
	button = gtk_color_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 0);
}
