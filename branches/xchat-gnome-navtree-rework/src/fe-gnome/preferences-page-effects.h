/*
 * preferences-page-effects.h - helpers for the effects preferences page
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

#ifndef XCHAT_GNOME_PREFERENCES_PAGE_EFFECTS_H
#define XCHAT_GNOME_PREFERENCES_PAGE_EFFECTS_H

G_BEGIN_DECLS

typedef struct _PreferencesPageEffects      PreferencesPageEffects;
typedef struct _PreferencesPageEffectsClass PreferencesPageEffectsClass;
#define PREFERENCES_PAGE_EFFECTS_TYPE            (preferences_page_effects_get_type ())
#define PREFERENCES_PAGE_EFFECTS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_PAGE_EFFECTS_TYPE, PreferencesPageEffects))
#define PREFERENCES_PAGE_EFFECTS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_PAGE_EFFECTS_TYPE, PreferencesPageEffectsClass))
#define IS_PREFERENCES_PAGE_EFFECTS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_PAGE_EFFECTS_TYPE))
#define IS_PREFERENCES_PAGE_EFFECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_PAGE_EFFECTS_TYPE))

struct _PreferencesPageEffects
{
	PreferencesPage parent;

	GtkWidget *background_none;
	GtkWidget *background_image;
	GtkWidget *background_transparent;
	GtkWidget *background_image_file;
	GtkWidget *background_transparency;
	GtkWidget *image_preview;

	/* gconf notification handlers */
	guint nh[3];
};

struct _PreferencesPageEffectsClass
{
	PreferencesPageClass parent_class;
};

GType              	preferences_page_effects_get_type (void) G_GNUC_CONST;
PreferencesPageEffects* preferences_page_effects_new  (gpointer prefs_dialog, GladeXML *xml);

G_END_DECLS

#endif
