#include "gui.h"
#include "main_window.h"
#include "preferences_dialog.h"
#include "connect_dialog.h"
#include "navigation_tree.h"
#include "about.h"
#include "textgui.h"
#include "userlist.h"

XChatGUI gui;

gboolean initialize_gui() {
	gui.xml = glade_xml_new("xchat-gnome.glade", NULL, NULL);
	if(!gui.xml)
		return FALSE;
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
