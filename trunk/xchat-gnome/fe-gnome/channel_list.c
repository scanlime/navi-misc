#include <gnome.h>
#include "channel_list.h"
#include "preferences.h"

static GSList *chanlists = NULL;

static gint chanlist_compare_p(gconstpointer a, gconstpointer b, gpointer data) {
	channel_list_window *as = (channel_list_window *) a;

	if(a == NULL)
		return 1;

	if(as->server == b)
		return 0;
	else return 1;
}

static gboolean chanlist_delete(GtkWidget *widget, GdkEvent *event, channel_list_window *win) {
	GtkWidget *window;

	g_slist_remove(chanlists, (gpointer) win);

	window = glade_xml_get_widget(win->xml, "window 1");
	gtk_widget_hide_all(window);
	g_object_unref(win->xml);
	g_free(win);
	return FALSE;
}

static void chanlist_refresh(GtkWidget *button, channel_list_window *win) {
	GtkWidget *treeview;
	GtkTreeModel *model, *store;

	treeview = glade_xml_get_widget(win->xml, "channel list");
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	store = gtk_tree_model_sort_get_model(GTK_TREE_MODEL_SORT(model));
	gtk_list_store_clear(GTK_LIST_STORE(store));
	win->server->p_list_channels(win->server, "");
}

static void chanlist_save(GtkWidget *button, channel_list_window *win) {
}

static void chanlist_join(GtkWidget *button, channel_list_window *win) {
	GtkWidget *treeview;
	GtkTreeModel *model;
	GtkTreeSelection *select;
	GtkTreeIter iter;
	char *channel;
	server *serv;

	treeview = glade_xml_get_widget(win->xml, "channel list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &channel, 3, &serv, -1);
		serv->p_join(serv, channel, "");
	}
}

static void chanlist_selected(GtkTreeSelection *selection, channel_list_window *win) {
	GtkWidget *button;
	button = glade_xml_get_widget(win->xml, "join button");

	gtk_widget_set_sensitive(button, gtk_tree_selection_get_selected(selection, NULL, NULL));
}

static gboolean chanlist_resize(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
	preferences_set_channel_list_window_size(event->width, event->height);
	return FALSE;
}

void create_channel_list(session *sess) {
	channel_list_window *win;
	GtkWidget *treeview, *widget;
	GtkCellRenderer *channel_r, *users_r, *topic_r;
	GtkTreeViewColumn *channel_c, *users_c, *topic_c;
	GtkSizeGroup *group;
	GtkTreeSelection *select;
	int width, height;

	if(sess == NULL)
		return;

	if(chanlists == NULL)
		chanlists = g_slist_alloc();

	/* check to see if we already have a channel list GUI available */
	if(g_slist_find_custom(chanlists, sess->server, (GCompareFunc) chanlist_compare_p) != NULL)
		return;

	win = g_malloc(sizeof(channel_list_window));

	win->server = sess->server;

	win->xml = glade_xml_new("channel-list.glade", NULL, NULL);
	if(!win->xml)
		win->xml = glade_xml_new(XCHATSHAREDIR"/channel-list.glade", NULL, NULL);
	if(!win->xml) {
		free(win);
		return;
	}

	widget = glade_xml_get_widget(win->xml, "window 1");
	gchar *title = g_strdup_printf("%s Channel List", sess->server->networkname);
	gtk_window_set_title(GTK_WINDOW(widget), title);
	g_free(title);
	g_signal_connect(G_OBJECT(widget), "delete-event", G_CALLBACK(chanlist_delete), win);

	treeview = glade_xml_get_widget(win->xml, "channel list");
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(treeview), 1);

	widget = glade_xml_get_widget(win->xml, "refresh button");
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(chanlist_refresh), win);
	widget = glade_xml_get_widget(win->xml, "save button");
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(chanlist_save), win);
	widget = glade_xml_get_widget(win->xml, "join button");
	gtk_widget_set_sensitive(widget, FALSE);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(chanlist_join), win);

	win->store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT);
	win->sort = GTK_TREE_MODEL_SORT(gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(win->store)));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(win->sort));
	gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(treeview), TRUE);

	channel_r = gtk_cell_renderer_text_new();
	channel_c = gtk_tree_view_column_new_with_attributes("Channel Name", channel_r, "text", 0, NULL);
	gtk_tree_view_column_set_resizable(channel_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(channel_c, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), channel_c);
	users_r = gtk_cell_renderer_text_new();
	users_c = gtk_tree_view_column_new_with_attributes("Users", users_r, "text", 1, NULL);
	gtk_tree_view_column_set_resizable(users_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(users_c, 4);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), users_c);
	topic_r = gtk_cell_renderer_text_new();
	topic_c = gtk_tree_view_column_new_with_attributes("Topic", topic_r, "text", 2, NULL);
	gtk_tree_view_column_set_resizable(topic_c, TRUE);
	gtk_tree_view_column_set_sort_column_id(topic_c, 2);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), topic_c);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(win->xml, "minimum users");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "maximum users");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "text filter");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "toggles hbox");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(win->xml, "minusers label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "maxusers label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "filter label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(win->xml, "applyto label");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(chanlist_selected), win);


	widget = glade_xml_get_widget(win->xml, "window 1");
	gtk_widget_show_all(widget);
	preferences_get_channel_list_window_size(&width, &height);
	if(!(width == 0 || height == 0))
		gtk_window_resize(GTK_WINDOW(widget), width, height);
	g_signal_connect(G_OBJECT(widget), "configure-event", G_CALLBACK(chanlist_resize), NULL);

	g_slist_append(chanlists, win);
}

void channel_list_append(server *serv, char *channel, char *users, char *topic) {
	GtkWidget *treeview;
	GtkListStore *store;
	GtkTreeModelSort *sort;
	GtkTreeIter iter;
	GSList *element;
	channel_list_window *win;
	int nusers;

	element = g_slist_find_custom(chanlists, serv, (GCompareFunc) chanlist_compare_p);
	if(element == NULL)
		return;

	win = element->data;
	treeview = glade_xml_get_widget(win->xml, "channel list");
	sort = GTK_TREE_MODEL_SORT(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	store = GTK_LIST_STORE(gtk_tree_model_sort_get_model(sort));

	nusers = atoi(users);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, channel, 1, users, 2, topic, 3, serv, 4, nusers, -1);
}

void repopulate_channel_list(channel_list_window *win) {
}

gboolean channel_list_exists(server *serv) {
	return (g_slist_find_custom(chanlists, serv, (GCompareFunc) chanlist_compare_p) != NULL);
}
