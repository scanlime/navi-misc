/*
 * preferences_plugins_page.h - helpers for the plugins preferences page
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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

#include <gconf/gconf-client.h>

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

static GSList *enabled_plugins;
static GConfClient *gconf_client;

/* Callbacks */
static void on_load_toggled (GtkCellRendererToggle *toggle, gchar *arg, gpointer user_data);
static void on_open_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_remove_plugin_clicked (GtkButton *button, gpointer user_data);
static void on_selection_changed (GtkTreeSelection *selection, gpointer user_data);
static void on_plugin_filechooser (GtkDialog *dialog, gint response, gpointer user_data);
static void on_row_activated (GtkTreeView *treeview, GtkTreePath *path,
		GtkTreeViewColumn *col, gpointer user_data);

/* Helpers */
static void xchat_gnome_plugin_add (char *filename);
static gboolean set_loaded_if_match (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter,
		gpointer data);
static void load_unload (char *filename, gboolean loaded, GtkTreeModel *model,
		GtkTreeIter iter);
static void autoload_plugin_cb (gpointer data, gpointer user_data);

static GtkWidget *file_selector; // because everyone needs a file selec-tor!

void
initialize_preferences_plugins_page ()
{
	GtkWidget *treeview, *open, *remove;
	GtkListStore *store;
	GtkCellRenderer *text_renderer, *load_renderer;
	GtkTreeViewColumn *text_column, *load_column;
	GtkTreeSelection *select;
	const gchar *homedir;
	gchar *xchatdir;
	GError *err = NULL;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	open = glade_xml_get_widget (gui.xml, "plugin open");
	remove = glade_xml_get_widget (gui.xml, "plugin remove");

  /* Plugin name, version, description, file, loaded (true or false), file handle */
	store = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));

	load_renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (G_OBJECT (load_renderer), "activatable", TRUE, NULL);
	load_column = gtk_tree_view_column_new_with_attributes (_("Enable"), load_renderer, "active", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), load_column);
	text_renderer = gtk_cell_renderer_text_new ();
	text_column = gtk_tree_view_column_new_with_attributes (_("Plugin"), text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (load_renderer), "toggled", G_CALLBACK (on_load_toggled), NULL);
	g_signal_connect (G_OBJECT (open), "clicked", G_CALLBACK (on_open_plugin_clicked), NULL);
	g_signal_connect (G_OBJECT (remove), "clicked", G_CALLBACK (on_remove_plugin_clicked), NULL);
	g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (on_selection_changed), NULL);
	g_signal_connect (G_OBJECT (treeview), "row-activated", G_CALLBACK (on_row_activated), NULL);

	file_selector = gtk_file_chooser_dialog_new (_("Open Plugin"),
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
	xchatdir = g_strdup_printf ("%s/.xchat2/plugins", homedir);

	/* Create a list of all the plugins in our known directories. */
	for_files (XCHATLIBDIR"/plugins", "*.so", xchat_gnome_plugin_add);
	for_files (XCHATLIBDIR"/plugins", "*.sl", xchat_gnome_plugin_add);
	for_files (XCHATLIBDIR"/plugins", "*.py", xchat_gnome_plugin_add);
	for_files (XCHATLIBDIR"/plugins", "*.pl", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.so", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.sl", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.py", xchat_gnome_plugin_add);
	for_files (xchatdir, "*.pl", xchat_gnome_plugin_add);

	g_free (xchatdir);

	/* Get our GConf stuff. */
	gconf_client = gconf_client_get_default ();
	enabled_plugins = gconf_client_get_list (gconf_client, "/apps/xchat/plugins/enabled",
			GCONF_VALUE_STRING, &err);
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
	while (list) {
		plugin = list->data;
		if (plugin->version[0] != 0) {
			gtk_tree_model_foreach (model, set_loaded_if_match, plugin->filename);
		}
		list = list->next;
	}
}

void
autoload_plugins ()
{
	g_slist_foreach (enabled_plugins, &autoload_plugin_cb, NULL);
}

static void
on_load_toggled (GtkCellRendererToggle *toggle, gchar *arg, gpointer user_data)
{
	GtkWidget *treeview;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	gchar *filename;
	gboolean loaded;

	treeview = glade_xml_get_widget (gui.xml, "plugins list");
	model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		//char *buf;
		gtk_tree_model_get (model, &iter, 3, &filename, 4, &loaded, -1);
		/* Some of this code was taken from the fe-gtk plugingui,
		 * the names have been changed to protect the innocent.
		 */
		load_unload (filename, loaded, model, iter);
	}
}
static void
on_open_plugin_clicked (GtkButton *button, gpointer user_data)
{
	const gchar *homedir;
	gchar *plugindir;

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

	button = GTK_BUTTON (glade_xml_get_widget (gui.xml, "plugin remove"));
	gtk_widget_set_sensitive (GTK_WIDGET (button), sensitive);
}

static void
on_plugin_filechooser (GtkDialog *dialog, gint response, gpointer user_data)
{
	char *filename;

	if (response == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_selector));
		xchat_gnome_plugin_add (filename);
	}

	gtk_widget_hide (file_selector);
}

static void
on_row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	gboolean loaded;
	char *file;

	model = gtk_tree_view_get_model (treeview);
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 3, &file, 4, &loaded, -1);
		gtk_list_store_set (GTK_LIST_STORE(model), &iter, 4, !loaded, -1);
		load_unload (file, loaded, model, iter);
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

	gtk_list_store_append (store, &iter);

	/* FIXME: The problem with all of this is that it doesn't do any checking to make
	 * sure the file is even a valid type. Should add some stuff to maybe check the
	 * extension?
	 */
	if (handle != NULL && g_module_symbol (handle, "xchat_plugin_get_info", (gpointer *) (&info_func))) {
		/* Create a new plugin instance and add it to our list of known plugins. */
		((xchat_plugin_get_info*)info_func) (&name, &desc, &version);
	} else {
		/* In the event that this foolish plugin has no get_info function we'll just use
		 * the file name. */
		name = rindex (filename, '/') + 1;
		version = _("unknown");
		desc = _("unkown");
	}

	gtk_list_store_set (store, &iter, 0, name, 1, version, 2, desc, 3, filename, 5, handle, -1);
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

gint
filename_test (gconstpointer a, gconstpointer b)
{
	return strcmp ((char*)a, (char*)b);
}

static void
load_unload (char *filename, gboolean loaded, GtkTreeModel *model, GtkTreeIter iter)
{
	char *buf;
	GError *err = NULL;

	if (loaded) {
		GSList *removed_plugin;
		buf = malloc (strlen (filename) + 10);
		if (strchr (filename, ' '))
			sprintf (buf, "UNLOAD \"%s\"", filename);
		else
			sprintf (buf, "UNLOAD %s", filename);

		/* FIXME: Bad to assume that the plugin was successfully unloaded. */
		gtk_list_store_set (GTK_LIST_STORE (model), &iter, 4, FALSE, -1);

		/* For some reason the normal find and remove functions don't work with
		 * strings. I suspect they're comparing the pointers instead of the strings
		 * themselves. So we just define our own little custom test that uses
		 * strcmp to remove the plugin filename from the list.
		 */
		if ((removed_plugin = g_slist_find_custom (enabled_plugins, filename, &filename_test)) != NULL) {
			enabled_plugins = g_slist_delete_link (enabled_plugins, removed_plugin);
		}

	} else {
		buf = malloc (strlen (filename) + 9);
		if (strchr (filename, ' '))
			sprintf (buf, "LOAD \"%s\"", filename);
		else
			sprintf (buf, "LOAD %s", filename);

		enabled_plugins = g_slist_append (enabled_plugins, filename);
	}

	handle_command (gui.current_session, buf, FALSE);
	/* Update the gconf stuffs. Good gods Anderson... What are you
	 * babbling about?
	 */
	gconf_client_set_list (gconf_client, "/apps/xchat/plugins/enabled",
			GCONF_VALUE_STRING, enabled_plugins, &err);
	free (buf);
}

static void
autoload_plugin_cb (gpointer data, gpointer user_data)
{
	char *filename;
	filename = (char*) data;

	/* We can't use handle_command to run LOAD <filename> here; not sure
	 * why. But loading the plugin with plugin_load seems to work just
	 * fine.
	 */
	plugin_load (gui.current_session, filename, NULL);
}
