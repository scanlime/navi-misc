#include "connect_dialog.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

void initialize_connection_dialog() {
	GtkWidget *treeview;
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
