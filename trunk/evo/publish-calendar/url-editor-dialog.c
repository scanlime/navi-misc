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
#include <e-util/e-passwords.h>
#include <string.h>

static GtkDialogClass *parent_class = NULL;

static void
check_input (UrlEditorDialog2 *dialog)
{
	gint n = 0;
	GSList *sources;

	if (strlen (gtk_entry_get_text (GTK_ENTRY (dialog->url_entry))) == 0)
		goto fail;
	if (GTK_WIDGET_IS_SENSITIVE (dialog->events_selector)) {
		sources = e_source_selector_get_selection (E_SOURCE_SELECTOR (dialog->events_selector));
		n += g_slist_length (sources);
	}
	if (GTK_WIDGET_IS_SENSITIVE (dialog->tasks_selector)) {
		sources = e_source_selector_get_selection (E_SOURCE_SELECTOR (dialog->tasks_selector));
		n += g_slist_length (sources);
	}
	if (n == 0)
		goto fail;

	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, TRUE);
	return;
fail:
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);
}

static void
url_entry_changed (GtkEntry *entry, UrlEditorDialog2 *dialog)
{
	check_input (dialog);
}

static void
source_selection_changed (ESourceSelector *selector, UrlEditorDialog2 *dialog)
{
	check_input (dialog);
}

static void
type_selector_changed (GtkComboBox *combo, UrlEditorDialog2 *dialog)
{
	gint selected = gtk_combo_box_get_active (combo);

	if (selected == URI_PUBLISH_AS_FB) {
		gtk_widget_set_sensitive (dialog->events_selector, TRUE);
		gtk_widget_set_sensitive (dialog->tasks_selector, FALSE);
	} else {
		gtk_widget_set_sensitive (dialog->events_selector, TRUE);
		gtk_widget_set_sensitive (dialog->tasks_selector, TRUE);
	}
}

static gboolean
url_editor_dialog_construct2 (UrlEditorDialog2 *dialog)
{
	GladeXML *gui;
	GtkWidget *toplevel;
	GConfClient *gconf;
	GtkSizeGroup *group;
	EPublishUri *uri;

	gconf = gconf_client_get_default ();

	gui = glade_xml_new (PLUGINDIR "/publish-calendar.glade", "publishing toplevel", NULL);
	dialog->gui = gui;

#define GW(name) ((dialog->name) = glade_xml_get_widget (dialog->gui, #name))
	GW(url_entry);
	GW(publish_frequency);

	GW(type_selector);
	GW(events_swin);
	GW(tasks_swin);

	GW(username_entry);
	GW(password_entry);
	GW(remember_pw);
#undef GW

	g_return_val_if_fail (gui != NULL, FALSE);

	uri = dialog->uri;

	toplevel = glade_xml_get_widget (gui, "publishing toplevel");
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), toplevel);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);

	dialog->cancel = gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	dialog->ok = gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_OK, FALSE);

	dialog->events_source_list = e_source_list_new_for_gconf (gconf, "/apps/evolution/calendar/sources");
	dialog->events_selector = e_source_selector_new (dialog->events_source_list);
	gtk_widget_show (dialog->events_selector);
	gtk_container_add (GTK_CONTAINER (dialog->events_swin), dialog->events_selector);

	dialog->tasks_source_list = e_source_list_new_for_gconf (gconf, "/apps/evolution/tasks/sources");
	dialog->tasks_selector = e_source_selector_new (dialog->tasks_source_list);
	gtk_widget_show (dialog->tasks_selector);
	gtk_container_add (GTK_CONTAINER (dialog->tasks_swin), dialog->tasks_selector);

	group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
	gtk_size_group_add_widget (group, dialog->url_entry);
	gtk_size_group_add_widget (group, dialog->publish_frequency);
	gtk_size_group_add_widget (group, dialog->type_selector);
	gtk_size_group_add_widget (group, dialog->username_entry);
	gtk_size_group_add_widget (group, dialog->password_entry);
	g_object_unref (group);

	if (uri == NULL) {
		gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->publish_frequency), 0);
		gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->type_selector), 0);

		dialog->uri = g_new0 (EPublishUri, 1);
		uri = dialog->uri;
	} else {
		ESource *source;
		GSList *p;

		for (p = uri->events; p; p = g_slist_next (p)) {
			gchar *source_uid = g_strdup (p->data);
			source = e_source_list_peek_source_by_uid (dialog->events_source_list, source_uid);
			e_source_selector_select_source ((ESourceSelector *) dialog->events_selector, source);
			g_free (source_uid);
		}
		for (p = uri->events; p; p = g_slist_next (p)) {
			gchar *source_uid = g_strdup (p->data);
			source = e_source_list_peek_source_by_uid (dialog->tasks_source_list, source_uid);
			e_source_selector_select_source ((ESourceSelector *) dialog->tasks_selector, source);
			g_free (source_uid);
		}

		if (uri->location && strlen (uri->location)) {
			gtk_entry_set_text (GTK_ENTRY (dialog->url_entry), uri->location);
			if (uri->username && strlen (uri->username))
				gtk_entry_set_text (GTK_ENTRY (dialog->username_entry), uri->username);
		}
		gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->publish_frequency), uri->publish_frequency);
		gtk_combo_box_set_active (GTK_COMBO_BOX (dialog->type_selector), uri->publish_format);

		uri->password = e_passwords_get_password ("Calendar", uri->location);
		if (uri->password) {
			if (strlen (uri->password) != 0) {
				gtk_entry_set_text (GTK_ENTRY (dialog->password_entry), uri->password);
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog->remember_pw), TRUE);
			} else {
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dialog->remember_pw), FALSE);
			}
		}
	}

	g_signal_connect (G_OBJECT (dialog->url_entry), "changed", G_CALLBACK (url_entry_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->type_selector), "changed", G_CALLBACK (type_selector_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->events_selector), "selection_changed", G_CALLBACK (source_selection_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->tasks_selector), "selection_changed", G_CALLBACK (source_selection_changed), dialog);

	g_object_unref (gconf);

	return TRUE;
}

GtkWidget *
url_editor_dialog_new2 (GtkTreeModel *url_list_model, EPublishUri *uri)
{
	UrlEditorDialog2 *dialog;

	dialog = (UrlEditorDialog2 *) g_object_new (URL_EDITOR_DIALOG_TYPE, NULL);
	dialog->url_list_model = g_object_ref (url_list_model);
	dialog->uri = uri;

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
	gint response;

	response = gtk_dialog_run (GTK_DIALOG (dialog));
	if (response == GTK_RESPONSE_OK) {
		if (dialog->uri->location)
			g_free (dialog->uri->location);
		if (dialog->uri->username)
			g_free (dialog->uri->username);
		if (dialog->uri->password)
			g_free (dialog->uri->password);

		dialog->uri->location = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->url_entry)));
		dialog->uri->username = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->username_entry)));
		dialog->uri->password = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->password_entry)));

		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog->remember_pw))) {
			e_passwords_add_password (dialog->uri->location, dialog->uri->password);
			e_passwords_remember_password ("Calendar", dialog->uri->location);
		} else {
			e_passwords_forget_password ("Calendar", dialog->uri->location);
		}
	}
	gtk_widget_hide_all (GTK_WIDGET (dialog));
}
