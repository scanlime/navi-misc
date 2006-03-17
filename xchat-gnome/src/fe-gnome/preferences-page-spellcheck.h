/*
 * preferences-page-spellcheck.h - helpers for the spellcheck preferences page
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_SPELLCHECK_H
#define XCHAT_GNOME_PREFERENCES_PAGE_SPELLCHECK_H

typedef struct
{
	GdkPixbuf *icon;

	GtkWidget *enable_spellcheck;
	GtkWidget *spellcheck_list;
	GtkWidget *vbox_langs;
	GtkCellRenderer *activated_renderer, *name_renderer;
	GtkListStore *spellcheck_store;
	GtkTreeViewColumn *spellcheck_column;

	/* gconf notification handlers */
	guint nh[2];
} PreferencesSpellcheckPage;

PreferencesSpellcheckPage *preferences_page_spellcheck_new  (gpointer prefs_dialog, GladeXML *xml);
void                preferences_page_spellcheck_free (PreferencesSpellcheckPage *page);

#endif
