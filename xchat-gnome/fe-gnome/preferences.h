/*
 * preferences.h - interface to storing preferences
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

#include <gnome.h>
#include "../common/xchat.h"
#include "../common/servlist.h"

#ifndef XCHAT_GNOME_PREFERENCES_H
#define XCHAT_GNOME_PREFERENCES_H

gboolean preferences_exist();
void load_preferences();
char *preferences_nickname();
char *preferences_realname();
char *preferences_quitmsg();
char *preferences_partmsg();
char *preferences_awaymsg();
int preferences_get_color_scheme();
int preferences_get_palette_scheme();
void preferences_set_color_scheme(int selection);
void preferences_set_palette_scheme(int selection);
void preferences_set_main_window_size(int width, int height);
void preferences_set_channel_list_window_size(int width, int height);
void preferences_get_main_window_size(int *width, int *height);
void preferences_get_channel_list_window_size(int *width, int *height);
void preferences_set_main_window_v_position(int v);
void preferences_set_main_window_h_position(int h);
void preferences_get_main_window_positions(int *v, int *h);

#endif
