/*
 * gui.c - main gui initialization and helper functions
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include "gui.h"
#include "main_window.h"
#include "preferences_dialog.h"
#include "connect_dialog.h"
#include "navigation_tree.h"
#include "about.h"
#include "textgui.h"
#include "userlist_gui.h"
#include "setup_druid.h"
#include "pixmaps.h"
#include "transfers.h"
#include "../common/text.h"

XChatGUI gui;
Userlist *u;

gboolean
initialize_gui_1 ()
{
	gui.xml = glade_xml_new ("xchat-gnome.glade", NULL, NULL);
	if (!gui.xml)
		gui.xml = glade_xml_new (XCHATSHAREDIR "/xchat-gnome.glade", NULL, NULL);
	if (!gui.xml)
		return FALSE;
	initialize_setup_druid ();
	return TRUE;
}

gboolean
initialize_gui_2 ()
{
	GtkWidget *widget;

	gui.current_session = NULL;
	pixmaps_init ();
	initialize_main_window ();
	initialize_text_gui ();
	initialize_preferences_dialog ();
	initialize_connection_dialog ();
	initialize_userlist ();
	initialize_transfers_window ();

	gui.tree_model = navigation_model_new ();
	gui.server_tree = navigation_tree_new (gui.tree_model);
	widget = glade_xml_get_widget (gui.xml, "server channel list");
	gtk_widget_show (GTK_WIDGET (gui.server_tree));
	gtk_container_add (GTK_CONTAINER (widget), GTK_WIDGET (gui.server_tree));

	return TRUE;
}

int
xtext_get_stamp_str (time_t tim, char **ret)
{
	return get_stamp_str ("[%H:%M:%S] ", tim, ret);
}
