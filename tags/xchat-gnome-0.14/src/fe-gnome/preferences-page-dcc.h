/*
 * preferences-page-dcc.h - helpers for the DCC preferences page
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

#include <glade/glade-xml.h>
#include "gui.h"

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_DCC_H
#define XCHAT_GNOME_PREFERENCES_PAGE_DCC_H

typedef struct
{
	GdkPixbuf *icon;

	GtkWidget *download_dir_button;
	GtkWidget *completed_dir_button;
	GtkWidget *convert_spaces;
	GtkWidget *save_nicknames_dcc;
	GtkWidget *autoaccept_dcc_chat;
	GtkWidget *autoaccept_dcc_file;
	GtkWidget *get_dcc_ip_from_server;
	GtkWidget *use_specified_dcc_ip;
	GtkWidget *special_ip_address;
	GtkWidget *individual_send_throttle;
	GtkWidget *global_send_throttle;
	GtkWidget *individual_receive_throttle;
	GtkWidget *global_receive_throttle;
} PreferencesDCCPage;

PreferencesDCCPage *preferences_page_dcc_new  (gpointer prefs_dialog, GladeXML *xml);
void                preferences_page_dcc_free (PreferencesDCCPage *page);

#endif
