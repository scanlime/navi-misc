/*
 * userlist.h - helpers for the userlist view
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
#include "textgui.h"

#ifndef XCHAT_GNOME_USERLIST_H
#define XCHAT_GNOME_USERLIST_H

void initialize_userlist();
void create_userlist(session *sess);
void userlist_insert(session *sess, struct User *newuser, int row, int sel);
gboolean userlist_remove(session *sess, struct User *user);
void userlist_change(session *sess, struct User *user);
void userlist_display(session_gui *sess);

#endif
