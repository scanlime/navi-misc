/*
 * gui.c - main gui initialization and helper functions
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
#include <string.h>
#include <glib/gi18n.h>
#include "gui.h"
#include "main-window.h"
#include "preferences-dialog.h"
#include "connect-dialog.h"
#include "about.h"
#include "userlist-gui.h"
#include "pixmaps.h"
#include "util.h"
#include "../common/text.h"
#include "../common/xchatc.h"

XChatGUI gui;
Userlist *u;

gboolean
initialize_gui_1 (void)
{
	gui.initialized = FALSE;

	gui.manager = gtk_ui_manager_new ();

	gchar *path = locate_data_file ("xchat-gnome.glade");
	gui.xml = glade_xml_new (path, NULL, NULL);
	g_free (path);

	if (!gui.xml) {
		return FALSE;
	}
	return TRUE;
}

gboolean
initialize_gui_2 (void)
{
	GtkWidget *widget;

	gtk_window_set_default_icon_name ("xchat-gnome");

	gui.current_session = NULL;
	gui.tree_model = navigation_model_new ();
	gui.server_tree = navigation_tree_new (gui.tree_model);
	pixmaps_init ();
	initialize_userlist ();
	initialize_main_window ();

	widget = glade_xml_get_widget (gui.xml, "server channel list");
	gtk_widget_show (GTK_WIDGET (gui.server_tree));
	gtk_container_add (GTK_CONTAINER (widget), GTK_WIDGET (gui.server_tree));

	gui.dcc = dcc_window_new ();

	gui.initialized = TRUE;

	set_action_state (gui.server_tree);

	return TRUE;
}

int
xtext_get_stamp_str (time_t tim, char **ret)
{
	if (strlen (prefs.stamp_format) == 0) {
		strncpy (prefs.stamp_format, "[%H:%M:%S] ", 11);
		prefs.stamp_format[11] = '\0';
	}
	return get_stamp_str (prefs.stamp_format, tim, ret);
}
