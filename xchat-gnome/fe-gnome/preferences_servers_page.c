#include "preferences_servers_page.h"
#include "../common/xchat.h"
#include "../common/servlist.h"

void initialize_preferences_servers_page() {
	GtkWidget *treeview, *edit_button, *remove_button;
	GtkListStore *store;
	GtkCellRenderer *text_renderer;
	GtkTreeViewColumn *text_column;
	GtkTreeSelection *select;

	treeview = glade_xml_get_widget(gui.xml, "configure server list");

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), text_column);

	edit_button = glade_xml_get_widget(gui.xml, "servers edit");
	gtk_widget_set_sensitive(edit_button, FALSE);
	remove_button = glade_xml_get_widget(gui.xml, "servers remove");
	gtk_widget_set_sensitive(remove_button, FALSE);

	preferences_servers_page_populate(treeview, NULL);
}

void preferences_servers_page_populate(GtkWidget *treeview, GSList *netlist) {
	GtkListStore *store;
	GtkTreeIter *iter;
	GtkTreePath *path;
	ircnet *net;

	if(!netlist) {
//		net = servlist_net_add(_("New Network"), "");
//		servlist_server_add(net, "newserver/6667");
		netlist = network_list;
	}
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	gtk_list_store_clear(store);
	
	while(netlist) {
		net = netlist->data;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, net->name, -1);
		netlist = netlist->next;
	}
}
