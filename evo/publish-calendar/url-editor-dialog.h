/*
 * Authors:
 *  David Trowbridge
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
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

#ifndef _URL_EDITOR_DIALOG_H_
#define _URL_EDITOR_DIALOG_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <libedataserverui/e-source-selector.h>

G_BEGIN_DECLS

#define URL_EDITOR_DIALOG_TYPE            (url_editor_dialog_get_type2 ())
#define URL_EDITOR_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), URL_EDITOR_DIALOG, UrlEditorDialog))
#define URL_EDITOR_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), URL_EDITOR_DIALOG, UrlEditorDialogClass))
#define IS_URL_EDITOR_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), URL_EDITOR_DIALOG))
#define IS_URL_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), URL_EDITOR_DIALOG))

enum {
	URL_LIST_ENABLED_COLUMN,
	URL_LIST_LOCATION_COLUMN,
	URL_LIST_URL_COLUMN,
	URL_LIST_N_COLUMNS,
};

typedef struct _UrlEditorDialog2 UrlEditorDialog2;
typedef struct _UrlEditorDialogClass2 UrlEditorDialogClass2;

struct _UrlEditorDialog2 {
	GtkDialog parent;

	GtkTreeModel *url_list_model;
	GladeXML *gui;

	GtkWidget *url_entry;
	GtkWidget *publish_frequency;

	GtkWidget *type_selector;
	GtkWidget *publish_events;
	GtkWidget *events_swin;
	GtkWidget *publish_tasks;
	GtkWidget *tasks_swin;

	GtkWidget *username_entry;
	GtkWidget *password_entry;
	GtkWidget *remember_pw;

	GtkWidget *ok;
	GtkWidget *cancel;
};

struct _UrlEditorDialogClass2 {
	GtkDialogClass parent_class;
};

GtkWidget *url_editor_dialog_new2 (GtkTreeModel *url_list_model);
GType      url_editor_dialog_get_type2 (void);
void       url_editor_dialog_run2 (UrlEditorDialog2 *dialog);

G_END_DECLS

#endif /* _URL_EDITOR_DIALOG_H_ */
