/*
 * preferences-page-colors.h - helpers for the colors preferences page
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_COLORS_H
#define XCHAT_GNOME_PREFERENCES_PAGE_COLORS_H

typedef struct
{
	GtkWidget *combo;

	GtkWidget *show_colors;
	GtkWidget *color_buttons[4];
	GtkWidget *palette_buttons[32];

	GtkWidget *color_label_1;
	GtkWidget *color_label_2;
	GtkWidget *color_label_3;
	GtkWidget *color_label_4;
	GtkWidget *color_label_5;

	GtkWidget *foreground_background_hbox;
	GtkWidget *text_color_hbox;
	GtkWidget *background_color_hbox;
	GtkWidget *foreground_mark_hbox;
	GtkWidget *background_mark_hbox;

	GtkWidget *mirc_colors_box;
	GtkWidget *mirc_palette_table;
	GtkWidget *extra_colors_box;
	GtkWidget *extra_palette_table;

	GdkPixbuf *icon;

	guint      nh[2];
} PreferencesColorsPage;

PreferencesColorsPage *preferences_page_colors_new  (gpointer prefs_dialog, GladeXML *xml);
void                   preferences_page_colors_free (PreferencesColorsPage *page);

#endif
