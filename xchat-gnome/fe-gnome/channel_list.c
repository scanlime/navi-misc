#include <gnome.h>
#include "channel_list.h"

channel_list_window *create_channel_list(session *sess) {
	channel_list_window *win;
	GtkWidget *treeview;
	GtkCellRenderer *channel_r, *users_r, *topic_r;
	GtkTreeViewColumn *channel_c, *users_c, *topic_c;
	GtkTreeSelection *select;

	win = malloc(sizeof(channel_list_window));

	win->xml = glade_xml_new("channel-list.glade", NULL, NULL);
	if(!win->xml)
		return NULL;

	treeview = glade_xml_get_widget(win->xml, "channel list");

	win->store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(win->store));

	channel_r = gtk_cell_renderer_text_new();
	channel_c = gtk_tree_view_column_new_with_attributes("Channel Name", channel_r, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), channel_c);
	users_r = gtk_cell_renderer_text_new();
	users_c = gtk_tree_view_column_new_with_attributes("Users", users_r, "text", 2, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), users_c);
	topic_r = gtk_cell_renderer_text_new();
	topic_c = gtk_tree_view_column_new_with_attributes("Topic", topic_r, "text", 3, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), topic_c);

	return win;
}
