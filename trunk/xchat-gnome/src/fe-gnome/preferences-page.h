/*
 * preferences-page.h - A preference page
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_H
#define XCHAT_GNOME_PREFERENCES_PAGE_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _PreferencesPage 	 PreferencesPage;
typedef struct _PreferencesPageClass	 PreferencesPageClass;
#define PREFERENCES_PAGE_TYPE            (preferences_page_get_type ())
#define PREFERENCES_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_TYPE, PreferencesPage))
#define PREFERENCES_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_TYPE, PreferencesPageClass))
#define IS_PREFERENCES_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_TYPE))
#define IS_PREFERENCES_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_TYPE))

struct _PreferencesPage 
{
	GObject parent;

	GtkWidget *vbox;
	GdkPixbuf *icon;
	gchar	  *title;
};

struct _PreferencesPageClass
{
	GObjectClass parent_class;

};

GType              	preferences_page_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
