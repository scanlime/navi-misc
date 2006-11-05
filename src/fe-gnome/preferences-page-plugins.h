/*
 * preferences-page-plugins.h - helpers for the plugins preferences page
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

#include "gui.h"
#include "preferences-page.h"

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_PLUGINS_H
#define XCHAT_GNOME_PREFERENCES_PAGE_PLUGINS_H

G_BEGIN_DECLS

typedef struct _PreferencesPagePlugins      PreferencesPagePlugins;
typedef struct _PreferencesPagePluginsClass PreferencesPagePluginsClass;
#define PREFERENCES_PAGE_PLUGINS_TYPE            (preferences_page_plugins_get_type ())
#define PREFERENCES_PAGE_PLUGINS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_PLUGINS_TYPE, PreferencesPagePlugins))
#define PREFERENCES_PAGE_PLUGINS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_PLUGINS_TYPE, PreferencesPagePluginsClass))
#define IS_PREFERENCES_PAGE_PLUGINS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_PLUGINS_TYPE))
#define IS_PREFERENCES_PAGE_PLUGINS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_PLUGINS_TYPE))

struct _PreferencesPagePlugins 
{
	PreferencesPage parent;

	GtkWidget *plugins_list;
	GtkWidget *plugins_open;
	GtkWidget *plugins_remove;

	GtkListStore *plugin_store;
	GtkCellRenderer *text_renderer, *load_renderer;
	GtkTreeViewColumn *text_column, *load_column;
};

struct _PreferencesPagePluginsClass
{
	PreferencesPageClass parent_class;

	/* signals */
	void (* new_plugin_page)		(PreferencesPagePlugins *page,
						 PreferencesPage *page_plugin);
	void (* remove_plugin_page)		(PreferencesPagePlugins *page,
						 gchar *plugin_name);

};

GType              	preferences_page_plugins_get_type (void) G_GNUC_CONST;
PreferencesPagePlugins* preferences_page_plugins_new  (gpointer prefs_dialog, GladeXML *xml);
void			preferences_page_plugins_check_plugins (PreferencesPagePlugins *page);

G_END_DECLS

/* This is a small helper function to autoload the plugins we had enabled
 * last session. We put this here instead of just doing from the initialize
 * function so that we can load the plugins after the window is created. It
 * seems to me that's the kind of behaviour you'd expect (having the windows
 * of the plugins show up after the main window, not before).
 */
void autoload_plugins ();
#endif
