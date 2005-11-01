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

	return page;
}

void
preferences_page_effects_free (PreferencesEffectsPage *page)
{
	gdk_pixbuf_unref (page->icon);
	g_free (page);
}
