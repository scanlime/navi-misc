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
#include <string.h>
#include <libedataserverui/e-source-selector.h>

static GtkDialogClass *parent_class = NULL;

static void
url_editor_dialog_finalize (GObject *obj)
{
	UrlEditorDialog *dialog = (UrlEditorDialog *) obj;

	g_object_unref (dialog->url_list_model);
	g_object_unref (dialog->gui);

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

static void
init_widgets (UrlEditorDialog *dialog)
{
#define GW(name) ((dialog->name) = glade_xml_get_widget (dialog->gui, #name))
	GW(url_editor);
	GW(calendar_list_label);
	GW(url_entry);
	GW(daily);
	GW(weekly);
	GW(user_publish);
	GW(scrolled_window);
	GW(username_entry);
	GW(password_entry);
	GW(remember_pw);
#undef GW
}

static void
url_editor_dialog_construct (UrlEditorDialog *dialog)
{
	GladeXML *gui;
	GtkWidget *toplevel;
	EPublishUri *uri;

	gui = glade_xml_new (EVOLUTION_GLADEDIR "/url-editor-dialog.glade", "toplevel_dialog", NULL);
	dialog->gui = gui;

	g_return_if_fail (gui != NULL);

	init_widgets (dialog);

	uri = dialog->uri;

	if (uri->location && uri->username) {
		if (strlen (uri->location) != 0)
			gtk_entry_set_text (GTK_ENTRY (dialog->url_entry), uri->location);
		if (strlen (uri->username) != 0)
			gtk_entry_set_text (GTK_ENTRY (dialog->username_entry), uri->username);
	}

	uri->password = e_passwords_get_password ("Calendar", uri->location);

	if (uri->password) {
		if (strlen (uri->password) != 0) {
			gtk_entry_set_text (dialog->password_entry, uri->password);
			e_dialog_toggle_set (dialog->remember_pw, TRUE);
		} else {
			e_dialog_toggle_set (dialog->remember_pw, FALSE);
		}
	}

	switch (uri->publish_freq) {
	}


	toplevel = glade_xml_get_widget (gui, "toplevel_dialog");
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), toplevel);
}

GtkWidget *
url_editor_dialog_new (GtkTreeModel *url_list_model, EPublishUri *pub_uri)
{
	UrlEditorDialog *dialog;

	dialog = (UrlEditorDialog *) g_object_new (url_editor_dialog_get_type (), NULL);

	dialog->uri = pub_uri;

	url_editor_dialog_construct (dialog);

	if (dialog->gui) {
		return GTK_WIDGET (dialog);
	} else {
		g_object_unref (dialog);
		return NULL;
	}
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
