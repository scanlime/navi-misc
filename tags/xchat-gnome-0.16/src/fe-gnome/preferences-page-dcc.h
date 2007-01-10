/*
 * preferences-page-dcc.h - helpers for the DCC preferences page
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

#include <glade/glade-xml.h>
#include "gui.h"
#include "preferences-page.h"

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_DCC_H
#define XCHAT_GNOME_PREFERENCES_PAGE_DCC_H

G_BEGIN_DECLS

typedef struct _PreferencesPageDCC      PreferencesPageDCC;
typedef struct _PreferencesPageDCCClass PreferencesPageDCCClass;
#define PREFERENCES_PAGE_DCC_TYPE            (preferences_page_dcc_get_type ())
#define PREFERENCES_PAGE_DCC(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_DCC_TYPE, PreferencesPageDCC))
#define PREFERENCES_PAGE_DCC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_DCC_TYPE, PreferencesPageDCCClass))
#define IS_PREFERENCES_PAGE_DCC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_DCC_TYPE))
#define IS_PREFERENCES_PAGE_DCC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_DCC_TYPE))


struct _PreferencesPageDCC
{
	PreferencesPage parent;

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
};

struct _PreferencesPageDCCClass
{
	PreferencesPageClass parent_class;
};

GType              	preferences_page_dcc_get_type (void) G_GNUC_CONST;
PreferencesPageDCC* preferences_page_dcc_new  (gpointer prefs_dialog, GladeXML *xml);

G_END_DECLS

#endif
