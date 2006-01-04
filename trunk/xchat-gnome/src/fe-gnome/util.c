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
#include <string.h>
#include "util.h"

void
error_dialog (const gchar *header, const gchar *message)
{
	GtkWidget *dialog, *label;
	gchar *text;

	dialog = glade_xml_get_widget (gui.xml, "error dialog");
	label = glade_xml_get_widget (gui.xml, "error label");
	text = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s", header, message);
	gtk_label_set_markup (GTK_LABEL (label), text);
	g_free (text);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_hide (dialog);
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
