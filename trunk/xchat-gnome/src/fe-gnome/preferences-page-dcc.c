/*
 * preferences-page-dcc.c - helpers for the DCC preferences page
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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
#include <string.h>
#include "preferences-page-dcc.h"
#include "preferences-dialog.h"
#include "util.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"

G_DEFINE_TYPE(PreferencesPageDCC, preferences_page_dcc, PREFERENCES_PAGE_TYPE)

static void
path_changed (GtkFileChooser *file_chooser, char *target)
{
	gchar *dir = gtk_file_chooser_get_filename (file_chooser);
	if (dir) {
		strncpy (target, dir, PATHLEN);
		target[PATHLEN] = '\0';
		g_free (dir);
	}
}

static void
toggle_changed (GtkToggleButton *button, unsigned int *target)
{
	*target = gtk_toggle_button_get_active (button);
}

static void
get_ip_from_server_changed (GtkRadioButton *button, PreferencesPageDCC *page)
{
	gboolean on = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
	gtk_widget_set_sensitive (page->special_ip_address, !on);

	prefs.ip_from_server = on;
}

static void
special_ip_changed (GtkEntry *entry, gpointer data)
{
	strncpy (prefs.dcc_ip_str, gtk_entry_get_text (entry), 15);
	prefs.dcc_ip_str[15] = '\0';
}

static void
spin_changed (GtkSpinButton *button, int *target)
{
	*target = gtk_spin_button_get_value_as_int (button);
}

PreferencesPageDCC *
preferences_page_dcc_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesPageDCC *page = g_object_new (PREFERENCES_PAGE_DCC_TYPE, NULL);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;

#define GW(name) GtkWidget *name = glade_xml_get_widget (xml, #name)
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

	page->special_ip_address = special_ip_address;

	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (download_dir_button), prefs.dccdir);
	if (strlen (prefs.dcc_completed_dir) == 0) {
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (completed_dir_button), prefs.dccdir);
	} else {
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (completed_dir_button), prefs.dcc_completed_dir);
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (convert_spaces), prefs.dcc_send_fillspaces);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (save_nicknames_dcc), prefs.dccwithnick);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autoaccept_dcc_chat), prefs.autodccchat);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autoaccept_dcc_file), prefs.autodccsend);

	if (prefs.ip_from_server) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_dcc_ip_from_server), TRUE);
		gtk_widget_set_sensitive (special_ip_address, FALSE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (use_specified_dcc_ip), TRUE);
		gtk_entry_set_text (GTK_ENTRY (special_ip_address), prefs.dcc_ip_str);
		gtk_widget_set_sensitive (special_ip_address, FALSE);
	}

	gtk_spin_button_set_value (GTK_SPIN_BUTTON (individual_send_throttle), (gdouble) prefs.dcc_max_send_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (global_send_throttle), (gdouble) prefs.dcc_global_max_send_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (individual_receive_throttle), (gdouble) prefs.dcc_max_get_cps);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (global_receive_throttle), (gdouble) prefs.dcc_global_max_get_cps);

	g_signal_connect (G_OBJECT (download_dir_button),         "selection-changed", G_CALLBACK (path_changed), prefs.dccdir);
	g_signal_connect (G_OBJECT (completed_dir_button),        "selection-changed", G_CALLBACK (path_changed), prefs.dcc_completed_dir);

	g_signal_connect (G_OBJECT (convert_spaces),              "toggled",           G_CALLBACK (toggle_changed), &prefs.dcc_send_fillspaces);
	g_signal_connect (G_OBJECT (save_nicknames_dcc),          "toggled",           G_CALLBACK (toggle_changed), &prefs.dccwithnick);
	g_signal_connect (G_OBJECT (autoaccept_dcc_chat),         "toggled",           G_CALLBACK (toggle_changed), &prefs.autodccchat);
	g_signal_connect (G_OBJECT (autoaccept_dcc_file),         "toggled",           G_CALLBACK (toggle_changed), &prefs.autodccsend);

	g_signal_connect (G_OBJECT (get_dcc_ip_from_server),      "toggled",           G_CALLBACK (get_ip_from_server_changed), page);
	g_signal_connect (G_OBJECT (special_ip_address),          "changed",           G_CALLBACK (special_ip_changed),         NULL);

	g_signal_connect (G_OBJECT (individual_send_throttle),    "value-changed",     G_CALLBACK (spin_changed), &prefs.dcc_max_send_cps);
	g_signal_connect (G_OBJECT (global_send_throttle),        "value-changed",     G_CALLBACK (spin_changed), &prefs.dcc_global_max_send_cps);
	g_signal_connect (G_OBJECT (individual_receive_throttle), "value-changed",     G_CALLBACK (spin_changed), &prefs.dcc_max_get_cps);
	g_signal_connect (G_OBJECT (global_receive_throttle),     "value-changed",     G_CALLBACK (spin_changed), &prefs.dcc_global_max_get_cps);

	GtkIconTheme *theme = gtk_icon_theme_get_default ();
	PREFERENCES_PAGE (page)->icon = gtk_icon_theme_load_icon (theme, "xchat-gnome-dcc", 16, 0, NULL);

	GtkTreeIter iter;
	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, PREFERENCES_PAGE (page)->icon, 1, _("File Transfers & DCC"), 2, 3, -1);

	return page;
}

static void
preferences_page_dcc_init (PreferencesPageDCC *page)
{
}

static void
preferences_page_dcc_class_init (PreferencesPageDCCClass *klass)
{
}
