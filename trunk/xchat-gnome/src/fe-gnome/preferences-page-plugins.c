/*
 * preferences-page-plugins.c - helpers for the plugins preferences page
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
#include <gconf/gconf-client.h>
#include <string.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-tree-view.h>
#endif

#include "preferences-page-plugins.h"
#include "preferences-dialog.h"
#include "plugins.h"
#include "util.h"
#include "../common/xchat.h"
#define PLUGIN_C
typedef struct session xchat_context;
#include "../common/xchat-plugin.h"
#include "../common/plugin.h"
#include "../common/util.h"

typedef int (xchat_init_func) (xchat_plugin *, char **, char **, char **, char *);
typedef int (xchat_deinit_func) (xchat_plugin *);
typedef void (xchat_plugin_get_info) (char **, char **, char **, char **);

extern GSList *plugin_list; 	// xchat's list of loaded plugins.
extern GSList *enabled_plugins;	// Our list of loaded plugins.
extern XChatGUI gui;

static PreferencesPagePlugins *pageref;
#ifdef HAVE_LIBSEXY
static GtkWidget* get_plugin_tooltip (SexyTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, PreferencesPagePlugins *page);
#endif

enum
{
	COL_NAME,
	COL_VERSION,
	COL_DESC,
	COL_FILENAME,
	COL_LOADED,
	COL_DISPLAY
};

static GObjectClass *parent_class;

static void
fe_plugin_add (char *filename)
{
	GtkTreeIter iter;

	void *handle;
	gpointer info_func;
	char *name, *desc, *version, *display;

	handle = g_module_open (filename, 0);

	gtk_list_store_append (pageref->plugin_store, &iter);

	/* FIXME: The problem with all of this is that it doesn't do any checking to make
	 * sure the file is even a valid type. Should add some stuff to maybe check the
	 * extension?
	 */
	if (handle != NULL && g_module_symbol (handle, "xchat_plugin_get_info", &info_func)) {
		/* Create a new plugin instance and add it to our list of known plugins. */
		/* FIXME: zed added a 'reserved' field, but i'm not sure what it is */
		((xchat_plugin_get_info*) info_func) (&name, &desc, &version, NULL);
	} else {
		/* In the event that this foolish plugin has no get_info function we'll just use
		 * the file name.
		 */
		name = strrchr (filename, '/') + 1;
		version = _("unknown");
		desc = _("unknown");
	}

	display = g_strdup_printf ("<b>%s</b>\n%s", name, desc);
	gtk_list_store_set (pageref->plugin_store, &iter, COL_NAME, name, COL_VERSION, version,
			    COL_DESC, desc, COL_FILENAME, filename, COL_DISPLAY, display, -1);
	g_free (display);

	if (handle != NULL) {
		g_module_close (handle);
	}
}

static gint
filename_test (gconstpointer a, gconstpointer b)
{
	return strcmp ((char*)a, (char*)b);
}

static void
load_unload (char *filename, gboolean loaded, PreferencesPagePlugins *page, GtkTreeIter iter)
{
	GConfClient *client;

	/* FIXME: Now that we're handling these things in such a way that
	 * we check for successful a load/unload we should maybe do something
	 * with any errors we receive.
	 */
	if (loaded) {
		/* Unload the plugin. */
		GSList *removed_plugin;
		int err = unload_plugin (filename);

		if (err == 1) {
			gtk_list_store_set (page->plugin_store, &iter, COL_LOADED, FALSE, -1);

			if ((removed_plugin = g_slist_find_custom (enabled_plugins, filename, &filename_test)) != NULL) {
				enabled_plugins = g_slist_delete_link (enabled_plugins, removed_plugin);
			}

		} else {
			gchar *errmsg = g_strdup_printf (_("An error occurred unloading %s"), filename);
			error_dialog (_("Plugin Unload Failed"), errmsg);
		}

	} else {
		/* Load the plugin. */
		gchar *err = load_plugin (gui.current_session, filename, NULL, TRUE, FALSE);
		if (err == NULL) {
			gtk_list_store_set (page->plugin_store, &iter, COL_LOADED, TRUE, -1);
			enabled_plugins = g_slist_append (enabled_plugins, filename);
		} else {
			error_dialog (_("Plugin Load Failed"), err);
		}
	}

	/* Update the enabled gconf key. */
	client = gconf_client_get_default ();
	gconf_client_set_list (client, "/apps/xchat/plugins/loaded", GCONF_VALUE_STRING, enabled_plugins, NULL);
	g_object_unref (client);
}

static void
load_toggled (GtkCellRendererToggle *toggle, gchar *pathstr, PreferencesPagePlugins *page)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	gchar *filename;
	gboolean loaded;

	path = gtk_tree_path_new_from_string (pathstr);
	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (page->plugin_store), &iter, path)) {
		gtk_tree_model_get (GTK_TREE_MODEL (page->plugin_store), &iter, COL_FILENAME, &filename,
		                    COL_LOADED, &loaded, -1);


		load_unload (filename, loaded, page, iter);
	}

	gtk_tree_path_free (path);
}

static void
open_plugin_clicked (GtkButton *button, PreferencesPagePlugins *page)
{
	GtkWidget *file_selector;
	const gchar *homedir;
	gchar *plugindir;
	gchar *filename;
	gint response;

	file_selector = gtk_file_chooser_dialog_new (_("Open Plugin"), NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	homedir = g_get_home_dir();
	plugindir = g_strdup_printf ("%s/.xchat2/plugins", homedir);
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_selector), plugindir);
	g_free (plugindir);

	response = gtk_dialog_run (GTK_DIALOG (file_selector));

	if (response == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_selector));
		pageref = page;
		fe_plugin_add (filename);
	}
	gtk_widget_destroy (file_selector);
}

static void
remove_plugin_clicked (GtkButton *button, PreferencesPagePlugins *page)
{
	GtkTreeSelection *select;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean loaded;
	gchar *filename;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->plugins_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, COL_FILENAME, &filename, COL_LOADED, &loaded, -1);
		if (loaded)
			unload_plugin (filename);
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	}
}

static void
selection_changed (GtkTreeSelection *select, PreferencesPagePlugins *page)
{
	gtk_widget_set_sensitive (page->plugins_remove, gtk_tree_selection_get_selected (select, NULL, NULL));
}

static void
row_activated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, PreferencesPagePlugins *page)
{
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *filename;
	gboolean loaded;

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->plugins_list));
	if (gtk_tree_selection_get_selected (select, &model, &iter)) {
		gtk_tree_model_get (model, &iter, COL_FILENAME, &filename, COL_LOADED, &loaded, -1);
		/* Apparently setting the loaded field in the list store no longer causes
		 * the toggle signal to be emitted. So we explicitly call load_unload
		 * here.
		 */
		load_unload (filename, loaded, page, iter);
	}
}

static gboolean
set_loaded_if_match (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	char *filename;

	gtk_tree_model_get (model, iter, COL_FILENAME, &filename, -1);

	if (strcmp ((char*)data, filename) == 0) {
		gtk_list_store_set (GTK_LIST_STORE (model), iter, COL_LOADED, TRUE, -1);
		return TRUE;
	}

	return FALSE;
}

#ifdef HAVE_LIBSEXY
static GtkWidget*
get_plugin_tooltip (SexyTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *column, PreferencesPagePlugins *page)
{
	GtkTreeIter iter;
	GtkWidget *label;
	gchar *text;

	label = NULL;

	if (gtk_tree_model_get_iter (GTK_TREE_MODEL (page->plugin_store), &iter, path)) {
		gtk_tree_model_get (GTK_TREE_MODEL (page->plugin_store), &iter, COL_DESC, &text, -1);
		label = gtk_label_new (text);
		g_free (text);
		gtk_widget_show_all (label);
	}

	return label;
}
#endif

PreferencesPagePlugins*
preferences_page_plugins_new (gpointer prefs_dialog, GladeXML *xml)
{
	PreferencesPagePlugins *page = g_object_new (preferences_page_plugins_get_type (), NULL);
	PreferencesDialog *p = (PreferencesDialog *) prefs_dialog;
	GtkTreeIter iter;
	GtkTreeSelection *select;
	GtkWidget *scroll;
	GSList *list;
	xchat_plugin *plugin;

	const gchar *homedir;
	gchar *xchatdir, *path;

#define GW(name) ((page->name) = glade_xml_get_widget (xml, #name))
	GW(plugins_open);
	GW(plugins_remove);
#undef GW

#ifdef HAVE_LIBSEXY
	page->plugins_list = sexy_tree_view_new ();
#else
	page->plugins_list = gtk_tree_view_new ();
#endif
	scroll = glade_xml_get_widget (xml, "scrolledwindow19");
	gtk_container_add (GTK_CONTAINER (scroll), page->plugins_list);

	gtk_widget_show (page->plugins_list);

	path = locate_data_file ("plugin-manager.png");
	page->icon = gdk_pixbuf_new_from_file (path, NULL);
	g_free (path);

	gtk_list_store_append (p->page_store, &iter);
	gtk_list_store_set (p->page_store, &iter, 0, page->icon, 1, _("Scripts and Plugins"), 2, 5, -1);

	/*                                          name,          version,       description,   file,          loaded*/
	page->plugin_store = gtk_list_store_new (6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
	                                         G_TYPE_BOOLEAN, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (page->plugins_list), GTK_TREE_MODEL (page->plugin_store));

	page->load_renderer = gtk_cell_renderer_toggle_new ();
	page->text_renderer = gtk_cell_renderer_text_new ();

	g_object_set (G_OBJECT (page->text_renderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
	g_object_set (G_OBJECT (page->load_renderer), "activatable", TRUE, NULL);

	page->load_column = gtk_tree_view_column_new_with_attributes (_("Enable"), page->load_renderer, "active",
	                                                              COL_LOADED, NULL);
	page->text_column = gtk_tree_view_column_new_with_attributes (_("Plugin"), page->text_renderer, "markup",
	                                                              COL_DISPLAY, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (page->plugins_list), page->load_column);
	gtk_tree_view_append_column (GTK_TREE_VIEW (page->plugins_list), page->text_column);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->plugins_list));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT (page->load_renderer),  "toggled",       G_CALLBACK (load_toggled),          page);
	g_signal_connect (G_OBJECT (page->plugins_open),   "clicked",       G_CALLBACK (open_plugin_clicked),   page);
	g_signal_connect (G_OBJECT (page->plugins_remove), "clicked",       G_CALLBACK (remove_plugin_clicked), page);
	g_signal_connect (G_OBJECT (select),               "changed",       G_CALLBACK (selection_changed),     page);
	g_signal_connect (G_OBJECT (page->plugins_list),   "row-activated", G_CALLBACK (row_activated),         page);

#ifdef HAVE_LIBSEXY
	g_signal_connect (G_OBJECT (page->plugins_list),   "get-tooltip",   G_CALLBACK (get_plugin_tooltip),    page);
#endif

	homedir = g_get_home_dir ();
	xchatdir = g_strdup_printf ("%s/.xchat2/plugins", homedir);
	pageref = page;
	for_files (XCHATLIBDIR "/plugins", "*.so", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.sl", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.py", fe_plugin_add);
	for_files (XCHATLIBDIR "/plugins", "*.pl", fe_plugin_add);
	for_files (xchatdir, "*.so", fe_plugin_add);
	for_files (xchatdir, "*.sl", fe_plugin_add);
	for_files (xchatdir, "*.py", fe_plugin_add);
	for_files (xchatdir, "*.pl", fe_plugin_add);
	g_free (xchatdir);

	/* Put our fun, happy plugins of joy into the great list store of pluginny goodness.
	 * Starting with the list of plugins we keep and then the list of plugins loaded by
	 * the xchat core in its infinite wisdom. While we do the loaded plugins we'll add
	 * them to our list of known plugins.
	 */
	list = plugin_list;
	while (list) {
		plugin = list->data;
		if (plugin->version[0] != 0) {
			gtk_tree_model_foreach (GTK_TREE_MODEL (page->plugin_store), set_loaded_if_match, plugin->filename);
		}
		list = list->next;
	}

	return page;
}

static void
preferences_page_plugins_dispose (GObject *object)
{
	PreferencesPagePlugins *page = (PreferencesPagePlugins *) object;

	if (page->icon) {
		g_object_unref (page->icon);
		page->icon = NULL;
	}

	if (page->plugin_store) {
		g_object_unref (page->plugin_store);
		page->plugin_store = NULL;
	}

	parent_class->dispose (object);
}

static void
preferences_page_plugins_class_init (PreferencesPagePluginsClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	object_class->dispose = preferences_page_plugins_dispose;
}

GType
preferences_page_plugins_get_type (void)
{
	static GType preferences_page_plugins_type = 0;
	if (G_UNLIKELY (preferences_page_plugins_type == 0)) {
		static const GTypeInfo preferences_page_plugins_info = {
			sizeof (PreferencesPagePluginsClass),
			NULL, NULL,
			(GClassInitFunc) preferences_page_plugins_class_init,
			NULL, NULL,
			sizeof (PreferencesPagePlugins),
			0,
			NULL,
		};
		preferences_page_plugins_type = g_type_register_static (G_TYPE_OBJECT, "PreferencesPagePlugins", &preferences_page_plugins_info, 0);
	}

	return preferences_page_plugins_type;
}
