/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors :
 *  Gary Ekker <gekker@novell.com>
 *  David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright 2004, Ximian, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/*
 * UrlEditorDialog - a GtkObject which handles a libglade-loaded dialog
 * to edit the calendar preference settings.
 */

#ifndef _URL_EDITOR_DIALOG_H_
#define _URL_EDITOR_DIALOG_H_

#include <gtk/gtk.h>

#include "../e-pub-utils.h"
#include <libedataserverui/e-source-selector.h>

G_BEGIN_DECLS

#define URL_EDITOR_DIALOG_TYPE            (url_editor_dialog_get_type ())
#define URL_EDITOR_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), URL_EDITOR_DIALOG, UrlEditorDialog))
#define URL_EDITOR_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), URL_EDITOR_DIALOG, UrlEditorDialogClass))
#define IS_URL_EDITOR_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), URL_EDITOR_DIALOG))
#define IS_URL_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), URL_EDITOR_DIALOG))

enum {
	URL_LIST_ENABLED_COLUMN,
	URL_LIST_LOCATION_COLUMN,
	URL_LIST_FREE_BUSY_URL_COLUMN,
	URL_LIST_N_COLUMNS
};

typedef struct _UrlEditorDialog UrlEditorDialog;
typedef struct _UrlEditorDialogClass UrlEditorDialogClass;

struct _UrlEditorDialog {
	GtkDialog parent;

	GtkWidget *child;
	GtkTreeModel *url_list_model;
	EPublishUri *url_data;
};

struct _UrlEditorDialogClass {
	GtkDialogClass parent_class;
};

GtkWidget *url_editor_dialog_new (GtkTreeModel *url_list_model, EPublishUri *pub_uri);
GType      url_editor_dialog_get_type (void);

G_END_DECLS

#endif /* _URL_EDITOR_DIALOG_H_ */
