#include <glade/glade.h>
#include <gnome.h>
#include "xtext.h"
#include "../common/xchat.h"

#ifndef XCHAT_GNOME_GUI_H
#define XCHAT_GNOME_GUI_H

typedef struct {
	GladeXML *xml;
	GnomeApp *main_window;
	GtkDialog *preferences_dialog;
	GnomeAbout *about;
	GtkXText *xtext;
	session *current_session;
} XChatGUI;

extern XChatGUI gui;

gboolean initialize_gui();
int xtext_get_stamp_str (time_t tim, char **ret);

#endif
