#include "connect_dialog.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

void connection_dialog_close(GtkWidget *widget, gpointer data);
void connection_dialog_connect(GtkWidget *widget, gpointer data);

void initialize_connection_dialog() {
	GtkWidget *treeview, *close_button, *connect_button;
	GtkListStore *store;
	GtkCellRenderer *text_renderer;
	GtkTreeViewColumn *text_column;

	GtkTreeSelection *select;

	treeview = glade_xml_get_widget(gui.xml, "connect server list");

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), text_column);

	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);

	close_button = glade_xml_get_widget(gui.xml, "close server connect");
	g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(connection_dialog_close), NULL);
	connect_button = glade_xml_get_widget(gui.xml, "connect to server");
	g_signal_connect(G_OBJECT(connect_button), "clicked", G_CALLBACK(connection_dialog_connect), NULL);
}

void display_connection_dialog() {
	GtkWidget *dialog, *treeview;
	GtkListStore *store;
	GtkTreeIter iter;
	ircnet *net;
	GSList *netlist;

	treeview = glade_xml_get_widget(gui.xml, "connect server list");

	/* FIXME: get a custom list? */
	netlist = network_list;
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	gtk_list_store_clear(store);

	while(netlist) {
		net = netlist->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, net->name, -1);
		netlist = netlist->next;
	}

	dialog = glade_xml_get_widget(gui.xml, "connect to network");
	gtk_widget_show_all(dialog);
}

void connection_dialog_close(GtkWidget *widget, gpointer data) {
	GtkWidget *dialog = glade_xml_get_widget(gui.xml, "connect to network");
	gtk_widget_hide_all(dialog);
}

void connection_dialog_connect(GtkWidget *widget, gpointer data) {
	GtkWidget *treeview;
	GtkTreeSelection *select;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *network;

	treeview = glade_xml_get_widget(gui.xml, "connect server list");
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

	if(gtk_tree_selection_get_selected(select, &model, &iter)) {
		gtk_tree_model_get(model, &iter, 0, &network, -1);
		g_print("want to connect to \"%s\"\n", network);
		g_free(network);
	}
}
