#include <sys/types.h>
#include <regex.h>
#include <gtk/gtk.h>

#include "xchat-plugin.h"

#define VERSION "0.2"

static xchat_plugin *ph;	// Plugin handle.
static regex_t email;		// Regex that matches e-mail addresses.
static regex_t url;			// Regex that matches urls.
static int urls;			// Current total in the scraper.

static GtkWidget *window;
static GtkListStore *list_store;

static void make_window ()
{
	GtkScrolledWindow *scrolled;
	GtkWidget *treeview;
	GtkTreeViewColumn *nick_col, *chan_col, *url_col;
	GtkCellRendererText *nick_rend, *chan_rend, *url_rend;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	list_store = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	scrolled = gtk_scrolled_window_new (NULL, NULL);
	treeview = gtk_tree_view_new ();

	nick_rend = gtk_cell_renderer_text_new ();
	nick_col = gtk_tree_view_column_new_with_attributes ("Nick", nick_rend, 0);

	chan_rend = gtk_cell_renderer_text_new ();
	chan_col = gtk_tree_view_column_new_with_attributes ("Channel", chan_rend, 0);

	url_rend = gtk_cell_renderer_text_new ();
	url_col = gtk_tree_view_column_new_with_attributes ("URL", url_rend, 0);

}

static int grabURL (char **word, char **userdata)
{
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

	regcomp (&email, "[\w\.\-\+]+@([0-9a-z\-]+\.)+[a-z]+", REG_ICASE);
	regcomp (&url, "(ht|f)tps?://[^\s\>\]\)]+", REG_ICASE);

	urls = 0;

	xchat_hook_print (ph, "Channel Message", XCHAT_PRI_NORM, grabURL, 0);

	xchat_print (ph, "URL Scrapler loaded.\n");

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
