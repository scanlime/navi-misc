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

#include "url-editor-dialog.h"

static GtkDialogClass *parent_class = NULL;

static gboolean
url_editor_dialog_construct (UrlEditorDialog *dialog)
{
	GladeXML *gui;
	GtkWidget *toplevel;

	gui = glade_xml_new (PLUGINDIR "/publish-calendar.glade", "publish vbox", NULL);
	dialog->gui = gui;

	g_return_val_if_fail (gui != NULL, FALSE);

	toplevel = glade_xml_get_widget (gui, "publish vbox");
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), toplevel);
	return TRUE;
}

GtkWidget *
url_editor_dialog_new (GtkTreeModel *url_list_model)
{
	UrlEditorDialog *dialog;

	dialog = (UrlEditorDialog *) g_object_new (URL_EDITOR_DIALOG_TYPE, NULL);
	dialog->url_list_model = g_object_ref (url_list_model);

	if (url_editor_dialog_construct (dialog))
		return GTK_WIDGET (dialog);

	g_object_unref (dialog);
	return NULL;
}

static void
url_editor_dialog_dispose (GObject *obj)
{
	UrlEditorDialog *dialog = (UrlEditorDialog *) obj;

	if (dialog->url_list_model) {
		g_object_unref (dialog->url_list_model);
		dialog->url_list_model = NULL;
	}

	((GObjectClass *)(parent_class))->dispose (obj);
}

static void
url_editor_dialog_class_init (UrlEditorDialogClass *klass)
{
	GObjectClass *object_class;

	object_class = (GObjectClass *) klass;
	parent_class = g_type_class_ref (GTK_TYPE_DIALOG);

	object_class->dispose = url_editor_dialog_dispose;
}

static void
url_editor_dialog_init (UrlEditorDialog *dialog)
{
}

GType
url_editor_dialog_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static GTypeInfo info = {
			sizeof (UrlEditorDialogClass),
			NULL, NULL,
			(GClassInitFunc) url_editor_dialog_class_init,
			NULL, NULL,
			sizeof (UrlEditorDialog),
			0,
			(GInstanceInitFunc) url_editor_dialog_init,
		};

		/* FIXME: remove 2 when UrlEditorDialog is removed from mainline */
		type = g_type_register_static (GTK_TYPE_DIALOG, "UrlEditorDialog2", &info, 0);
	}

	return type;
}

void
url_editor_dialog_run (UrlEditorDialog *dialog)
{
	/* FIXME */
}
