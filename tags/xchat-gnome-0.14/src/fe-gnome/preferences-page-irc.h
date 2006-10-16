/*
 * preferences-page-irc.h - helpers for the irc preferences page
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_IRC_H
#define XCHAT_GNOME_PREFERENCES_PAGE_IRC_H

typedef struct
{
	GtkWidget *nick_name;
	GtkWidget *real_name;
	GtkWidget *quit_message;
	GtkWidget *part_message;
	GtkWidget *away_message;

	GtkWidget *highlight_list;
	GtkWidget *highlight_add;
	GtkWidget *highlight_edit;
	GtkWidget *highlight_remove;

	GtkWidget *usesysfonts;
	GtkWidget *usethisfont;
	GtkWidget *font_selection;

	GtkWidget *auto_logging;
	GtkWidget *show_timestamps;
	GtkWidget *show_marker;

	GdkPixbuf *icon;
	GtkListStore *highlight_store;
	GtkTreeViewColumn *highlight_column;

	/* gconf notification handlers */
	guint nh[8];
} PreferencesIrcPage;

PreferencesIrcPage *preferences_page_irc_new  (gpointer prefs_dialog, GladeXML *xml);
void                preferences_page_irc_free (PreferencesIrcPage *page);

#endif
