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
#include "userlist.h"
#include "pixmaps.h"

XChatGUI gui;

gboolean initialize_gui() {
	gui.xml = glade_xml_new("xchat-gnome.glade", NULL, NULL);
	if(!gui.xml)
		return FALSE;
	gui.current_session = NULL;
	pixmaps_init();
	initialize_main_window();
	initialize_text_gui();
	initialize_preferences_dialog();
	initialize_connection_dialog();
	initialize_navigation_tree();
	initialize_userlist();
	return TRUE;
}

int xtext_get_stamp_str (time_t tim, char **ret) {
	return get_stamp_str("[%H:%M:%S] ", tim, ret);
}
