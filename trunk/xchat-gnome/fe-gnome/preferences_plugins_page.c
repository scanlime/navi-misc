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

typedef int (xchat_init_func) (xchat_plugin *, char **, char **, char **, char *);
typedef int (xchat_deinit_func) (xchat_plugin *);
typedef void (xchat_plugin_get_info) (char **, char **, char **);

extern GSList *plugin_list; // xchat's list of loaded plugins.
extern XChatGUI gui;

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
	gchar *homedir, *xchatdir;

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
	GtkListStore *store;
	GSList *list;
	xchat_plugin *plugin;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));

	/* Put our fun, happy plugins of joy into the great list store of pluginny goodness.
	 * starting with the list of plugins we keep and then the list of plugins loaded by
	 * the xchat core in its infinite wisdom. While we do the loaded plugins we'll add
	 * them to our list of known plugins.
	 */
	list = plugin_list;
	while (list)
	{
		plugin = list->data;
		if (plugin->version[0] != 0)
		{
    	printf ("%s %s\n", plugin->filename, plugin->name);
			/* FIXME: Code to toggle loaded thingy goes here */
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
	GtkWidget *treeview;
	GtkListStore *store;
	GtkTreeIter iter;

	void *handle;
	xchat_plugin_get_info *info_func;
	char *name, *desc, *version;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	
	/* For now we are just assuming it's ok to use gmodule, if this is a mistake
	 * we can change this to match the common plugins stuff so that we use
	 * gmodule if it's available, otherwise we'll do something else.
	 */
	handle = g_module_open (filename, 0);

	if (handle == NULL) {
		printf ("Null handle in plugin_add\n");
		return;
	}

	if (g_module_symbol (handle, "xchat_plugin_get_info", (gpointer *)&info_func)) {
		/* Create a new plugin instance and add it to our list of known plugins. */
		((xchat_plugin_get_info*)info_func) (&name, &desc, &version);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, name, 1, version, 2, desc, 3, filename, -1);
	}
	else
		printf ("%s: No xchat_plugin_info symbol\n", filename);
}
