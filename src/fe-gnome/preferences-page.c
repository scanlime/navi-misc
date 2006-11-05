/*
 * preferences-page.c - A preference page
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

#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "preferences-page.h"

G_DEFINE_TYPE(PreferencesPage, preferences_page, G_TYPE_OBJECT)

static void
preferences_page_dispose (GObject *object)
{
	PreferencesPage *page = PREFERENCES_PAGE (object);

	if (page->icon)
	{
		g_object_unref (page->icon);
		page->icon = NULL;
	}

	G_OBJECT_CLASS (preferences_page_parent_class)->dispose (object);
}

static void
preferences_page_finalize (GObject *object)
{
	PreferencesPage *page = PREFERENCES_PAGE (object);

	if (page->title) {
		g_free (page->title);
		page->title = NULL;
	}

	G_OBJECT_CLASS (preferences_page_parent_class)->finalize (object);
}

static void
preferences_page_class_init (PreferencesPageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = preferences_page_dispose;
	object_class->finalize = preferences_page_finalize;
}

static void
preferences_page_init (PreferencesPage *page)
{
	page->vbox = NULL;
	page->icon = NULL;
	page->title = NULL;
}
