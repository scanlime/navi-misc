#include "preferences_dialog.h"

void initialize_pages_list();
void initialize_irc_preferences_page();
void initialize_file_transfers_page();
void initialize_servers_page();
void settings_page_changed(GtkTreeSelection *selection, gpointer data);

void initialize_preferences_dialog() {
	gui.preferences_dialog = GTK_DIALOG(glade_xml_get_widget(gui.xml, "preferences"));
	gtk_window_set_transient_for(GTK_WINDOW(gui.preferences_dialog), GTK_WINDOW(gui.main_window));
	gtk_widget_hide_all(GTK_WIDGET(gui.preferences_dialog));
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(glade_xml_get_widget(gui.xml, "settings notebook")), FALSE);
	initialize_pages_list();
	initialize_irc_preferences_page();
	initialize_file_transfers_page();
}

void initialize_pages_list() {
	GtkWidget *options_list;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;
	GdkPixbuf *file_transfers, *irc_prefs, *servers;

	options_list = glade_xml_get_widget(gui.xml, "settings page list");

	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(options_list), GTK_TREE_MODEL(store));

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(options_list), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(options_list), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(options_list));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(settings_page_changed), NULL);

	irc_prefs = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/irc.png", NULL), 24, 24, GDK_INTERP_BILINEAR);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, irc_prefs, 1, "IRC Preferences", 2, 0, -1);
	file_transfers = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/dcc.png", NULL), 24, 24, GDK_INTERP_BILINEAR);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, file_transfers, 1, "File Transfers & DCC", 2, 1, -1);
	servers = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/servers.png", NULL), 24, 24, GDK_INTERP_BILINEAR);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, servers, 1, "Servers", 2, 2, -1);
}

void initialize_irc_preferences_page() {
	GtkWidget *widget;
	GtkSizeGroup *group;

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "nick name");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "real name");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "quit message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "part message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "away message");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "highlight list container");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}

void initialize_file_transfers_page() {
	GtkWidget *widget;
	GtkSizeGroup *group;

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "dcc transfer destination");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "completed dcc destination");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "download label");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "completed label");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);

	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = glade_xml_get_widget(gui.xml, "dcc ip address");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "individual send throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "global send throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "individual receive throttle");
	gtk_size_group_add_widget(group, widget);
	widget = glade_xml_get_widget(gui.xml, "global receive throttle");
	gtk_size_group_add_widget(group, widget);
	g_object_unref(group);
}

void initialize_servers_page() {
}

void settings_page_changed(GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkWidget *notebook;
	gint page;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		notebook = glade_xml_get_widget(gui.xml, "settings notebook");
		gtk_tree_model_get(model, &iter, 2, &page, -1);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
	}
}
