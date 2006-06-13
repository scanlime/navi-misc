/*
 * util.h: Helper functions for miscellaneous tasks
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

#include "gui.h"

#ifndef XCHAT_GNOME_ERROR_DIALOG_H
#define XCHAT_GNOME_ERROR_DIALOG_H

void error_dialog (const gchar *header, const gchar *message);
gint gtk_tree_iter_sort_func_nocase (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data);
gboolean dialog_escape_key_handler_destroy (GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean dialog_escape_key_handler_hide (GtkWidget *widget, GdkEventKey *event, gpointer data);
void menu_position_under_widget (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data);
void menu_position_under_tree_view (GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer user_data);

#endif
