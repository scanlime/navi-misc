/*
 * preferences-page-dcc.c - helpers for the DCC preferences page
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

#include "preferences-page-dcc.h"
#include "preferences-dialog.h"

PreferencesDCCPage *
preferences_page_dcc_new (gpointer prefs_dialog, GladeXML *xml)
{
	GtkTreeIter iter;
	PreferencesDCCPage *page = g_new0 (PreferencesDCCPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;

	page->icon = gdk_pixbuf_new_from_file (XCHATSHAREDIR "/dcc.png", NULL);
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, "File Transfers & DCC", 2, 2, -1);

	return page;
}

void
preferences_page_dcc_free (PreferencesDCCPage *page)
{
	gdk_pixbuf_unref (page->icon);
	g_free (page);
}
