#include <sys/types.h>
#include <regex.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include "xchat-plugin.h"

#define VERSION "0.2"
#define MAXURLS 10

#define URLREGEX "(ht|f)tps?://[^a-zA-z0-9]+"
#define EMAILREGEX "[\\w\\.\\-\\+]+@([0-9a-z\\-]+\\.)+[a-z]+"

static xchat_plugin *ph;	// Plugin handle.
//static regex_t *email;	// Regex that matches e-mail addresses.
static regex_t *url;		// Regex that matches urls.
static int urls;			// Current total in the scraper.

static GtkWidget *window;
static GtkListStore *list_store;

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

	gtk_widget_show_all (window);
}

static int grabURL (char **word, void *userdata)
{
	GtkTreeIter iter;

	const char *chan;
	size_t len = 1;
	regmatch_t *match = malloc (len * sizeof (regmatch_t));

	if (regexec(url, word[4], len, match, REG_NOTBOL | REG_NOTEOL) == 0)
	{
		chan = xchat_get_info (ph, "channel");
		xchat_print (ph, "URL found.\n");

		if (urls >= MAXURLS)
		{
			gtk_tree_model_get_iter_first (GTK_TREE_MODEL(list_store), &iter);
			gtk_list_store_remove (list_store, &iter);
		}
		else
		{
			urls++;
		}

		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (list_store, &iter, 0, word[3], 1, chan, 2, match, -1);
	}

	return XCHAT_EAT_NONE;
}

int xchat_plugin_init (xchat_plugin *plugin_handle,
		char **plugin_name,
		char **plugin_desc,
		char **plugin_version,
		char *arg)
{
	ph = plugin_handle;

	*plugin_name = "URL Scraper";
	*plugin_desc = "Grabs URLs and puts them in a separate window for easy viewing.";
	*plugin_version = VERSION;

	//regcomp (&email, EMAILREGEX, REG_ICASE);

	url = malloc (sizeof (regex_t));
	if (regcomp (url, URLREGEX, REG_EXTENDED | REG_ICASE))
	{
		xchat_print (ph, "URL Scraper failed to load: couldn't compile URL regex.\n");
		return 0;
	}

	urls = 0;

	make_window ();

	xchat_hook_print (ph, "Channel Message", XCHAT_PRI_NORM, grabURL, 0);

	xchat_print (ph, "URL Scraper loaded.\n");

	return 1;
}

int xchat_plugin_deinit ()
{
	gtk_widget_destroy (window);

	xchat_print (ph, "URL Scraper unloaded.\n");

	return 1;
}

void xchat_plugin_get_info (char **plugin_name,
		char **plugin_desc,
		char **plugin_version)
{
	*plugin_name = "URL Scraper";
	*plugin_desc = "Grabs URLs and puts them in a separate window for easy viewing.";
	*plugin_version = VERSION;
}
