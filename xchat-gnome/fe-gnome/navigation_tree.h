/*
 * navigation_tree.h - functions to create and maintain the navigation tree
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

#ifndef XCHAT_GNOME_NAVIGATION_TREE_H
#define XCHAT_GNOME_NAVIGATION_TREE_H

void initialize_navigation_tree();
void navigation_tree_create_new_network_entry(struct session *sess);
void navigation_tree_create_new_channel_entry(struct session *sess);
void navigation_tree_remove(struct session *sess);
void navigation_tree_set_channel_name(struct session *sess);
void navigation_tree_set_hilight(struct session *sess);

#endif
