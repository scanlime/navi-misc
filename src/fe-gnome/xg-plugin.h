/*
 * xg-plugin.h - handle (auto)loading and unloading of plugins
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include "navigation-tree.h"
#include "../common/xchat.h"

#ifndef XG_PLUGIN_H
#define XG_PLUGIN_H

typedef struct _xchat_gnome_plugin xchat_gnome_plugin;
#ifndef PLUGIN_C

/* This is our own plugin struct that we use for passing in function
 * pointers specific to our GUI.
 */
struct _xchat_gnome_plugin
{
	GtkWidget    *(*xg_get_main_window) (void);
	GtkTreeModel *(*xg_get_chan_list)   (void);
	GtkUIManager *(*xg_get_ui_manager)  (void);
};
#endif

GtkWidget    *xg_get_main_window (void);
GtkTreeModel *xg_get_chan_list   (void);
GtkUIManager *xg_get_ui_manager  (void);

#ifndef PLUGIN_C

#ifndef XG_PLUGIN_HANDLE
#define XG_PLUGIN_HANDLE (xgph)
#endif

#define xg_get_main_window ((XG_PLUGIN_HANDLE)->xg_get_main_window)
#define xg_get_chan_list   ((XG_PLUGIN_HANDLE)->xg_get_chan_list)
#define xg_get_ui_manager  ((XG_PLUGIN_HANDLE)->xg_get_ui_manager)

#endif
#endif
