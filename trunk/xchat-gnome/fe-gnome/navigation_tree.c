#include "navigation_tree.h"

void initialize_navigation_tree() {
	GtkWidget *navigation_view;
	GtkTreeStore *store;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;

	navigation_view = glade_xml_get_widget(gui.xml, "server channel list");

	store = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model(GTK_TREE_VIEW(navigation_view), GTK_TREE_MODEL(store));

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), text_column);
}

void navigation_tree_create_new_network_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkWidget *treeview;
	GtkTreeIter iter;
	
	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 2, sess, -1);
}

static gboolean navigation_tree_set_channel_name_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		struct session *sess = s;
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 1, (sess->channel), -1);
	}
	return TRUE;
}

void navigation_tree_set_channel_name(struct session *sess) {
	GtkTreeModel *store;
	GtkWidget *treeview;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

	gtk_tree_model_foreach(store, navigation_tree_set_channel_name_iterate, sess);
}
