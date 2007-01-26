/*
 * plugins.h - handle (auto)loading and unloading of plugins
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

#include "navigation-tree.h"
#include "taco-bar.h"
#include "../common/xchat.h"

#ifndef XCHAT_GNOME_PLUGINS_H
#define XCHAT_GNOME_PLUGINS_H

#ifdef PLUGIN_C

/* This is our own plugin struct that we use for passing in function
 * pointers specific to our GUI. It must be kept identical to xg-plugin.h.
 */
struct _xchat_gnome_plugin
{
	GtkWidget    *(*xg_get_main_window) ();
	GtkTreeModel *(*xg_get_chan_list)   ();
	TacoBar	     *(*xg_get_taco_bar)    ();
	GtkUIManager *(*xg_get_ui_manager)  ();
};
#endif

extern GSList *enabled_plugins;

void  plugins_initialize (void);
void  autoload_plugins   (void);
int   unload_plugin      (char *filename);
char *load_plugin        (session * sess, char *filename, char *arg, gboolean script, gboolean autoload);

#endif
