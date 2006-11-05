/*
 * preferences-page-plugin-notification.c - Preferences page of the
 * notifcation plugin
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

#include "preferences-page.h"
#include "preferences-page-plugin-notification.h"
#include "util.h"

#define NOTIFICATION_KEY "/apps/xchat/plugins/notification/level"

G_DEFINE_TYPE(PreferencesPagePluginNotification, preferences_page_plugin_notification, PREFERENCES_PAGE_TYPE)

static void
preferences_page_plugin_notification_finalize (GObject *object)
{
	PreferencesPagePluginNotification *page = (PreferencesPagePluginNotification *) object;
	/* The vbox is already destroyed when the page is removed from the
	 * gtk_notebook */

	gconf_client_notify_remove (page->gconf, page->notify);
	g_object_unref (page->gconf);

	G_OBJECT_CLASS (preferences_page_plugin_notification_parent_class)->finalize (object);
}

static void
preferences_page_plugin_notification_class_init (PreferencesPagePluginNotificationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = preferences_page_plugin_notification_finalize;
}

PreferencesPagePluginNotification*
preferences_page_plugin_notification_new (void)
{
	return g_object_new (PREFERENCES_PAGE_PLUGIN_NOTIFICATION_TYPE, NULL);
}

static void
notif_changed (GtkToggleButton *button, PreferencesPagePluginNotification *page)
{
	gint level;

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->notif_level_3)))
		level = 3;
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->notif_level_2)))
		level = 2;
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (page->notif_level_1)))
		level = 1;
	else
		level = 0;

	gconf_client_set_int (page->gconf, NOTIFICATION_KEY, level, NULL);
}

static void
preferences_page_plugin_notification_set_notif_level (PreferencesPagePluginNotification *page, gint level)
{
	switch (level)
	{
		case 3:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->notif_level_3), TRUE);
			break;
		case 2:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->notif_level_2), TRUE);
			break;
		case 1:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->notif_level_1), TRUE);
			break;
		case 0:
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (page->notif_level_0), TRUE);
	}
}

static void
gconf_notif_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, PreferencesPagePluginNotification *page)
{
	gint level;

	g_signal_handlers_block_by_func (G_OBJECT (page->notif_level_3), G_CALLBACK (notif_changed), page);
	g_signal_handlers_block_by_func (G_OBJECT (page->notif_level_2), G_CALLBACK (notif_changed), page);
	g_signal_handlers_block_by_func (G_OBJECT (page->notif_level_1), G_CALLBACK (notif_changed), page);
	g_signal_handlers_block_by_func (G_OBJECT (page->notif_level_0), G_CALLBACK (notif_changed), page);

	level = gconf_client_get_int (client, entry->key, NULL);
	preferences_page_plugin_notification_set_notif_level (page, level);

	g_signal_handlers_unblock_by_func (G_OBJECT (page->notif_level_3), G_CALLBACK (notif_changed), page);
	g_signal_handlers_unblock_by_func (G_OBJECT (page->notif_level_2), G_CALLBACK (notif_changed), page);
	g_signal_handlers_unblock_by_func (G_OBJECT (page->notif_level_1), G_CALLBACK (notif_changed), page);
	g_signal_handlers_unblock_by_func (G_OBJECT (page->notif_level_0), G_CALLBACK (notif_changed), page);
}

static void
preferences_page_plugin_notification_init (PreferencesPagePluginNotification *page)
{
	GdkPixbuf *icon;
	gchar *path;
	GladeXML *xml = NULL;
	gint level;
	GtkIconTheme *theme = gtk_icon_theme_get_default ();
	
	path = locate_data_file ("preferences-notification.glade");
	xml = glade_xml_new (path, "vbox_notification", NULL);
	g_free (path);

	PREFERENCES_PAGE (page)->vbox = glade_xml_get_widget (xml, "vbox_notification");

	icon = gtk_icon_theme_load_icon (theme, "xchat-gnome", 16, 0, NULL);
	PREFERENCES_PAGE (page)->icon = icon;

	PREFERENCES_PAGE (page)->title = g_strdup (_("Notification"));

	page->gconf = gconf_client_get_default ();
	page->notify = gconf_client_notify_add (page->gconf, NOTIFICATION_KEY,
				(GConfClientNotifyFunc) gconf_notif_changed, page, NULL, NULL);

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(notif_level_3);
	GW(notif_level_2);
	GW(notif_level_1);
	GW(notif_level_0);
#undef GW

	level = gconf_client_get_int (page->gconf, NOTIFICATION_KEY, NULL);
	preferences_page_plugin_notification_set_notif_level (page, level);

	g_signal_connect (G_OBJECT (page->notif_level_3), "clicked", G_CALLBACK (notif_changed), page);
	g_signal_connect (G_OBJECT (page->notif_level_2), "clicked", G_CALLBACK (notif_changed), page);
	g_signal_connect (G_OBJECT (page->notif_level_1), "clicked", G_CALLBACK (notif_changed), page);
	g_signal_connect (G_OBJECT (page->notif_level_0), "clicked", G_CALLBACK (notif_changed), page);
}
