#include "gui.h"
#include "../common/xchat.h"
#include "xtext.h"

#ifndef XCHAT_GNOME_TEXTGUI_H
#define XCHAT_GNOME_TEXTGUI_H

void initialize_text_gui();
void text_gui_add_text_buffer(struct session *sess);
void text_gui_print(xtext_buffer *buf, unsigned char *text, gboolean indent);

typedef struct {
	xtext_buffer *buffer;
} session_gui;

#endif
