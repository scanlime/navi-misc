/*
 * preferences-page-colors.c - helpers for the colors preferences page
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
#include <gconf/gconf-client.h>
#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "preferences-page-colors.h"
#include "preferences-dialog.h"
#include "xtext.h"

static int scheme;

static void
color_button_changed (GtkColorButton *button, gpointer data)
{
	int index = GPOINTER_TO_INT (data);
	GdkColor c;

	if (scheme != 2)
		return;

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

	load_colors (2);
	load_palette (2);

	conversation_panel_update_colors (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
set_color_buttons (int selection, GtkWidget **color_buttons)
{
	load_colors (selection);

	g_signal_handlers_block_by_func (G_OBJECT (color_buttons[0]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (32));
	g_signal_handlers_block_by_func (G_OBJECT (color_buttons[1]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (33));
	g_signal_handlers_block_by_func (G_OBJECT (color_buttons[2]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (34));
	g_signal_handlers_block_by_func (G_OBJECT (color_buttons[3]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (35));

	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[0]), &colors[34]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[1]), &colors[35]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[2]), &colors[32]);
	gtk_color_button_set_color (GTK_COLOR_BUTTON (color_buttons[3]), &colors[33]);

	g_signal_handlers_unblock_by_func (G_OBJECT (color_buttons[0]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (32));
	g_signal_handlers_unblock_by_func (G_OBJECT (color_buttons[1]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (33));
	g_signal_handlers_unblock_by_func (G_OBJECT (color_buttons[2]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (34));
	g_signal_handlers_unblock_by_func (G_OBJECT (color_buttons[3]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (35));

	conversation_panel_update_colors (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
set_palette_buttons (int selection, GtkWidget **palette_buttons)
{
	int i;

	load_palette (selection);
	for (i = 0; i < 32; i++) {
		g_signal_handlers_block_by_func (G_OBJECT (palette_buttons[i]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (i));
		gtk_color_button_set_color (GTK_COLOR_BUTTON (palette_buttons[i]), &colors[i]);
		g_signal_handlers_unblock_by_func (G_OBJECT (palette_buttons[i]), G_CALLBACK (color_button_changed), GINT_TO_POINTER (i));
	}
	conversation_panel_update_colors (CONVERSATION_PANEL (gui.conversation_panel));
}

static void
colors_changed (GtkComboBox *combo_box, PreferencesColorsPage *page)
{
	int i, selection;
	GConfClient *client;

	client = gconf_client_get_default ();

	selection = gtk_combo_box_get_active (combo_box);
	scheme = selection;

	/* If we've set custom, sensitize the color buttons */
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
gconf_color_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, PreferencesColorsPage *page)
{
	int selection;
	selection = gconf_client_get_int (client, entry->key, NULL);
	scheme = selection;

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
	gint i, j;

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

	for (j = 0; j < 2; j++) {
		for (i = 0; i < 8; i++) {
			gint c = j * 8 + i;
			page->palette_buttons[c] = gtk_color_button_new ();
			gtk_widget_show (page->palette_buttons[c]);
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
			gtk_widget_show (page->palette_buttons[c]);
			gtk_widget_set_sensitive (page->palette_buttons[c], FALSE);
			gtk_table_attach_defaults (GTK_TABLE (page->extra_palette_table), page->palette_buttons[c], i, i+1, j, j+1);
			gtk_color_button_set_color (GTK_COLOR_BUTTON (page->palette_buttons[c]), &colors[c]);
			g_signal_connect (G_OBJECT (page->palette_buttons[c]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (c));
		}
	}

	for (i = 0; i < 4; i++) {
		page->color_buttons[i] = gtk_color_button_new ();
		gtk_widget_show (page->color_buttons[i]);
		gtk_widget_set_sensitive (page->color_buttons[i], FALSE);
	}
	gtk_box_pack_start (GTK_BOX (page->text_color_hbox),       page->color_buttons[0], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->background_color_hbox), page->color_buttons[1], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->foreground_mark_hbox),  page->color_buttons[2], FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page->background_mark_hbox),  page->color_buttons[3], FALSE, TRUE, 0);

	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_2), page->color_buttons[0]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_3), page->color_buttons[1]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_4), page->color_buttons[2]);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_5), page->color_buttons[3]);

	g_signal_connect (G_OBJECT (page->color_buttons[0]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (32));
	g_signal_connect (G_OBJECT (page->color_buttons[1]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (33));
	g_signal_connect (G_OBJECT (page->color_buttons[2]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (34));
	g_signal_connect (G_OBJECT (page->color_buttons[3]), "color-set", G_CALLBACK (color_button_changed), GINT_TO_POINTER (35));

	if (g_file_test ("../../data/color.png", G_FILE_TEST_EXISTS))
		page->icon = gdk_pixbuf_new_from_file ("../../data/color.png", NULL);
	else
		page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/color.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, _("Colors"), 2, 1, -1);

	page->combo = gtk_combo_box_new_text ();
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), _("Black on White"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), _("White on Black"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (page->combo), _("Custom"));
	gtk_widget_show (page->combo);
	gtk_label_set_mnemonic_widget (GTK_LABEL (page->color_label_1), page->combo);
	gtk_box_pack_start (GTK_BOX (page->foreground_background_hbox), page->combo, FALSE, TRUE, 0);
	scheme = gconf_client_get_int (p->gconf, "/apps/xchat/irc/color_scheme", NULL);

	page->notify = gconf_client_notify_add (p->gconf, "/apps/xchat/irc/color_scheme",
	                                        (GConfClientNotifyFunc) gconf_color_changed, page, NULL, NULL);

	g_signal_connect (G_OBJECT (page->combo), "changed", G_CALLBACK (colors_changed), page);
	gtk_combo_box_set_active (GTK_COMBO_BOX (page->combo), scheme);

	return page;
}

void
preferences_page_colors_free (PreferencesColorsPage *page)
{
	GConfClient *client;

	client = gconf_client_get_default ();
	gconf_client_notify_remove (client, page->notify);
	g_object_unref (client);
	g_object_unref (page->icon);
	g_free (page);
}
