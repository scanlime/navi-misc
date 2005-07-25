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

#include "../common/xchat.h"
#include "../common/xchatc.h"
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

	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (page->download_dir_button), prefs.dccdir);
	if (strlen (prefs.dcc_completed_dir) == 0)
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (page->completed_dir_button), prefs.dccdir);
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (page->completed_dir_button), prefs.dcc_completed_dir);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->convert_spaces), prefs.dcc_send_fillspaces);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->save_nicknames_dcc), prefs.dccwithnick);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->autoaccept_dcc_chat), prefs.autodccchat);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->autoaccept_dcc_file), prefs.autodccsend);

	if (prefs.ip_from_server) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->get_dcc_ip_from_server), TRUE);
		gtk_widget_set_sensitive (page->special_ip_address, FALSE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->use_specified_dcc_ip), TRUE);
		gtk_entry_set_text (GTK_ENTRY (page->special_ip_address), prefs.dcc_ip_str);
		gtk_widget_set_sensitive (page->special_ip_address, FALSE);
	}

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (page->individual_send_throttle), (gdouble) prefs.dcc_max_send_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (page->global_send_throttle), (gdouble) prefs.dcc_global_max_send_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (page->individual_receive_throttle), (gdouble) prefs.dcc_max_get_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (page->global_receive_throttle), (gdouble) prefs.dcc_global_max_get_cps);

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
