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

static void
publish_events_toggled (GtkToggleButton *button, UrlEditorDialog2 *dialog)
{
	gtk_widget_set_sensitive (dialog->events_selector, gtk_toggle_button_get_active (button));
}

static void
publish_tasks_toggled (GtkToggleButton *button, UrlEditorDialog2 *dialog)
{
	gtk_widget_set_sensitive (dialog->tasks_selector, gtk_toggle_button_get_active (button));
}

static gboolean
url_editor_dialog_construct2 (UrlEditorDialog2 *dialog)
{
	GladeXML *gui;
	GtkWidget *toplevel;
	ESourceList *source_list;
	GConfClient *gconf;
	GtkSizeGroup *group;

	gconf = gconf_client_get_default ();

	gui = glade_xml_new (PLUGINDIR "/publish-calendar.glade", "publishing toplevel", NULL);
	dialog->gui = gui;

#define GW(name) ((dialog->name) = glade_xml_get_widget (dialog->gui, #name))
	GW(url_entry);
	GW(publish_frequency);

	GW(type_selector);
	GW(publish_events);
	GW(events_swin);
	GW(publish_tasks);
	GW(tasks_swin);

	GW(username_entry);
	GW(password_entry);
	GW(remember_pw);
#undef GW

	g_return_val_if_fail (gui != NULL, FALSE);

	toplevel = glade_xml_get_widget (gui, "publishing toplevel");
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), toplevel);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);

	dialog->cancel = gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	dialog->ok = gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

	source_list = e_source_list_new_for_gconf (gconf, "/apps/evolution/calendar/sources");
	dialog->events_selector = e_source_selector_new (source_list);
	gtk_widget_show (dialog->events_selector);
	gtk_container_add (GTK_CONTAINER (dialog->events_swin), dialog->events_selector);

	source_list = e_source_list_new_for_gconf (gconf, "/apps/evolution/tasks/sources");
	dialog->tasks_selector = e_source_selector_new (source_list);
	gtk_widget_show (dialog->tasks_selector);
	gtk_container_add (GTK_CONTAINER (dialog->tasks_swin), dialog->tasks_selector);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, dialog->url_entry);
	gtk_size_group_add_widget (group, dialog->publish_frequency);
	gtk_size_group_add_widget (group, dialog->type_selector);
	gtk_size_group_add_widget (group, dialog->username_entry);
	gtk_size_group_add_widget (group, dialog->password_entry);
	g_object_unref (group);

	gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->publish_frequency), 0);
	gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->type_selector), 0);

	g_signal_connect (G_OBJECT (dialog->publish_events), "toggled", G_CALLBACK (publish_events_toggled), dialog);
	g_signal_connect (G_OBJECT (dialog->publish_tasks), "toggled", G_CALLBACK (publish_tasks_toggled), dialog);

	g_object_unref (gconf);

	return TRUE;
}

GtkWidget *
url_editor_dialog_new2 (GtkTreeModel *url_list_model)
{
	UrlEditorDialog2 *dialog;

	dialog = (UrlEditorDialog2 *) g_object_new (URL_EDITOR_DIALOG_TYPE, NULL);
	dialog->url_list_model = g_object_ref (url_list_model);

	if (url_editor_dialog_construct2 (dialog))
		return GTK_WIDGET (dialog);

	g_object_unref (dialog);
	return NULL;
}

static void
url_editor_dialog_dispose2 (GObject *obj)
{
	UrlEditorDialog2 *dialog = (UrlEditorDialog2 *) obj;

	if (dialog->url_list_model) {
		g_object_unref (dialog->url_list_model);
		dialog->url_list_model = NULL;
	}

	((GObjectClass *)(parent_class))->dispose (obj);
}

static void
url_editor_dialog_class_init2 (UrlEditorDialogClass2 *klass)
{
	GObjectClass *object_class;

	object_class = (GObjectClass *) klass;
	parent_class = g_type_class_ref (GTK_TYPE_DIALOG);

	object_class->dispose = url_editor_dialog_dispose2;
}

static void
url_editor_dialog_init2 (UrlEditorDialog2 *dialog)
{
}

GType
url_editor_dialog_get_type2 (void)
{
	static GType type = 0;

	if (!type) {
		static GTypeInfo info = {
			sizeof (UrlEditorDialogClass2),
			NULL, NULL,
			(GClassInitFunc) url_editor_dialog_class_init2,
			NULL, NULL,
			sizeof (UrlEditorDialog2),
			0,
			(GInstanceInitFunc) url_editor_dialog_init2,
		};

		type = g_type_register_static (GTK_TYPE_DIALOG, "UrlEditorDialog2", &info, 0);
	}

	return type;
}

void
url_editor_dialog_run2 (UrlEditorDialog2 *dialog)
{
	gtk_dialog_run (GTK_DIALOG (dialog));
	/* FIXME */
}
