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
#include "preferences-page.h"

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_IRC_H
#define XCHAT_GNOME_PREFERENCES_PAGE_IRC_H

G_BEGIN_DECLS

typedef struct _PreferencesPageIrc      PreferencesPageIrc;
typedef struct _PreferencesPageIrcClass PreferencesPageIrcClass;
#define PREFERENCES_PAGE_IRC_TYPE            (preferences_page_irc_get_type ())
#define PREFERENCES_PAGE_IRC(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_IRC_TYPE, PreferencesPageIrc))
#define PREFERENCES_PAGE_IRC_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_IRC_TYPE, PreferencesPageIrcClass))
#define IS_PREFERENCES_PAGE_IRC(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_IRC_TYPE))
#define IS_PREFERENCES_PAGE_IRC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_IRC_TYPE))

struct _PreferencesPageIrc
{
	PreferencesPage parent;

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

	GtkListStore *highlight_store;
	GtkTreeViewColumn *highlight_column;

	/* gconf notification handlers */
	guint nh[8];
};

struct _PreferencesPageIrcClass
{
	PreferencesPageClass parent_class;
};

GType              	preferences_page_irc_get_type (void) G_GNUC_CONST;
PreferencesPageIrc* preferences_page_irc_new  (gpointer prefs_dialog, GladeXML *xml);

G_END_DECLS

#endif
