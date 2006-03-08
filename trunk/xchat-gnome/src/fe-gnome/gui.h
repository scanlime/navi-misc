/*
 * gui.h - main gui information structure
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

#ifndef XCHAT_GNOME_GUI_H
#define XCHAT_GNOME_GUI_H

#include <glade/glade.h>
#include <gtk/gtkactiongroup.h>
#include <gtk/gtkuimanager.h>
#include "userlist.h"
#include "navigation-tree.h"
#include "dcc-window.h"
#include "preferences-dialog.h"
#include "xtext.h"

#include "../common/xchat.h"

typedef struct
{
	GladeXML *xml;
	GladeXML *setup_xml;
	GtkActionGroup *action_group;
	GtkUIManager *manager;
	GtkWidget *about;
	PreferencesDialog *prefs_dialog;
	DccWindow *dcc;
	GtkExpander *topic_expander;
	GtkWidget *userlist_window;
	GtkWidget *userlist;
	GtkWidget *userlist_toggle;
	session *current_session;
	gboolean quit;

	GtkWidget *conversation_panel;
	GtkWidget *find_bar;
	GtkWidget *main_window;
	GtkWidget *status_bar;
	GtkWidget *text_entry;
	GtkWidget *topic_label;

	NavModel *tree_model;
	NavTree *server_tree;
} XChatGUI;

extern XChatGUI gui;
extern Userlist *u;

gboolean initialize_gui_1 (void);
gboolean initialize_gui_2 (void);
int xtext_get_stamp_str (time_t tim, char **ret);

#endif
