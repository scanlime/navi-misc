/*
 * plugins.h - handle (auto)loading and unloading of plugins
 *
 * Copyright (C) 2005 David Trowbridge
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

#ifndef XCHAT_GNOME_PLUGINS_H
#define XCHAT_GNOME_PLUGINS_H

#ifdef PLUGIN_C
/* This is our own plugin struct that we use for passing in function
 * pointers specific to our GUI.
 */
typedef struct _xchat_gnome_plugin
{
	NavTree *(*xg_get_nav_tree) (xchat_gnome_plugin *ph);
} xchat_gnome_plugin;

#else
extern GSList *enabled_plugins;

void plugins_initialize ();
void autoload_plugins ();
int unload_plugin (char *filename);
char *load_plugin (session *sess, char *filename, char *arg);
#endif

#endif

#ifdef PLUGIN_C
#ifndef XG_PLUGIN_HANDLE
#define XG_PLUGIN_HANDLE (ph)
#endif
#define xg_get_nav_tree ((XG_PLUGIN_HANDLE)->xg_get_nav_tree)
#endif
