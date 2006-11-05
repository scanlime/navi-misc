/*
 * preferences-page-networks.h - helpers for the servers preferences page
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

#include <glade/glade-xml.h>
#include "gui.h"
#include "preferences-page.h"

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_NETWORKS_H
#define XCHAT_GNOME_PREFERENCES_PAGE_NETWORKS_H

G_BEGIN_DECLS

typedef struct _PreferencesPageNetworks      PreferencesPageNetworks;
typedef struct _PreferencesPageNetworksClass PreferencesPageNetworksClass;
#define PREFERENCES_PAGE_NETWORKS_TYPE            (preferences_page_networks_get_type ())
#define PREFERENCES_PAGE_NETWORKS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_NETWORKS_TYPE, PreferencesPageNetworks))
#define PREFERENCES_PAGE_NETWORKS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_NETWORKS_TYPE, PreferencesPageNetworksClass))
#define IS_PREFERENCES_PAGE_NETWORKS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_NETWORKS_TYPE))
#define IS_PREFERENCES_PAGE_NETWORKS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_NETWORKS_TYPE))

struct _PreferencesPageNetworks
{
	PreferencesPage parent;

	GtkWidget *network_list;
	GtkWidget *network_add;
	GtkWidget *network_edit;
	GtkWidget *network_remove;

	GtkListStore *network_store;
	GtkTreeModelSort *sort_model;
};

struct _PreferencesPageNetworksClass
{
	PreferencesPageClass parent_class;
};

GType              	preferences_page_networks_get_type (void) G_GNUC_CONST;
PreferencesPageNetworks* preferences_page_networks_new  (gpointer prefs_dialog, GladeXML *xml);

G_END_DECLS

#endif
