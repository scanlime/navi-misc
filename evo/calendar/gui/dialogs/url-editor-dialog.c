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
#include <e-util/e-dialog-widgets.h>
#include <e-util/e-passwords.h>
#include <e-util/e-icon-factory.h>

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

static gboolean
is_valid_url (const gchar *url)
{
	const gchar *p = url;

	if (strlen (url) == 0) {
		return FALSE;
	}
	while (*p) {
		if ((*p == '\\') || (*p == ' ')) {
			return FALSE;
		}
		p++;
	}
	return TRUE;
}

static void
fb_url_changed (GtkEntry *url_entry, UrlEditorDialog *dialog)
{
	const gchar *entry_contents;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gboolean valid;

	model = dialog->url_list_model;

	entry_contents = gtk_entry_get_text (url_entry);
	if (!is_valid_url (entry_contents)) {
		gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, FALSE);
		return;
	}
	/* check for duplicates */
	valid = gtk_tree_model_get_iter_first (model, &iter);
	while (valid) {
		gchar *url_name;
		gtk_tree_model_get (model, &iter, URL_LIST_LOCATION_COLUMN, &url_name, -1);
		if (!strcasecmp (url_name, entry_contents)) {
			gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, FALSE);
			return;
		}
		valid = gtk_tree_model_iter_next (model, &iter);
	}
	/* valid and unique */
	gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, TRUE);
	gtk_entry_set_activates_default (GTK_ENTRY (dialog->url_entry), TRUE);
}

static void
fb_url_activated (GtkEntry *url_entry, UrlEditorDialog *dialog)
{
	dialog->uri->location = g_strdup (gtk_entry_get_text (GTK_ENTRY (url_entry)));
}

static void
fb_ok_enable (GtkWidget *widget, UrlEditorDialog *dialog)
{
	gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, TRUE);
}

static void
fb_daily_toggled (GtkWidget *button, UrlEditorDialog *dialog)
{
	enum publish_frequency frequency;

	frequency = e_dialog_radio_get (dialog->daily, pub_frequency_type_map);
	dialog->uri->publish_freq = frequency;
	gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, TRUE);
}

static void
selection_changed (ESourceSelector *selector, UrlEditorDialog *dialog)
{
	GSList *selection = e_source_selector_get_selection (selector);

	if (selection != NULL) {
		GSList *p, *l = NULL;

		for (p = selection; p; p = g_slist_next (p)) {
			ESource *source = E_SOURCE (p->data);
			gchar *source_uid = g_strdup (e_source_peek_uid (source));
			l = g_slist_append (l, source_uid);
		}
		dialog->uri->calendars = l;
	}
	e_source_selector_free_selection (selection);
	gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, TRUE);
}

static void
init_widgets (UrlEditorDialog *dialog)
{
	GtkWidget *selector;
	ESourceList *source_list;
	GConfClient *gconf;
	GList *icon_list;
	GSList *p;

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

	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

	gtk_widget_ensure_style (dialog->url_editor);
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog->url_editor)->vbox), 0);
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog->url_editor)->action_area), 12);

	g_signal_connect (G_OBJECT (dialog->url_entry), "changed", G_CALLBACK (fb_url_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->username_entry), "changed", G_CALLBACK (fb_ok_enable), dialog);
	g_signal_connect (G_OBJECT (dialog->password_entry), "changed", G_CALLBACK (fb_ok_enable), dialog);
	g_signal_connect (G_OBJECT (dialog->remember_pw), "toggled", G_CALLBACK (fb_ok_enable), dialog);
	g_signal_connect (G_OBJECT (dialog->url_entry), "activate", G_CALLBACK (fb_url_activated), dialog);
	g_signal_connect (G_OBJECT (dialog->daily), "toggled", G_CALLBACK (fb_daily_toggled), dialog);
	g_signal_connect (G_OBJECT (dialog->weekly), "toggled", G_CALLBACK (fb_daily_toggled), dialog);
	g_signal_connect (G_OBJECT (dialog->user_publish), "toggled", G_CALLBACK (fb_daily_toggled), dialog);

	gconf = gconf_client_get_default ();
	source_list = e_source_list_new_for_gconf (gconf, "/apps/evolution/calendar/sources");

	if (dialog->uri->calendars) {
		ESource *source;

		selector = e_source_selector_new (source_list);

		for (p = dialog->uri->calendars; p; p = g_slist_next (p)) {
			gchar *source_uid = g_strdup (p->data);
			source = e_source_list_peek_source_by_uid (source_list, source_uid);
			e_source_selector_select_source ((ESourceSelector *)selector, source);
			g_free (source_uid);
		}
	} else {
		selector = e_source_selector_new (source_list);
	}
	e_source_selector_show_selection ((ESourceSelector *) selector, TRUE);
	g_signal_connect (G_OBJECT (selector), "selection_changed", G_CALLBACK (selection_changed), dialog);

	gtk_label_set_mnemonic_widget (GTK_LABEL (dialog->calendar_list_label), selector);
	gtk_widget_show (selector);
	gtk_container_add (GTK_CONTAINER (dialog->scrolled_window), selector);

	icon_list = e_icon_factory_get_icon_list ("stock_calendar");
	if (icon_list) {
		gtk_window_set_icon_list (GTK_WINDOW (dialog->url_editor), icon_list);
		g_list_foreach (icon_list, (GFunc) g_object_unref, NULL);
		g_list_free (icon_list);
	}
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
			gtk_entry_set_text (GTK_ENTRY (dialog->password_entry), uri->password);
			e_dialog_toggle_set (dialog->remember_pw, TRUE);
		} else {
			e_dialog_toggle_set (dialog->remember_pw, FALSE);
		}
	}

	switch (uri->publish_freq) {
		case URI_PUBLISH_DAILY:
			e_dialog_radio_set (dialog->daily, URI_PUBLISH_DAILY, pub_frequency_type_map);
			break;
		case URI_PUBLISH_WEEKLY:
			e_dialog_radio_set (dialog->daily, URI_PUBLISH_WEEKLY, pub_frequency_type_map);
			break;
		case URI_PUBLISH_USER:
		default:
			e_dialog_radio_set (dialog->daily, URI_PUBLISH_USER, pub_frequency_type_map);
	}

	gtk_dialog_set_response_sensitive ((GtkDialog *) dialog, GTK_RESPONSE_OK, FALSE);

	toplevel = glade_xml_get_widget (gui, "toplevel_dialog");
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), toplevel);
}

GtkWidget *
url_editor_dialog_new (GtkTreeModel *url_list_model, EPublishUri *pub_uri)
{
	UrlEditorDialog *dialog;

	dialog = (UrlEditorDialog *) g_object_new (url_editor_dialog_get_type (), NULL);

	dialog->uri = pub_uri;
	dialog->url_list_model = url_list_model;

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

void
url_editor_dialog_run (UrlEditorDialog *dialog)
{
	gint r;

	g_return_if_fail (dialog != NULL);

	r = gtk_dialog_run (GTK_DIALOG (dialog));
	if (r == GTK_RESPONSE_OK) {
		fb_url_activated (dialog->url_entry, dialog);
		dialog->uri->username = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->username_entry)));
		dialog->uri->password = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->password_entry)));
		if (e_dialog_toggle_get (dialog->remember_pw)) {
			e_passwords_add_password (dialog->uri->location, dialog->uri->password);
			e_passwords_remember_password ("Calendar", dialog->uri->location);
		} else {
			e_passwords_forget_password ("Calendar", dialog->uri->location);
		}
	}

	gtk_widget_hide_all (dialog);
}
