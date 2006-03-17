/*
 * preferences-dialog.h - helpers for the preference dialog
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

#include <gconf/gconf-client.h>
#include "gui.h"
#include "preferences-page-irc.h"
#include "preferences-page-colors.h"
#include "preferences-page-effects.h"
#include "preferences-page-dcc.h"
#include "preferences-page-networks.h"
#include "preferences-page-plugins.h"
#include "preferences-page-spellcheck.h"

#ifndef XCHAT_GNOME_PREFERENCES_DIALOG_H
#define XCHAT_GNOME_PREFERENCES_DIALOG_H

G_BEGIN_DECLS

typedef struct _PreferencesDialog      PreferencesDialog;
typedef struct _PreferencesDialogClass PreferencesDialogClass;
#define PREFERENCES_DIALOG_TYPE            (preferences_dialog_get_type ());
#define PREFERENCES_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PREFERENCES_DIALOG_TYPE, PreferencesDialog))
#define PREFERENCES_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PREFERENCES_DIALOG_TYPE, PreferencesDialogClass))
#define IS_PREFERENCES_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PREFERENCES_DIALOG_TYPE))
#define IS_PREFERENCES_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PREFERENCES_DIALOG_TYPE))

struct _PreferencesDialog
{
	GObject parent;

	GConfClient *gconf;

	GtkWidget *dialog;
	GtkWidget *settings_page_list;
	GtkWidget *settings_notebook;

	GtkListStore *page_store;

	PreferencesIrcPage        *irc_page;
	PreferencesColorsPage     *colors_page;
	PreferencesEffectsPage    *effects_page;
	PreferencesDCCPage        *dcc_page;
	PreferencesNetworksPage   *networks_page;
	PreferencesPluginsPage    *plugins_page;
	PreferencesSpellcheckPage *spellcheck_page;
};

struct _PreferencesDialogClass
{
	GObjectClass parent_class;
};

GType              preferences_dialog_get_type (void) G_GNUC_CONST;
PreferencesDialog *preferences_dialog_new (void);
void               preferences_dialog_show (PreferencesDialog *dialog);

G_END_DECLS

#endif
