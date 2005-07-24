/*
 * preferences-page-dcc.c - helpers for the DCC preferences page
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

#include "preferences-page-dcc.h"
#include "preferences-dialog.h"

PreferencesDCCPage *
preferences_page_dcc_new (gpointer prefs_dialog, GladeXML *xml)
{
	GtkTreeIter iter;
	PreferencesDCCPage *page = g_new0 (PreferencesDCCPage, 1);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkSizeGroup *group;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(download_dir_button);
	GW(completed_dir_button);
	GW(convert_spaces);
	GW(save_nicknames_dcc);
	GW(autoaccept_dcc_chat);
	GW(autoaccept_dcc_file);
	GW(get_dcc_ip_from_server);
	GW(use_specified_dcc_ip);
	GW(special_ip_address);
	GW(individual_send_throttle);
	GW(global_send_throttle);
	GW(individual_receive_throttle);
	GW(global_receive_throttle);
#undef GW

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, page->download_dir_button);
	gtk_size_group_add_widget (group, page->completed_dir_button);
	gtk_size_group_add_widget (group, page->special_ip_address);
	gtk_size_group_add_widget (group, page->individual_send_throttle);
	gtk_size_group_add_widget (group, page->global_send_throttle);
	gtk_size_group_add_widget (group, page->individual_receive_throttle);
	gtk_size_group_add_widget (group, page->global_receive_throttle);
	g_object_unref (group);

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
