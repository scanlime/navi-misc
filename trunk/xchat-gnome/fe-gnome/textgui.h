#include "gui.h"
#include "../common/xchat.h"
#include "xtext.h"

#ifndef XCHAT_GNOME_TEXTGUI_H
#define XCHAT_GNOME_TEXTGUI_H

void initialize_text_gui();
void text_gui_add_text_buffer(struct session *sess);
void text_gui_print(xtext_buffer *buf, unsigned char *text, gboolean indent);
void set_nickname(struct server *serv, char *newnick);
void set_gui_topic(struct session *sess, char *topic);

typedef struct {
	xtext_buffer *buffer;
	GtkTreeModel *userlist_model;
} session_gui;

#endif
