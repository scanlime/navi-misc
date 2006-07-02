/*
 * irc-network-editor.h - GtkDialog subclass for editing an IrcNetwork
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

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include <gtk/gtk.h>
#include "irc-network-editor.h"
#include "util.h"

static GtkDialogClass *parent_class;

static gboolean check_input   (IrcNetworkEditor *editor);
static void     apply_changes (IrcNetworkEditor *editor);

static void
irc_network_editor_dispose (GObject *object)
{
	IrcNetworkEditor *e = (IrcNetworkEditor *) object;

	if (e->gconf) {
		g_object_unref (e->gconf);
		e->gconf = NULL;
	}
	if (e->network) {
		g_object_unref (e->network);
		e->network = NULL;
	}

	((GObjectClass *) parent_class)->dispose (object);
}

static void
irc_network_editor_response (GtkDialog *dialog, gint response)
{
	IrcNetworkEditor *editor = IRC_NETWORK_EDITOR (dialog);
	if (response == GTK_RESPONSE_OK) {
		if (check_input (editor)) {
			apply_changes (editor);
			gtk_widget_destroy (GTK_WIDGET (editor));
		}
	} else {
		gtk_widget_destroy (GTK_WIDGET (editor));
	}
}

static void
irc_network_editor_class_init (IrcNetworkEditorClass *klass)
{
	GObjectClass   *object_class;
	GtkDialogClass *dialog_class;

	object_class = (GObjectClass *) klass;
	object_class->dispose = irc_network_editor_dispose;

	dialog_class = (GtkDialogClass *) klass;
	dialog_class->response = irc_network_editor_response;
}

static void
use_globals_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gchar *text;

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/nickname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->nickname), text);
	g_free (text);

	text = gconf_client_get_string (e->gconf, "/apps/xchat/irc/realname", NULL);
	gtk_entry_set_text (GTK_ENTRY (e->realname), text);
	g_free (text);

	gtk_widget_set_sensitive (e->custom_box, FALSE);
}

static void
use_custom_set (GtkRadioButton *button, IrcNetworkEditor *e)
{
	gtk_widget_set_sensitive (e->custom_box, TRUE);
}

static void
add_server_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeIter iter;
	GtkTreePath *path;

	gtk_list_store_append (e->server_store, &iter);
	path = gtk_tree_model_get_path (GTK_TREE_MODEL (e->server_store), &iter);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (e->servers), path, e->server_column, TRUE);
	gtk_tree_path_free (path);
}

static void
edit_server_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->servers));
	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (e->server_store), &iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (e->servers), path, e->server_column, TRUE);
		gtk_tree_path_free (path);
	}
}

static void
remove_server_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->servers));
	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
		gtk_list_store_remove (e->server_store, &iter);
}

static void
server_selection_changed (GtkTreeSelection *selection, IrcNetworkEditor *e)
{
	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (e->edit_server, TRUE);
		gtk_widget_set_sensitive (e->remove_server, TRUE);
	} else {
		gtk_widget_set_sensitive (e->edit_server, FALSE);
		gtk_widget_set_sensitive (e->remove_server, FALSE);
	}
}

static void
server_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *newtext, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->servers));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		if (strlen (newtext))
			gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, newtext, -1);
		else
			gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
add_autojoin_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeIter iter;
	GtkTreePath *path;

	gtk_list_store_append (e->autojoin_store, &iter);
	path = gtk_tree_model_get_path (GTK_TREE_MODEL (e->autojoin_store), &iter);
	gtk_tree_view_set_cursor (GTK_TREE_VIEW (e->autojoin_channels), path, e->autojoin_column, TRUE);
	gtk_tree_path_free (path);
}

static void
edit_autojoin_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->autojoin_channels));
	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (e->autojoin_store), &iter);
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (e->autojoin_channels), path, e->autojoin_column, TRUE);
		gtk_tree_path_free (path);
	}
}

static void
remove_autojoin_clicked (GtkButton *button, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->autojoin_channels));
	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
		gtk_list_store_remove (e->autojoin_store, &iter);
}

static void
autojoin_selection_changed (GtkTreeSelection *selection, IrcNetworkEditor *e)
{
	if (gtk_tree_selection_get_selected (selection, NULL, NULL)) {
		gtk_widget_set_sensitive (e->edit_autojoin, TRUE);
		gtk_widget_set_sensitive (e->remove_autojoin, TRUE);
	} else {
		gtk_widget_set_sensitive (e->edit_autojoin, FALSE);
		gtk_widget_set_sensitive (e->remove_autojoin, FALSE);
	}
}

static void
autojoin_edited (GtkCellRendererText *renderer, gchar *arg1, gchar *newtext, IrcNetworkEditor *e)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (e->autojoin_channels));
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		if (strlen (newtext))
			gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, newtext, -1);
		else
			gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
irc_network_editor_init (IrcNetworkEditor *dialog)
{
	gchar **enc;
	GtkTreeSelection *server_selection, *autojoin_selection;
	GladeXML *xml;

	dialog->gconf = NULL;
	dialog->network = NULL;
	dialog->toplevel = NULL;

	xml = NULL;
	if (g_file_test ("../../data/irc-network-editor.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("../../data/irc-network-editor.glade", "toplevel", NULL);
	if (!xml)
		xml = glade_xml_new (XCHATSHAREDIR "/irc-network-editor.glade", "toplevel", NULL);
	if (!xml)
		return;

#define GW(name) ((dialog->name) = glade_xml_get_widget (xml, #name))
	GW(network_settings_table);

	GW(network_name);

	GW(autoconnect);
	GW(use_ssl);
	GW(allow_invalid);
	GW(cycle);

	GW(nickserv_password);
	GW(password);

	GW(servers);
	GW(add_server);
	GW(edit_server);
	GW(remove_server);

	GW(use_globals);
	GW(use_custom);
	GW(custom_box);
	GW(nickname);
	GW(realname);

	GW(autojoin_channels);
	GW(add_autojoin);
	GW(edit_autojoin);
	GW(remove_autojoin);

	GW(toplevel);
#undef GW

	g_object_unref (xml);

	dialog->server_store = gtk_list_store_new (1, G_TYPE_STRING);
	dialog->autojoin_store = gtk_list_store_new (1, G_TYPE_STRING);
	dialog->server_renderer = gtk_cell_renderer_text_new ();
	dialog->autojoin_renderer = gtk_cell_renderer_text_new ();

	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->servers), GTK_TREE_MODEL (dialog->server_store));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (dialog->servers), 0, _("Server"), dialog->server_renderer, "text", 0, NULL);
	g_object_set (G_OBJECT (dialog->server_renderer), "editable", TRUE, NULL);
	dialog->server_column = gtk_tree_view_get_column (GTK_TREE_VIEW (dialog->servers), 0);

	gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->autojoin_channels), GTK_TREE_MODEL (dialog->autojoin_store));
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (dialog->autojoin_channels), 0, _("Channel"), dialog->autojoin_renderer, "text", 0, NULL);
	g_object_set (G_OBJECT (dialog->autojoin_renderer), "editable", TRUE, NULL);
	dialog->autojoin_column = gtk_tree_view_get_column (GTK_TREE_VIEW (dialog->autojoin_channels), 0);

	dialog->encoding = gtk_combo_box_new_text ();
	gtk_widget_show (dialog->encoding);
	gtk_table_attach_defaults (GTK_TABLE (dialog->network_settings_table), dialog->encoding, 1, 2, 5, 6);

	enc = (gchar **) encodings;
	do {
		gtk_combo_box_append_text (GTK_COMBO_BOX (dialog->encoding), _(*enc));
		enc++;
	} while (*enc);

	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_OK,     GTK_RESPONSE_OK);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), dialog->toplevel);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

	g_signal_connect (G_OBJECT (dialog->use_globals),       "toggled", G_CALLBACK (use_globals_set),            dialog);
	g_signal_connect (G_OBJECT (dialog->use_custom),        "toggled", G_CALLBACK (use_custom_set),             dialog);

	server_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dialog->servers));
	g_signal_connect (G_OBJECT (dialog->add_server),        "clicked", G_CALLBACK (add_server_clicked),         dialog);
	g_signal_connect (G_OBJECT (dialog->edit_server),       "clicked", G_CALLBACK (edit_server_clicked),        dialog);
	g_signal_connect (G_OBJECT (dialog->remove_server),     "clicked", G_CALLBACK (remove_server_clicked),      dialog);
	g_signal_connect (G_OBJECT (server_selection),          "changed", G_CALLBACK (server_selection_changed),   dialog);
	g_signal_connect (G_OBJECT (dialog->server_renderer),   "edited",  G_CALLBACK (server_edited),              dialog);

	autojoin_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dialog->autojoin_channels));
	g_signal_connect (G_OBJECT (dialog->add_autojoin),      "clicked", G_CALLBACK (add_autojoin_clicked),       dialog);
	g_signal_connect (G_OBJECT (dialog->edit_autojoin),     "clicked", G_CALLBACK (edit_autojoin_clicked),      dialog);
	g_signal_connect (G_OBJECT (dialog->remove_autojoin),   "clicked", G_CALLBACK (remove_autojoin_clicked),    dialog);
	g_signal_connect (G_OBJECT (autojoin_selection),        "changed", G_CALLBACK (autojoin_selection_changed), dialog);
	g_signal_connect (G_OBJECT (dialog->autojoin_renderer), "edited",  G_CALLBACK (autojoin_edited),            dialog);
}

GType
irc_network_editor_get_type (void)
{
	static GType irc_network_editor_type = 0;
	if (!irc_network_editor_type) {
		static const GTypeInfo irc_network_editor_info = {
			sizeof (IrcNetworkEditorClass),
			NULL, NULL,
			(GClassInitFunc) irc_network_editor_class_init,
			NULL, NULL,
			sizeof (IrcNetworkEditor),
			0,
			(GInstanceInitFunc) irc_network_editor_init,
		};

		irc_network_editor_type = g_type_register_static (GTK_TYPE_DIALOG, "IrcNetworkEditor", &irc_network_editor_info, 0);

		parent_class = g_type_class_ref (GTK_TYPE_DIALOG);
	}

	return irc_network_editor_type;
}

static void
populate_server_list (IrcNetworkEditor *e)
{
	GSList *s;
	GtkTreeIter iter;
	ircserver *serv;

	for (s = e->network->servers; s; s = g_slist_next (s)) {
		serv = s->data;

		gtk_list_store_append (e->server_store, &iter);
		gtk_list_store_set (e->server_store, &iter, 0, serv->hostname, -1);
	}
}

static void
populate_autojoin_list (IrcNetworkEditor *e)
{
	GtkTreeIter iter;
	gchar **channels;
	gint i;

	if (!e->network->autojoin)
		return;

	channels = g_strsplit (e->network->autojoin, ",", 0);
	for (i = 0; channels[i]; i++) {
		gtk_list_store_append (e->autojoin_store, &iter);
		gtk_list_store_set (e->autojoin_store, &iter, 0, channels[i], -1);
	}

	g_strfreev (channels);
}

static void
irc_network_editor_populate (IrcNetworkEditor *e)
{
	gchar *title;

	e->gconf = gconf_client_get_default ();

	if (!e->network->net) {
		/* We're creating a new network. Don't populate things from the structure */
		gtk_window_set_title (GTK_WINDOW (e), _("New Network"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_globals), TRUE);
		use_globals_set (GTK_RADIO_BUTTON (e->use_globals), e);
		gtk_combo_box_set_active (GTK_COMBO_BOX (e->encoding), 0);
		gtk_widget_grab_focus (e->network_name);
		return;
	}

	title = g_strdup_printf (_("%s Network Properties"), e->network->name);
	gtk_window_set_title (GTK_WINDOW (e), title);
	g_free (title);

	gtk_entry_set_text (GTK_ENTRY (e->network_name), e->network->name);

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->autoconnect),      e->network->autoconnect);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_ssl),          e->network->use_ssl);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->allow_invalid),    e->network->allow_invalid);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->cycle),            e->network->cycle);

	if (e->network->use_global) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_globals), TRUE);
		use_globals_set (GTK_RADIO_BUTTON (e->use_globals), e);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (e->use_custom), TRUE);
		use_custom_set (GTK_RADIO_BUTTON (e->use_custom), e);

		gtk_entry_set_text (GTK_ENTRY (e->nickname), e->network->nick);
		gtk_entry_set_text (GTK_ENTRY (e->realname), e->network->real);
	}

	gtk_widget_set_sensitive (e->edit_server, FALSE);
	gtk_widget_set_sensitive (e->remove_server, FALSE);
	gtk_widget_set_sensitive (e->edit_autojoin, FALSE);
	gtk_widget_set_sensitive (e->remove_autojoin, FALSE);

	if (e->network->password)
		gtk_entry_set_text (GTK_ENTRY (e->password), e->network->password);
	if (e->network->nickserv_password)
		gtk_entry_set_text (GTK_ENTRY (e->nickserv_password), e->network->nickserv_password);

	gtk_combo_box_set_active (GTK_COMBO_BOX (e->encoding), e->network->encoding);

	populate_server_list (e);
	populate_autojoin_list (e);
}

IrcNetworkEditor *
irc_network_editor_new (IrcNetwork *network)
{
	IrcNetworkEditor *e = g_object_new (irc_network_editor_get_type (), 0);
	if (e->toplevel == NULL) {
		g_object_unref (e);
		return NULL;
	}

	e->network = g_object_ref (network);
	irc_network_editor_populate (e);

	return e;
}

static void
apply_changes (IrcNetworkEditor *e)
{
	IrcNetwork *net;
	GSList *s;
	GtkTreeIter iter;
	gchar *t1 = NULL, *t2, *t3;

	net = e->network;

	if (net->name)     g_free (net->name);
	if (net->password) g_free (net->password);
	if (net->nick)     g_free (net->nick);
	if (net->real)     g_free (net->real);
	if (net->autojoin) g_free (net->autojoin);
	if (net->nickserv_password) g_free (net->nickserv_password);

	net->name     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->network_name)));
	net->password = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->password)));
	net->nick     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->nickname)));
	net->real     = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->realname)));
	net->nickserv_password = g_strdup (gtk_entry_get_text (GTK_ENTRY (e->nickserv_password)));

	net->autoconnect        = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->autoconnect));
	net->use_ssl            = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->use_ssl));
	net->allow_invalid      = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->allow_invalid));
	net->cycle              = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->cycle));
	net->use_global         = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (e->use_globals));
	net->encoding           = gtk_combo_box_get_active (GTK_COMBO_BOX (e->encoding));

	for (s = net->servers; s; s = g_slist_next (s)) {
		ircserver *serv = s->data;
		g_free (serv->hostname);
		g_free (serv);
	}
	g_slist_free (net->servers);
	net->servers = NULL;
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (e->server_store), &iter)) {
		do {
			char *text;
			ircserver *serv = g_new0 (ircserver, 1);
			gtk_tree_model_get (GTK_TREE_MODEL (e->server_store), &iter, 0, &text, -1);
			serv->hostname = g_strdup (text);
			net->servers = g_slist_append (net->servers, serv);
		} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (e->server_store), &iter));
	}

	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (e->autojoin_store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (e->autojoin_store), &iter, 0, &t1, -1);
		t1 = g_strdup (t1);
		while (gtk_tree_model_iter_next (GTK_TREE_MODEL (e->autojoin_store), &iter)) {
			gtk_tree_model_get (GTK_TREE_MODEL (e->autojoin_store), &iter, 0, &t2, -1);
			t3 = g_strdup_printf ("%s,%s", t1, t2);
			g_free (t1);
			t1 = t3;
		}
	}
	net->autojoin = t1;

	irc_network_save (net);
}

static gboolean
check_input (IrcNetworkEditor *editor)
{
	GtkTreeIter iter;

	if (!strlen (gtk_entry_get_text (GTK_ENTRY (editor->network_name)))) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK (editor->toplevel), 0);
		gtk_widget_grab_focus (editor->network_name);
		error_dialog (_("Invalid input"), _("You must enter a network name"));
		return FALSE;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (editor->use_custom))) {
		if (!strlen (gtk_entry_get_text (GTK_ENTRY (editor->nickname)))) {
			gtk_notebook_set_current_page (GTK_NOTEBOOK (editor->toplevel), 1);
			gtk_widget_grab_focus (editor->nickname);
			error_dialog (_("Invalid input"), _("You must enter a nick name"));
			return FALSE;
		}
		if (!strlen (gtk_entry_get_text (GTK_ENTRY (editor->realname)))) {
			gtk_notebook_set_current_page (GTK_NOTEBOOK (editor->toplevel), 1);
			gtk_widget_grab_focus (editor->realname);
			error_dialog (_("Invalid input"), _("You must enter a real name"));
			return FALSE;
		}
	}
	if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (editor->server_store), &iter)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK (editor->toplevel), 2);
		gtk_widget_grab_focus (editor->add_server);
		error_dialog (_("No Servers"), _("You must add at least one server for this network"));
		return FALSE;
	}
	return TRUE;
}

void
irc_network_editor_run (IrcNetworkEditor *editor)
{
	gtk_window_present (GTK_WINDOW (editor));
}
