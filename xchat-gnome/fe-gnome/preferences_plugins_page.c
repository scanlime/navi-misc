/*
 * preferences_plugins_page.h - helpers for the plugins preferences page
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include "preferences_plugins_page.h"
#include "../common/xchat.h"
#define PLUGIN_C
typedef struct session xchat_context;
#include "../common/xchat-plugin.h"
#include "../common/plugin.h"
#include "../common/outbound.h"
#include "../common/util.h"

extern GSList *plugin_list;
extern XChatGUI gui;
static GSList *known_plugins;

static void
on_load_plugin_clicked (GtkButton *button, gpointer user_data);
static void
on_unload_plugin_clicked (GtkButton *button, gpointer user_data);
static void
xchat_gnome_plugin_add (char *filename);

void
initialize_preferences_plugins_page ()
{
	GtkWidget *treeview, *load, *unload;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *load_renderer;
	GtkTreeViewColumn *text_column, *load_column;
	GtkTreeSelection *select;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
  load = glade_xml_get_widget (gui.xml, "plugin load");
  unload = glade_xml_get_widget (gui.xml, "plugin unload");

  /* Plugin name, version, description, file, loaded (true or false) */
	store = gtk_list_store_new (5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));

	load_renderer = gtk_cell_renderer_toggle_new ();
	load_column = gtk_tree_view_column_new_with_attributes ("Loaded", load_renderer, "active", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), load_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("Plugin", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

  g_signal_connect (G_OBJECT(load), "clicked", G_CALLBACK (on_load_plugin_clicked), NULL);
  g_signal_connect (G_OBJECT(unload), "clicked", G_CALLBACK (on_unload_plugin_clicked), NULL);

	// FIXME: remove this for now
	//preferences_plugins_page_populate ();
}

void
preferences_plugins_page_populate()
{
	GtkWidget *treeview;
	GtkTreeIter iter;
	GtkListStore *store;
	GSList *list;
	xchat_plugin *plugin;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));

	gtk_list_store_clear (store);

  for_files (XCHATLIBDIR"/plugins", "*.so", xchat_gnome_plugin_add);
  for_files (XCHATLIBDIR"/plugins", "*.sl", xchat_gnome_plugin_add);

	list = known_plugins;
	while (list)
	{
		plugin = list->data;
		if (plugin->version[0] != 0)
		{
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter, 0, plugin->name, 1, plugin->version, 2, plugin->desc, 3, plugin->filename, -1);
		}
		list = list->next;
	}
}


static void
on_load_plugin_clicked (GtkButton *button, gpointer user_data)
{
  GtkWidget *treeview;
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  gchar *filename;

  treeview = glade_xml_get_widget (gui.xml, "plugins list");
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

  if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
    char *buf;

    gtk_tree_model_get (model, &iter, 3, &filename, -1);
    buf = malloc (strlen (filename) + 9);

    if (strchr (filename, ' '))
      snprintf (buf, strlen (filename) + 9, "LOAD \"%s\"", filename);
    else
      snprintf (buf, strlen (filename) + 9, "LOAD %s", filename);

    handle_command (gui.current_session, buf, FALSE);
    free (buf);
  }
}

static void
on_unload_plugin_clicked (GtkButton *button, gpointer user_data)
{
  GtkWidget *treeview;
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  gchar *filename;

  treeview = glade_xml_get_widget (gui.xml, "plugins list");
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

  if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
    char *buf;

    gtk_tree_model_get (model, &iter, 3, &filename, -1);
    buf = malloc (strlen (filename) + 10);

    if (strchr (filename, ' '))
      snprintf (buf, strlen (filename) + 10, "UNLOAD \"%s\"", filename);
    else
      snprintf (buf, strlen (filename) + 10, "UNLOAD %s", filename);

    handle_command (gui.current_session, buf, FALSE);
    free (buf);

  }
}

static void
xchat_gnome_plugin_add (char *filename)
{
	printf ("xchat_gnome_plugin_add: %s\n", filename);
}
