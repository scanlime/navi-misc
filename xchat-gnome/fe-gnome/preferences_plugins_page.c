/*
 * preferences_plugins_page.h - helpers for the plugins preferences page
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

/* Callbacks */
static void on_load_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_unload_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_open_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_remove_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_selection_changed (GtkTreeSelection *selection, gpointer user_data);
static void on_plugin_filechooser (GtkDialog *dialog, gint response, gpointer user_data);

/* Helpers */
static void xchat_gnome_plugin_add (char *filename);
static gboolean set_loaded_if_match (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data);

static GtkWidget *file_selector; // because everyone needs a file selec-tor!

void
initialize_preferences_plugins_page ()
{
	GtkWidget *treeview, *load, *unload, *open, *remove;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *load_renderer;
	GtkTreeViewColumn *text_column, *load_column;
	GtkTreeSelection *select;
	gchar *homedir, *xchatdir;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
  load = glade_xml_get_widget (gui.xml, "plugin load");
  unload = glade_xml_get_widget (gui.xml, "plugin unload");
	open = glade_xml_get_widget (gui.xml, "plugin open");
	remove = glade_xml_get_widget (gui.xml, "plugin remove");

  /* Plugin name, version, description, file, loaded (true or false) */
	store = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));

	load_renderer = gtk_cell_renderer_toggle_new ();
	load_column = gtk_tree_view_column_new_with_attributes ("Loaded", load_renderer, "active", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), load_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes ("Plugin", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

  g_signal_connect (G_OBJECT (load), "clicked", G_CALLBACK (on_load_plugin_clicked), NULL);
  g_signal_connect (G_OBJECT (unload), "clicked", G_CALLBACK (on_unload_plugin_clicked), NULL);
  g_signal_connect (G_OBJECT (open), "clicked", G_CALLBACK (on_open_plugin_clicked), NULL);
  g_signal_connect (G_OBJECT (remove), "clicked", G_CALLBACK (on_remove_plugin_clicked), NULL);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (on_selection_changed), NULL);

	file_selector = gtk_file_chooser_dialog_new ("Open Plugin",
			GTK_WINDOW (glade_xml_get_widget (gui.xml, "preferences")),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
	g_signal_connect (G_OBJECT (file_selector), "response", G_CALLBACK (on_plugin_filechooser), NULL);

	/* Fun little string things that ultimately become the path to ~/.xchat2/plugins.
	 * FIXME: It might behoove us to store the expanded path string to ~/.xchat2 somewhere
	 * more permanent...
	 */
	homedir = g_get_home_dir();
	xchatdir = malloc (strlen (homedir) + strlen ("/.xchat2/plugins") + 1);
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
preferences_plugins_page_update()
{
	GtkWidget *treeview;
	GtkTreeModel *model;
	GSList *list;
	xchat_plugin *plugin;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));

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
			gtk_tree_model_foreach (model, set_loaded_if_match, plugin->filename);
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
		/* Yay! We got a filename! */
    gtk_tree_model_get (model, &iter, 3, &filename, -1);

		/* Ya know what's more clever than using /LOAD to load the plugin?
		 * using the plugin_load function provided by the core... way to go
		 * evan, you dumbass.
		 */
		plugin_load (gui.current_session, filename, NULL);
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
    gtk_tree_model_get (model, &iter, 3, &filename, -1);
		if (plugin_kill (filename, TRUE) == 1)
			gtk_list_store_set (GTK_LIST_STORE (model), &iter, 4, FALSE, -1);
  }
}

static void
on_open_plugin_clicked (GtkButton *button, gpointer user_data)
{
	char *homedir, *plugindir;

	homedir = g_get_home_dir();
	plugindir = malloc (strlen (homedir) + strlen ("/.xchat2/plugins") + 1);
	sprintf (plugindir, "%s/.xchat2/plugins", homedir);

	gtk_file_chooser_set_filename( GTK_FILE_CHOOSER (file_selector), plugindir);
	gtk_widget_show (file_selector);
}

static void
on_remove_plugin_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *treeview;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	gboolean loaded;
	char *filename;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 3, &filename, 4, &loaded, -1);
		/* If the plugin is loaded unload it. */
		if (loaded)
			plugin_kill (filename, TRUE);
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
on_selection_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkButton *button;
	gboolean sensitive = FALSE;

	if (gtk_tree_selection_get_selected (selection, NULL, NULL))
		sensitive = TRUE;

	button = GTK_BUTTON (glade_xml_get_widget (gui.xml, "plugin load"));
	gtk_widget_set_sensitive (GTK_WIDGET (button), sensitive);
	button = GTK_BUTTON (glade_xml_get_widget (gui.xml, "plugin unload"));
	gtk_widget_set_sensitive (GTK_WIDGET (button), sensitive);
	button = GTK_BUTTON (glade_xml_get_widget (gui.xml, "plugin remove"));
	gtk_widget_set_sensitive (GTK_WIDGET (button), sensitive);
}

static void on_plugin_filechooser (GtkDialog *dialog, gint response, gpointer user_data)
{
	char *filename;

	if (response == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_selector));
		xchat_gnome_plugin_add (filename);
	}

	gtk_widget_hide (file_selector);
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

	gtk_list_store_append (store, &iter);

	if (g_module_symbol (handle, "xchat_plugin_get_info", (gpointer *)&info_func)) {
		/* Create a new plugin instance and add it to our list of known plugins. */
		((xchat_plugin_get_info*)info_func) (&name, &desc, &version);
		gtk_list_store_set (store, &iter, 0, name, 1, version, 2, desc, 3, filename, -1);
	}
	/* In the event that this foolish plugin has no get_info function we'll just use
	 * the file name.
	 */
	else {
		name = rindex (filename, '/') + 1;
		version = "unknown";
		desc = "unkown";
		gtk_list_store_set (store, &iter, 0, name, 1, version, 2, desc, 3, filename, 5, handle, -1);
	}
}

static gboolean
set_loaded_if_match (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	char *filename;

	gtk_tree_model_get (model, iter, 3, &filename, -1);

	if (strcmp ((char*)data, filename) == 0) {
		gtk_list_store_set (GTK_LIST_STORE (model), iter, 4, TRUE, -1);
		return TRUE;
	}

	return FALSE;
}
