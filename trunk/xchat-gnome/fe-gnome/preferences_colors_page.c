/*
 * preferences_colors_page.c - helpers for the colors preferences page
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
#include "preferences_colors_page.h"
#include "palette.h"
#include "gui.h"
#include "xtext.h"

static void gconf_color_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry);
static void gconf_palette_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry);

static void
set_color_buttons (int selection, GtkWidget **color_buttons)
/* Change the color buttons to reflect the selected color scheme. */
{
	load_colors (selection);
	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_set_background (gui.xtext, NULL, FALSE, FALSE);
	gtk_xtext_refresh (gui.xtext, FALSE);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[0]), &colors[18]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[1]), &colors[19]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[2]), &colors[17]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[3]), &colors[16]);
}

static void
set_palette_buttons (int selection, GtkWidget **palette_buttons)
/* Change the palette color buttons to reflect the selected palette scheme. */
{
	int i;

	load_palette (selection);
	for (i = 0; i < 16; i++)
		gtk_color_button_set_color (GTK_COLOR_BUTTON (palette_buttons[i]), &colors[i]);
	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_refresh (gui.xtext, FALSE);
}

static void
colors_changed (GtkComboBox *combo_box, gpointer data)
/* Color scheme has been changed. */
{
	GtkWidget **color_buttons = (GtkWidget **) data;
	int i, selection;
	GConfClient *client;

	client = gconf_client_get_default ();

	selection = gtk_combo_box_get_active (combo_box);
	if (selection == 2)
		for (i = 0; i < 4; i++)
			gtk_widget_set_sensitive (color_buttons[i], TRUE);
	else
		for (i = 0; i < 4; i++)
			gtk_widget_set_sensitive (color_buttons[i], FALSE);
	gconf_client_set_int (client, "/apps/xchat/irc/color_scheme", selection, NULL);
	set_color_buttons (selection, color_buttons);
}

static void
palette_changed (GtkComboBox *combo_box, gpointer data)
/* Palette scheme has been changed. */
{
	GtkWidget **palette_buttons = (GtkWidget **) data;
	int i, selection;
	GConfClient *client;

	client = gconf_client_get_default ();

	selection = gtk_combo_box_get_active (combo_box);
	if (selection == 1)
		for (i = 0; i < 16; i++)
			gtk_widget_set_sensitive (palette_buttons[i], TRUE);
	else
		for (i = 0; i < 16; i++)
			gtk_widget_set_sensitive (palette_buttons[i], FALSE);
	gconf_client_set_int (client, "/apps/xchat/irc/palette_scheme", selection, NULL);
	set_palette_buttons (selection, palette_buttons);
}

void
initialize_preferences_colors_page ()
{
	GConfClient *client;
	GtkWidget *table, *widget;
	GtkWidget *hbox, *color_schemes, *palette_schemes;
	GtkSizeGroup *group;
	static GtkWidget *palette_buttons[16];
	static GtkWidget *color_buttons[4];
	int i, j;
	gint scheme;

	client = gconf_client_get_default ();

	/* Set up the palette. */
	palette_init();

	table = glade_xml_get_widget (gui.xml, "palette table");
	/* Initialize the palette color buttons. 2 rows of buttons and 8 columns. */
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 2; j++)
		{
			/* Create the palette color button. */
			palette_buttons[j * 8 + i] = gtk_color_button_new ();
			gtk_widget_set_sensitive (palette_buttons[j * 8 + i], FALSE);
			gtk_widget_show (palette_buttons[j * 8 + i]);
			gtk_color_button_set_color (GTK_COLOR_BUTTON (palette_buttons[j * 8 + i]), &colors[j * 8 + i]);
			/* Place the button in the table. */
			gtk_table_attach_defaults (GTK_TABLE (table), palette_buttons[j * 8 + i], i, i+1, j, j+1);
		}
	}

	/* Set up the color scheme. */
	color_buttons[0] = gtk_color_button_new ();
	color_buttons[1] = gtk_color_button_new ();
	color_buttons[2] = gtk_color_button_new ();
	color_buttons[3] = gtk_color_button_new ();
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[0]), &colors[18]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[1]), &colors[19]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[2]), &colors[17]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[3]), &colors[16]);

	/* Color buttons are insensitive unless the custom color scheme is selected.
	 * By default the custom scheme is not selected.
	 */
	gtk_widget_set_sensitive (color_buttons[0], FALSE);
	gtk_widget_set_sensitive (color_buttons[1], FALSE);
	gtk_widget_set_sensitive (color_buttons[2], FALSE);
	gtk_widget_set_sensitive (color_buttons[3], FALSE);

	hbox = glade_xml_get_widget (gui.xml, "text color hbox");
	gtk_box_pack_start (GTK_BOX (hbox), color_buttons[0], FALSE, TRUE, 0);
	hbox = glade_xml_get_widget (gui.xml, "background color hbox");
	gtk_box_pack_start (GTK_BOX(hbox), color_buttons[1], FALSE, TRUE, 0);
	hbox = glade_xml_get_widget (gui.xml, "foreground mark hbox");
	gtk_box_pack_start (GTK_BOX (hbox), color_buttons[2], FALSE, TRUE, 0);
	hbox = glade_xml_get_widget (gui.xml, "background mark hbox");
	gtk_box_pack_start (GTK_BOX (hbox), color_buttons[3], FALSE, TRUE, 0);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget (gui.xml, "color label 1");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 2");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 3");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 4");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 5");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 6");
	gtk_size_group_add_widget (group, widget);
	widget = glade_xml_get_widget (gui.xml, "color label 7");
	gtk_size_group_add_widget (group, widget);
	g_object_unref (group);

	/* Combo boxes for selecting color and palette schemes. */
	color_schemes = gtk_combo_box_new_text ();
	gtk_combo_box_append_text (GTK_COMBO_BOX (color_schemes), "Black on White");
	gtk_combo_box_append_text (GTK_COMBO_BOX (color_schemes), "White on Black");
	gtk_combo_box_append_text (GTK_COMBO_BOX (color_schemes), "Custom");
	g_signal_connect (G_OBJECT (color_schemes), "changed", G_CALLBACK (colors_changed), (gpointer) color_buttons);
	scheme = gconf_client_get_int (client, "/apps/xchat/irc/color_scheme", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/color_scheme", (GConfClientNotifyFunc) gconf_color_changed, NULL, NULL, NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX(color_schemes), scheme);
	palette_schemes = gtk_combo_box_new_text ();
	gtk_combo_box_append_text (GTK_COMBO_BOX (palette_schemes), "X-Chat Default");
	gtk_combo_box_append_text (GTK_COMBO_BOX (palette_schemes), "Custom");
	g_signal_connect (G_OBJECT (palette_schemes), "changed", G_CALLBACK (palette_changed), (gpointer) palette_buttons);
	scheme = gconf_client_get_int (client, "/apps/xchat/irc/palette_scheme", NULL);
	gconf_client_notify_add (client, "/apps/xchat/irc/palette_scheme", (GConfClientNotifyFunc) gconf_palette_changed, NULL, NULL, NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX (palette_schemes), scheme);

	hbox = glade_xml_get_widget (gui.xml, "foreground background hbox");
	gtk_box_pack_start (GTK_BOX (hbox), color_schemes, FALSE, TRUE, 0);
	hbox = glade_xml_get_widget (gui.xml, "palette hbox");
	gtk_box_pack_start (GTK_BOX (hbox), palette_schemes, FALSE, TRUE, 0);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, color_schemes);
	widget = glade_xml_get_widget (gui.xml, "palette schemes");
	gtk_size_group_add_widget (group, palette_schemes);
	g_object_unref (group);
}

static void
gconf_color_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry)
{
}

static void
gconf_palette_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, GtkEntry *entry)
{
}
