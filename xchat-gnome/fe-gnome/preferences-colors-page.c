/*
 * preferences-colors-page.c - helpers for the colors preferences page
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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
#include "preferences-colors-page.h"
#include "preferences-dialog.h"
#include "palette.h"
#include "gui.h"
#include "xtext.h"

static void
color_button_changed (GtkColorButton *button, gpointer data)
{
	int index = GPOINTER_TO_INT (data);
	GdkColor c;

	gtk_color_button_get_color (button, &c);
	if (index < 32) {
		custom_palette[index].red = c.red;
		custom_palette[index].green = c.green;
		custom_palette[index].blue = c.blue;
	} else {
		custom_colors[index - 32].red = c.red;
		custom_colors[index - 32].green = c.green;
		custom_colors[index - 32].blue = c.blue;
	}
	palette_save ();
}

static void
set_color_buttons (int selection, GtkWidget **color_buttons)
{
	load_colors (selection);
	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_set_background (gui.xtext, NULL, FALSE, FALSE);
	gtk_xtext_refresh (gui.xtext, FALSE);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[0]), &colors[34]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[1]), &colors[35]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[2]), &colors[32]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[3]), &colors[33]);
}

static void
set_palette_buttons (int selection, GtkWidget **palette_buttons)
{
	int i;

	load_palette (selection);
	for (i = 0; i < 32; i++)
		gtk_color_button_set_color (GTK_COLOR_BUTTON (palette_buttons[i]), &colors[i]);
	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_refresh (gui.xtext, FALSE);
}

static void
colors_changed (GtkComboBox *combo_box, PreferencesColorsPage *page)
{
	int i, selection;
	GConfClient *client;

	client = gconf_client_get_default ();

	selection = gtk_combo_box_get_active (combo_box);
	if (selection == 2) {
		for (i = 0; i < 4; i++)
			gtk_widget_set_sensitive (page->color_buttons[i], TRUE);
		for (i = 0; i < 32; i++)
			gtk_widget_set_sensitive (page->palette_buttons[i], TRUE);
	} else {
		for (i = 0; i < 4; i++)
			gtk_widget_set_sensitive (page->color_buttons[i], FALSE);
		for (i = 0; i < 32; i++)
			gtk_widget_set_sensitive (page->palette_buttons[i], FALSE);
	}
	gconf_client_set_int (client, "/apps/xchat/irc/color_scheme", selection, NULL);
	set_color_buttons (selection, page->color_buttons);
	set_palette_buttons (selection, page->palette_buttons);

	g_object_unref (client);
}

static void
gconf_color_changed (GConfClient *client, guint cnxn_id, const gchar *key, GConfValue *value, gboolean is_default, PreferencesColorsPage *page)
{
	int selection;
	selection = gconf_client_get_int (client, key, NULL);

	gtk_combo_box_set_active (GTK_COMBO_BOX (page->combo), selection);
	set_color_buttons (selection, page->color_buttons);
	set_palette_buttons (selection, page->palette_buttons);
}

PreferencesColorsPage *
preferences_page_colors_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesColorsPage *page = g_new0 (PreferencesColorsPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkSizeGroup *group;
	GtkTreeIter iter;
	gint i, j, scheme;

	palette_init ();

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(color_label_1);
	GW(color_label_2);
	GW(color_label_3);
	GW(color_label_4);
	GW(color_label_5);

	GW(foreground_background_hbox);
	GW(text_color_hbox);
	GW(background_color_hbox);
	GW(foreground_mark_hbox);
	GW(background_mark_hbox);

	GW(mirc_palette_table);
	GW(extra_palette_table);
#undef GW

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, page->color_label_1);
	gtk_size_group_add_widget (group, page->color_label_2);
	gtk_size_group_add_widget (group, page->color_label_3);
	gtk_size_group_add_widget (group, page->color_label_4);
	gtk_size_group_add_widget (group, page->color_label_5);
	g_object_unref (group);

	for (i = 0; i < 4; i++) {
		page->color_buttons[i] = gtk_color_button_new ();
		gtk_widget_set_sensitive (page->color_buttons[i], FALSE);
	}
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 8; i++) {
			gint c = j * 8 + i;
			page->palette_buttons[c] = gtk_color_button_new ();
			gtk_widget_set_sensitive (page->palette_buttons[c], FALSE);
			gtk_table_attach_defaults (GTK_TABLE (page->mirc_palette_table), page->palette_buttons[c], i, i+1, j, j+1);
			gtk_color_button_set_color (GTK_COLOR_BUTTON (page->palette_buttons[c]), &colors[c]);
			g_signal_connect (G_OBJECT (page->palette_buttons[c]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (c));
		}
	}
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 8; i++) {
			gint c = j * 8 + i + 16;
			page->palette_buttons[c] = gtk_color_button_new ();
			gtk_widget_set_sensitive (page->palette_buttons[c], FALSE);
			gtk_table_attach_defaults (GTK_TABLE (page->extra_palette_table), page->palette_buttons[c], i, i+1, j, j+1);
			gtk_color_button_set_color (GTK_COLOR_BUTTON (page->palette_buttons[c]), &colors[c]);
			g_signal_connect (G_OBJECT (page->palette_buttons[c]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (c));
		}
	}

	gtk_box_pack_start (GTK_BOX (page->text_color_hbox),       page->color_buttons[0], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->background_color_hbox), page->color_buttons[1], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->foreground_mark_hbox),  page->color_buttons[2], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->background_mark_hbox),  page->color_buttons[3], FALSE, TRUE, 0);

	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_2), page->color_buttons[0]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_3), page->color_buttons[1]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_4), page->color_buttons[2]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_5), page->color_buttons[3]);

	gtk_color_button_set_color (GTK_COLOR_BUTTON (page->color_buttons[0]), &colors[34]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (page->color_buttons[1]), &colors[35]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (page->color_buttons[2]), &colors[33]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (page->color_buttons[3]), &colors[32]);

	g_signal_connect (G_OBJECT (page->color_buttons[0]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (32 + 0));
	g_signal_connect (G_OBJECT (page->color_buttons[1]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (32 + 1));
	g_signal_connect (G_OBJECT (page->color_buttons[2]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (32 + 2));
	g_signal_connect (G_OBJECT (page->color_buttons[3]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (32 + 3));

	page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/color.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, "Colors", 2, 1, -1);

	page->combo = gtk_combo_box_new_text ();
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), "Black on White");
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), "White on Black");
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), "Custom");
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_1), page->combo);
	gtk_box_pack_start (GTK_BOX (page->foreground_background_hbox), page->combo, FALSE, TRUE, 0);
	scheme = gconf_client_get_int (p->gconf, "/apps/xchat/irc/color_scheme", NULL);
	gtk_combo_box_set_active (GTK_COMBO_BOX (page->combo), scheme);

	gconf_client_notify_add (p->gconf, "/apps/xchat/irc/color_scheme", (GConfClientNotifyFunc) gconf_color_changed, page, NULL, NULL);

	g_signal_connect (G_OBJECT (page->combo), "changed", G_CALLBACK (colors_changed), page);

	return page;
}

void
preferences_page_colors_free (PreferencesColorsPage *page)
{
}
