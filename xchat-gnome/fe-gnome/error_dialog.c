/*
 * error_dialog.c: Simple helper for showing error dialogs
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

#include "error_dialog.h"

void
error_dialog (const gchar *header, const gchar *message)
{
	GtkWidget *dialog, *label;
	dialog = glade_xml_get_widget (gui.xml, "error dialog");
	label = glade_xml_get_widget (gui.xml, "error label");
	gchar *text = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s", header, message);
	gtk_label_set_text (GTK_LABEL (label), text);
	g_free (text);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_hide (dialog);
}
