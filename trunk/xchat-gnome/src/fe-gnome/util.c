/*
 * util.c: Helper functions for miscellaneous tasks
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
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "util.h"
#include "gui.h"
#include "../common/xchatc.h"

void
error_dialog (const gchar *header, const gchar *message)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (GTK_WINDOW (gui.main_window),
					 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					 GTK_MESSAGE_ERROR,
					 GTK_BUTTONS_OK,
					 "%s", header);

	gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog), "%s", message);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

gboolean
dialog_escape_key_handler_destroy (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval == GDK_Escape) {
		g_signal_stop_emission_by_name (widget, "key-press-event");
		gtk_widget_destroy (widget);
		return TRUE;
	}

	return FALSE;
}

gboolean
dialog_escape_key_handler_hide (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval == GDK_Escape) {
		g_signal_stop_emission_by_name (widget, "key-press-event");
		gtk_widget_hide (widget);
		return TRUE;
	}

	return FALSE;
}

/* Code taken from gedit (gedit/gedit-utils.c)
 * These functions are GtkMenuPositionFunc callbacks. */
void
menu_position_under_widget (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data)
{
	GtkWidget *w = GTK_WIDGET (user_data);
	GtkRequisition requisition;

	gdk_window_get_origin (w->window, x, y);
	gtk_widget_size_request (GTK_WIDGET (menu), &requisition);

	if (gtk_widget_get_direction (w) == GTK_TEXT_DIR_RTL) {
		*x += w->allocation.x + w->allocation.width - requisition.width;
	} else {
		*x += w->allocation.x;
	}

	*y += w->allocation.y + w->allocation.height;

	*push_in = TRUE;
}

void
menu_position_under_tree_view (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data)
{
	GtkTreeView *tree = GTK_TREE_VIEW (user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model (tree);
	g_return_if_fail (model != NULL);

	selection = gtk_tree_view_get_selection (tree);
	g_return_if_fail (selection != NULL);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		GtkTreePath *path;
		GdkRectangle rect;

		gdk_window_get_origin (GTK_WIDGET (tree)->window, x, y);

		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_get_cell_area (tree, path,
					     gtk_tree_view_get_column (tree, 0),
					     &rect);
		gtk_tree_path_free (path);

		*x += rect.x;
		*y += rect.y + rect.height;

		if (gtk_widget_get_direction (GTK_WIDGET (tree)) == GTK_TEXT_DIR_RTL) {
			GtkRequisition requisition;
			gtk_widget_size_request (GTK_WIDGET (menu), &requisition);
			*x += rect.width - requisition.width;
		}
	} else {
		/* no selection -> regular "under widget" positioning */
		menu_position_under_widget (menu, x, y, push_in, tree);
	}
}

gchar *
locate_data_file (const gchar *file_name)
{
	gchar *uninstalled_path, *path;

	uninstalled_path = g_build_filename (TOPSRCDIR, "data", file_name, NULL);
	if (g_file_test (uninstalled_path, G_FILE_TEST_EXISTS)) {
		path = uninstalled_path;
	} else {
		g_free (uninstalled_path);
		path = gnome_program_locate_file (gnome_program_get (), GNOME_FILE_DOMAIN_APP_DATADIR, file_name, FALSE, NULL);
	}

	g_return_val_if_fail (path != NULL, NULL);

	return path;
}

server *
find_connected_server (ircnet *network)
{
	for (GSList *i = sess_list; i; i = g_slist_next (i)) {
		session *sess = (session *)(i->data);
		if (sess->server->network == network) {
			return sess->server;
		}
	}
	return NULL;
}
