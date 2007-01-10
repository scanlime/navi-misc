/*
 * preferences-page-plugin-notification.h - Preferences page of the
 * notification plugin
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_PLUGIN_NOTIFICATION_H
#define XCHAT_GNOME_PREFERENCES_PAGE_PLUGIN_NOTIFICATION_H

#include <gconf/gconf-client.h>

#include "preferences-page.h"

G_BEGIN_DECLS

typedef struct _PreferencesPagePluginNotification      PreferencesPagePluginNotification;
typedef struct _PreferencesPagePluginNotificationClass PreferencesPagePluginNotificationClass;
#define PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE            (preferences_page_plugin_notification_get_type ())
#define PREFERENCES_PAGE_PLUGIN_NOTIFICATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE, PreferencesPagePluginNotification))
#define PREFERENCES_PAGE_PLUGIN_NOTIFICATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE, PreferencesPagePluginNotificationClass))
#define IS_PREFERENCES_PAGE_PLUGIN_NOTIFICATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE))
#define IS_PREFERENCES_PAGE_PLUGIN_NOTIFICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE))

struct _PreferencesPagePluginNotification 
{
	PreferencesPage parent;

	GConfClient *gconf;
	guint notify;

	GtkWidget *notif_level_3;
	GtkWidget *notif_level_2;
	GtkWidget *notif_level_1;
	GtkWidget *notif_level_0;
};

struct _PreferencesPagePluginNotificationClass
{
	PreferencesPageClass parent_class;
};

GType              			preferences_page_plugin_notification_get_type (void) G_GNUC_CONST;
PreferencesPagePluginNotification* 	preferences_page_plugin_notification_new (void);

G_END_DECLS

#endif
