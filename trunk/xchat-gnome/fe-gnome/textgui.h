/*
 * textgui.h - helpers for the main text gui
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
#include "../common/xchat.h"
#include "xtext.h"

#ifndef XCHAT_GNOME_TEXTGUI_H
#define XCHAT_GNOME_TEXTGUI_H

void initialize_text_gui();
void text_gui_add_text_buffer(struct session *sess);
void text_gui_remove_text_buffer(struct session *sess);
void text_gui_print(xtext_buffer *buf, unsigned char *text, gboolean indent);
void set_nickname(struct server *serv, char *newnick);
void set_gui_topic(struct session *sess, char *topic);
void clear_buffer(struct session *sess);

typedef struct {
	xtext_buffer *buffer;
	GtkTreeModel *userlist_model;
} session_gui;

#endif
