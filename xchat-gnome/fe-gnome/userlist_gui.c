/*
 * userlist_gui.c - helpers for the userlist view
 *
 * Copyright (C) 2004 xchat-gnome team
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

#include <gnome.h>
#include "userlist_gui.h"
#include "pixmaps.h"
#include "textgui.h"
#include "palette.h"
#include "../common/xchat.h"
#include "../common/userlist.h"

gboolean userlist_click (GtkWidget *view, GdkEventButton *event, gpointer data);
void userlist_context (GtkWidget *treeview, struct User *user);

/* action callbacks */

static void user_send_file_activate (GtkAction *action, gpointer data);
static void user_open_dialog_activate (GtkAction *action, gpointer data);
static void user_kick_activate (GtkAction *action, gpointer data);
static void user_ban_activate (GtkAction *action, gpointer data);

static GtkActionEntry popup_action_entries [] = {
	{ "UserlistSendFile", NULL, N_("_Send File"), "", NULL, G_CALLBACK (user_send_file_activate) },
	{ "UserlistOpenDialog", NULL, N_("Open _Dialog"), "", NULL, G_CALLBACK (user_open_dialog_activate) },
	{ "UserlistKick", NULL, N_("_Kick"), "", NULL, G_CALLBACK (user_kick_activate) },
	{ "UserlistBan", NULL, N_("_Ban"), "", NULL, G_CALLBACK (user_ban_activate) }
};

GtkTooltips *tooltips;

void
initialize_userlist ()
{
	GtkWidget *userlist_view;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	userlist_view = glade_xml_get_widget (gui.xml, "userlist");

	icon_renderer = gtk_cell_renderer_pixbuf_new ();
	icon_column = gtk_tree_view_column_new_with_attributes ("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (userlist_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("name", text_renderer, "text", 1, "foreground-gdk", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (userlist_view), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (userlist_view));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	tooltips = gtk_tooltips_new ();

	g_signal_connect (G_OBJECT (userlist_view), "button_press_event", G_CALLBACK (userlist_click), NULL);

	gtk_action_group_add_actions (gui.action_group, popup_action_entries, G_N_ELEMENTS (popup_action_entries), NULL);
}

struct User*
userlist_get_selected ()
{
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct User *u;

	treeview = glade_xml_get_widget (gui.xml, "userlist");
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 2, &u, -1);
		return u;
	}
	return NULL;
}

gboolean
userlist_click (GtkWidget *view, GdkEventButton *event, gpointer data)
{
	GtkTreePath *path;
	GtkTreeSelection *select;
	if (!event)
		return FALSE;

	if (event->type == GDK_2BUTTON_PRESS)
	{
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (view), event->x, event->y, &path, 0, 0, 0))
		{
			g_print ("double click!\n");
	    		gtk_tooltips_set_tip (GTK_TOOLTIPS (tooltips), view, "hi", "woof");
			gtk_tooltips_enable (GTK_TOOLTIPS (tooltips));
			return TRUE;
		}
	}

	if (event->button == 3)
	{
		if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (view), event->x, event->y, &path, 0, 0, 0))
		{
			select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
			gtk_tree_selection_unselect_all (select);
			gtk_tree_selection_select_path (select, path);
			gtk_tree_path_free (path);
		}
		struct User *u = userlist_get_selected ();
		if (u != NULL)
			userlist_context (view, u);
		return TRUE;
	}
	return FALSE;
}

void
userlist_context (GtkWidget *treeview, struct User *user)
{
	GtkWidget *menu;

	menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");
	g_return_if_fail (menu != NULL);

	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 2, gtk_get_current_event_time ());
}

static void user_send_file_activate (GtkAction *action, gpointer data)
{
	/* FIXME implement me! */
}

static void user_open_dialog_activate (GtkAction *action, gpointer data)
{
	/* FIXME implement me! */
}

static void user_kick_activate (GtkAction *action, gpointer data)
{
	/* FIXME implement me! */
}

static void user_ban_activate (GtkAction *action, gpointer data)
{
	/* FIXME implement me! */
}
