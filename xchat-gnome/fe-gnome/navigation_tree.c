#include "navigation_tree.h"
#include "textgui.h"
#include "userlist.h"

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data);

void initialize_navigation_tree() {
	GtkWidget *navigation_view;
	GtkTreeStore *store;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GtkTreeSelection *select;

	navigation_view = glade_xml_get_widget(gui.xml, "server channel list");

	store = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model(GTK_TREE_VIEW(navigation_view), GTK_TREE_MODEL(store));

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(navigation_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
	g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
}

void navigation_tree_create_new_network_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkWidget *treeview;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_store_append(store, &iter, NULL);
	gtk_tree_store_set(store, &iter, 1, "<none>", 2, sess, -1);
}

static gboolean navigation_tree_create_new_channel_entry_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, struct session *data) {
	struct session *s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s->type == SESS_SERVER && s->server == data->server) {
		GtkTreeIter child;
		GtkWidget *treeview;

		treeview = glade_xml_get_widget(gui.xml, "server channel list");
		
		gtk_tree_store_append(GTK_TREE_STORE(model), &child, iter);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child, 1, "<none>", 2, data, -1);
		/* make sure the tree expands to show the new channel */
		gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, TRUE);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_create_new_channel_entry(struct session *sess) {
	GtkTreeStore *store;
	GtkWidget *treeview;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));

	gtk_tree_model_foreach(store, navigation_tree_create_new_channel_entry_iterate, sess);
}

static gboolean navigation_tree_set_channel_name_iterate(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	gpointer s;
	gtk_tree_model_get(model, iter, 2, &s, -1);
	if(s == data) {
		struct session *sess = s;
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 1, (sess->channel), -1);
		return TRUE;
	}
	return FALSE;
}

void navigation_tree_set_channel_name(struct session *sess) {
	GtkTreeModel *store;
	GtkWidget *treeview;
	GtkTreeIter iter;

	treeview = glade_xml_get_widget(gui.xml, "server channel list");
	store = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

	gtk_tree_model_foreach(store, navigation_tree_set_channel_name_iterate, sess);
}

void navigation_selection_changed(GtkTreeSelection *selection, gpointer data) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gpointer *s;
	session *sess;
	session_gui *tgui;

	if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 2, &s, -1);
		sess = s;
		tgui = sess->gui;
		gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
		gui.current_session = sess;
		userlist_display(tgui);
		set_nickname(sess->server, NULL);
	}
}
