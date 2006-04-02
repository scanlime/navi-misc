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

void
error_dialog (const gchar *header, const gchar *message)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (GTK_WINDOW (gui.main_window),
					 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					 GTK_MESSAGE_ERROR,
					 GTK_BUTTONS_OK,
					 "%s", header);

	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", message);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

gint gtk_tree_iter_sort_func_nocase (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data)
{
	gchar *as, *bs;
	gint result;

	gtk_tree_model_get (model, a, 1, &as, -1);
	gtk_tree_model_get (model, b, 1, &bs, -1);

	if (as == NULL) return 1;
	if (bs == NULL) {
		g_free (as);
		return -1;
	}

	result = strcasecmp (as, bs);

	g_free (as);
	g_free (bs);

	/* GtkTreeSortable has undefined results if this function isn't
	 * reflexive, antisymmetric and transitive.  If the two strings are
	 * equal, compare session pointers */
	if (result == 0) {
		gpointer ap, bp;
		gtk_tree_model_get (model, a, 2, &ap, -1);
		gtk_tree_model_get (model, b, 2, &bp, -1);

		return (GPOINTER_TO_UINT(ap) - GPOINTER_TO_UINT(bp));
	}

	return result;
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

