#include "gui.h"
#include "main_window.h"
#include "preferences_dialog.h"

XChatGUI gui;

gboolean initialize_gui() {
	gui.xml = glade_xml_new("xchat-gnome.glade", NULL, NULL);
	if(!gui.xml)
		return FALSE;
	initialize_main_window();
	initialize_preferences_dialog();
	return TRUE;
}

int xtext_get_stamp_str (time_t tim, char **ret) {
	return get_stamp_str("[%H:%M:%S] ", tim, ret);
}
