/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 * Authors:
 *  David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 David Trowbridge
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

#include "url-editor-dialog.h"
#include <glade/glade.h>

static GtkDialogClass *parent_class = NULL;

static void
url_editor_dialog_finalize (GObject *obj)
{
	UrlEditorDialog *dialog = (UrlEditorDialog *) obj;

	g_object_unref (dialog->url_list_model);

	((GObjectClass *)(parent_class))->finalize (obj);
}

static void
url_editor_dialog_class_init (UrlEditorDialogClass *klass)
{
	GObjectClass *object_class;

	object_class = (GObjectClass *) klass;
	parent_class = g_type_class_ref (GTK_TYPE_DIALOG);

	object_class->finalize = url_editor_dialog_finalize;
}

static void
url_editor_dialog_init (UrlEditorDialog *dialog)
{
}


GtkWidget *
url_editor_dialog_new (GtkTreeModel *url_list_model, EPublishUri *pub_uri)
{
	UrlEditorDialog *dialog;

	dialog = (UrlEditorDialog *) g_object_new (url_editor_dialog_get_type (), NULL);
	url_editor_dialog_construct (dialog);

	return (GtkWidget *) dialog;
}

GType
url_editor_dialog_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static GTypeInfo type_info = {
			sizeof (UrlEditorDialog),
			NULL, NULL,
			(GClassInitFunc) url_editor_dialog_class_init,
			NULL, NULL,
			sizeof (UrlEditorDialog),
			0,
			(GInstanceInitFunc) url_editor_dialog_init,
		};

		type = g_type_register_static (GTK_TYPE_DIALOG, "UrlEditorDialog", &type_info, 0);
	}

	return type;
}
