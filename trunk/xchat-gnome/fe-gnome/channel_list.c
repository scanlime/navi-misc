#include <gnome.h>
#include "channel_list.h"

channel_list_window *create_channel_list(session *sess) {
	channel_list_window *win;
	GtkWidget *scrolled_window, *settings_frame, *vbox;
	GtkCellRenderer *channel_r, *users_r, *topic_r;
	GtkTreeViewColumn *channel_c, *users_c, *topic_c;
	GtkTreeSelection *select;

	win = malloc(sizeof(channel_list_window));
	win->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	win->listview = gtk_tree_view_new();
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	vbox = gtk_vbox_new(FALSE, 0);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);

	win->store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(win->listview), GTK_TREE_MODEL(win->store));

	channel_r = gtk_cell_renderer_text_new();
	channel_c = gtk_tree_view_column_new_with_attributes("Channel Name", channel_r, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(win->listview), channel_c);
	users_r = gtk_cell_renderer_text_new();
	users_c = gtk_tree_view_column_new_with_attributes("Users", users_r, "text", 2, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(win->listview), users_c);
	topic_r = gtk_cell_renderer_text_new();
	topic_c = gtk_tree_view_column_new_with_attributes("Topic", topic_r, "text", 3, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(win->listview), topic_c);

	return win;
}
