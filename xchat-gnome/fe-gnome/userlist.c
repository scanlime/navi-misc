#include "userlist.h"
#include "../common/xchat.h"
#include "../common/userlist.h"

void initialize_userlist() {
	GtkWidget *userlist_view;
	GtkListStore *store;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	userlist_view = glade_xml_get_widget(gui.xml, "userlist");

	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER);
	gtk_tree_view_set_model(GTK_TREE_VIEW(userlist_view), GTK_TREE_MODEL(store));

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(userlist_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(userlist_view), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(userlist_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	/* FIXME: signale */
}

void userlist_insert(session *sess, struct User *newuser, int row, int sel) {
	GtkWidget *userlist_view;
	GtkListStore *store;
	GdkPixbuf *pix;
	GtkTreeIter iter;

	userlist_view = glade_xml_get_widget(gui.xml, "userlist");
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(userlist_view)));

	gtk_list_store_insert(store, &iter, row);
	gtk_list_store_set(store, &iter, 1, newuser->nick, 2, newuser, 3, sess, -1);
	/* FIXME: colors, away status, icons, selection */
}
