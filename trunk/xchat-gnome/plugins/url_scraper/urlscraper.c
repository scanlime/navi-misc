#include <sys/types.h>
#include <regex.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <libgnome/gnome-url.h>

#include "xchat-plugin.h"

#define VERSION "0.2"
#define MAXURLS 10

#define URLREGEX "(ht|f)tps?://[~a-z0-9./_-]+[a-z0-9]+"
#define EMAILREGEX "[a-z0-9.+_-]+@([0-9a-z-]+\\.)+[a-z]+"

static xchat_plugin *ph;	// Plugin handle.
static regex_t *email;		// Regex that matches e-mail addresses.
static regex_t *url;		// Regex that matches urls.
static int urls;			// Current total in the scraper.

static GtkWidget *window;
static GtkListStore *list_store;

void url_open (GtkTreeView *treeview, GtkTreePath *path,
		GtkTreeViewColumn *column, gpointer user_data);


static gboolean delete_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	return TRUE;
}

static void make_window ()
{
	GtkWidget *treeview, *scrolled;
	GtkTreeViewColumn *nick_col, *chan_col, *url_col;
	GtkCellRenderer *nick_rend, *chan_rend, *url_rend;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW(window), 400, 400);
	g_signal_connect (G_OBJECT(window), "delete-event", G_CALLBACK(delete_cb), 0);

	list_store = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	scrolled = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	treeview = gtk_tree_view_new ();

	gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(list_store));

	nick_rend = gtk_cell_renderer_text_new ();
	nick_col = gtk_tree_view_column_new_with_attributes ("Nick", nick_rend, "text", 0, NULL);

	chan_rend = gtk_cell_renderer_text_new ();
	chan_col = gtk_tree_view_column_new_with_attributes ("Channel", chan_rend, "text", 1, NULL);

	url_rend = gtk_cell_renderer_text_new ();
	url_col = gtk_tree_view_column_new_with_attributes ("URL", url_rend, "text", 2, NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), nick_col);
	gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), chan_col);
	gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), url_col);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(treeview), TRUE);

	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(scrolled), treeview);

	gtk_container_add (GTK_CONTAINER(window), scrolled);

	g_signal_connect (G_OBJECT(treeview), "row-activated", G_CALLBACK(url_open), NULL);

	gtk_widget_show_all (window);
}

static void add_match (char **word, regmatch_t match)
{
	int len;
	const char *chan;
	char *url_match, *channel;
	GtkTreeIter iter;

	len = match.rm_eo - match.rm_so;

	url_match = malloc (len + 1);
	memset (url_match, 0, len + 1);
	strncpy (url_match, word[2] + match.rm_so, len);

	chan = xchat_get_info (ph, "channel");
	channel = malloc (strlen (chan));
	strncpy (channel, chan+1, strlen (chan));

	/*if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(list_store), &iter)) {
		char *entry;
		do {
			gtk_tree_model_get (GTK_TREE_MODEL(list_store), &iter, 2, entry, -1);
			if (strcmp (entry, url_match)) {
				free (url_match);
				return;
			}
		} while (gtk_tree_model_iter_next (GTK_TREE_MODEL(list_store), &iter));
	}*/

	if (urls >= MAXURLS) {
		gtk_tree_model_get_iter_first (GTK_TREE_MODEL(list_store), &iter);
		gtk_list_store_remove (list_store, &iter);
	}
	else
		urls++;

	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, 0, word[1], 1, channel, 2, url_match, -1);
}

static int grabURL (char **word, void *userdata)
{
	regmatch_t match;

	if (regexec (url, word[2], 1, &match, REG_NOTBOL | REG_NOTEOL) == 0)
		add_match (word, match);
	else if (regexec (email, word[2], 1, &match, REG_NOTBOL | REG_NOTEOL) == 0)
		add_match (word, match);

	return XCHAT_EAT_NONE;
}

void xchat_plugin_get_info (char **plugin_name,
		char **plugin_desc,
		char **plugin_version)
{
	*plugin_name = "URL Scraper";
	*plugin_desc = "Grabs URLs and puts them in a separate window for easy viewing.";
	*plugin_version = VERSION;
}

void url_open (GtkTreeView *treeview, GtkTreePath *path,
		GtkTreeViewColumn *column, gpointer user_data)
{
	gchar *cur_url = NULL;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GError *err = NULL;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model (treeview);
	selection = gtk_tree_view_get_selection (treeview);
	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter, 2, &cur_url, -1);
		gnome_url_show (cur_url, &err);
	}
}

int xchat_plugin_init (xchat_plugin *plugin_handle,
		char **plugin_name,
		char **plugin_desc,
		char **plugin_version,
		char *arg)
{
	ph = plugin_handle;

	xchat_plugin_get_info (plugin_name, plugin_desc, plugin_version);

	url = malloc (sizeof (regex_t));
	if (regcomp (url, URLREGEX, REG_ICASE | REG_EXTENDED)) {
		xchat_print (ph, "URL Scraper failed to load: couldn't compile URL regex.\n");
		return 0;
	}

	email = malloc (sizeof (regex_t));
	if (regcomp (email, EMAILREGEX, REG_ICASE | REG_EXTENDED)) {
		xchat_print (ph, "URL Scraper failed to load: couldn't compile e-mail regex.\n");
		return 0;
	}

	urls = 0;

	make_window ();

	xchat_hook_print (ph, "Channel Message", XCHAT_PRI_NORM, grabURL, 0);
	xchat_hook_print (ph, "Private Message to Dialog", XCHAT_PRI_NORM, grabURL, 0);

	xchat_print (ph, "URL Scraper loaded.\n");

	return 1;
}

int xchat_plugin_deinit ()
{
	gtk_widget_destroy (window);

	regfree (url);
	regfree (email);

	xchat_print (ph, "URL Scraper unloaded.\n");

	return 1;
}
