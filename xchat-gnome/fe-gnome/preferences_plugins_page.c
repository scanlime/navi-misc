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

extern GSList *plugin_list; // xchat's list of loaded plugins.
extern XChatGUI gui;
static GSList *known_plugins; // Our own list of all the known plugins, loaded or not.
															// Known being any .so or .sl file located in either
															// XCHATDIR/plugins or ~/.xchat2/plugins.

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

/* FIXME: As far as I can tell this function is getting called atleast 3 times at the
 * start of the application, which seems kinda dumb. That means that just at the start
 * of the application we fill and clear the list 3 times... I'm 99% sure this is coming
 * from fe-gnome.c
 */
void
preferences_plugins_page_populate()
{
	GtkWidget *treeview;
	GtkTreeIter iter;
	GtkListStore *store;
	GSList *list;
	xchat_plugin *plugin;
	gchar *homedir, *xchatdir;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));

	gtk_list_store_clear (store);

	/* Fun little string things that ultimately become the path to ~/.xchat2/plugins.
	 * FIXME: It might behoove us to store the expanded path string to ~/.xchat2 somewhere
	 * more permanent...
	 */
	homedir = g_get_home_dir();
	xchatdir = malloc (strlen (homedir) + strlen (".xchat2/plugins") + 1);
	sprintf (xchatdir, "%s/.xchat2/plugins", homedir);

	/* Create a list of all the plugins in our known directories. */
  for_files (XCHATLIBDIR"/plugins", "*.so", xchat_gnome_plugin_add);
  for_files (XCHATLIBDIR"/plugins", "*.sl", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.so", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.sl", xchat_gnome_plugin_add);

	/* Put our fun, happy plugins of joy into the great list store of pluginny goodness.
	 * FIXME: It would be a good idea to use xchat's list of loaded plugins to toggle the
	 * loaded thingy here.
	 */
	list = known_plugins;
	while (list)
	{
		plugin = list->data;
    printf ("%s %s\n", plugin->filename, plugin->name);
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

	/* Ooooh... something's highlighted... */
  if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
    char *buf;

		/* Yay! We got a filename! */
    gtk_tree_model_get (model, &iter, 3, &filename, -1);
    buf = malloc (strlen (filename) + 9);

		/* Use the /LOAD command to load the plugin, rather than rewriting all that code
		 * ourselves, aren't we clever.
		 */
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

		/* With the exception of the larger buffer size and the addition of the letters
		 * U and N here, this function is the same as the one just above it. I'm sure
		 * you can figure it out.
		 */
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
	/* FIXME: Look, Ma!! No code!!! */
}
