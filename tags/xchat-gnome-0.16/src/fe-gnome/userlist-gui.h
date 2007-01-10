/*
 * userlist-gui.h - helpers for the userlist view
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

#include "gui.h"

#ifndef XCHAT_GNOME_USERLIST_GUI_H
#define XCHAT_GNOME_USERLIST_GUI_H

void initialize_userlist (void);
void userlist_gui_show   (void);
void userlist_gui_hide   (void);

extern struct User *current_user;

#endif
