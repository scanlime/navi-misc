/*
 * preferences-page-effects.c - helpers for the effects preferences page
 *
 * Copyright (C) 2005 David Trowbridge
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

#include "preferences-page-effects.h"
#include "preferences-dialog.h"

static void
type_changed (GtkRadioButton *button, PreferencesEffectsPage *page)
{
	gint type;

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->background_none)))
		type = 0;
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->background_image)))
		type = 1;
	else
		type = 2;
}

static void
image_changed (GtkFileChooser *chooser, PreferencesEffectsPage *page)
{
}

static void
transparency_changed (GtkRange *range, PreferencesEffectsPage *page)
{
}

static void
gconf_type_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, PreferencesEffectsPage *page)
{
	gint type;

	g_signal_handlers_block_by_func (G_OBJECT (page->background_none), "group-changed", type_changed);
	type = gconf_client_get_int (client, entry->key, NULL);
	if (type == 0)
		gtk_toggle_button_set_active (page->background_none, TRUE);
	else if (type == 1)
		gtk_toggle_button_set_active (page->background_image, TRUE);
	else
		gtk_toggle_button_set_active (page->background_transparent, TRUE);
	g_signal_handlers_unblock_by_func (G_OBJECT (page->background_none), "group-changed", type_changed);
}

static void
gconf_image_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, PreferencesEffectsPage *page)
{
}

static void
gconf_transparency_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, PreferencesEffectsPage *page)
{
}

PreferencesEffectsPage *
preferences_page_effects_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesEffectsPage *page = g_new0 (PreferencesEffectsPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(background_none);
	GW(background_image);
	GW(background_transparent);
	GW(background_image_file);
	GW(background_transparency);
#undef GW

	page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/effects.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, "Effects", 2, 2, -1);

	g_signal_connect (G_OBJECT (page->background_none),         "group-changed",     G_CALLBACK (type_changed),         page);
	g_signal_connect (G_OBJECT (page->background_image_file),   "selection-changed", G_CALLBACK (image_changed),        page);
	g_signal_connect (G_OBJECT (page->background_transparency), "value-changed",     G_CALLBACK (transparency_changed), page);

	page->nh[0] = gconf_client_notify_add (p->gconf, "/apps/xchat/main_window/background_type",         (GConfClientNotifyFunc) gconf_type_changed,         page, NULL, NULL);
	page->nh[1] = gconf_client_notify_add (p->gconf, "/apps/xchat/main_window/background_image",        (GConfClientNotifyFunc) gconf_image_changed,        page, NULL, NULL);
	page->nh[2] = gconf_client_notify_add (p->gconf, "/apps/xchat/main_window/background_transparency", (GConfClientNotifyFunc) gconf_transparency_changed, page, NULL, NULL);

	return page;
}

void
preferences_page_effects_free (PreferencesEffectsPage *page)
{
	gdk_pixbuf_unref (page->icon);
	g_free (page);
}
