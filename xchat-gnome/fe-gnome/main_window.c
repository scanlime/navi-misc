#include "main_window.h"
#include "xtext.h"
#include "palette.h"
#include "connect_dialog.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_connect_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	GtkXText *text;
	GtkBox *box;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "preferences menu")), "activate", (GCallback) on_preferences_menu_activate, NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", (GCallback) on_connect_menu_activate, NULL);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	text = gtk_xtext_new(colors, TRUE);
	box = GTK_BOX(glade_xml_get_widget(gui.xml, "main gui vbox"));
	gtk_box_pack_start(box, GTK_WIDGET(text), TRUE, TRUE, 0);
	gtk_box_reorder_child(box, GTK_WIDGET(text), TRUE);

	/* FIXME */
	gtk_xtext_set_palette(text, colors);
	gtk_xtext_set_max_lines(text, 3000);
	gtk_xtext_set_tint (text, 63, 63, 63);
	gtk_xtext_set_background(text, NULL, FALSE, FALSE);
	gtk_xtext_set_show_separator(text, TRUE);
	gtk_xtext_set_indent(text, TRUE);
	gtk_xtext_set_max_indent(text, 500);
	gtk_xtext_set_thin_separator(text, TRUE);
	gtk_xtext_set_time_stamp(text->buffer, TRUE);
	gtk_xtext_set_wordwrap(text, TRUE);

	if(!gtk_xtext_set_font(text, "Bitstream Vera Sans Mono 9"))
		g_print ("Failed to open BV Sans Mono font!\n");

	gtk_xtext_refresh(text, FALSE);
	gtk_xtext_buffer_show(text, text->buffer, TRUE);
	gtk_xtext_append_indent(text->buffer, "<hello>", 7, "yyyyyyep", 8);
	gtk_xtext_append_indent(text->buffer, "<hello world>", 13, "yyyyyyep again", 14);

	gtk_widget_show_all(GTK_WIDGET(text));
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));
	gtk_xtext_buffer_show(text, text->buffer, TRUE);
	gtk_xtext_refresh(text, FALSE);

	initialize_navigation_tree();
}

void initialize_navigation_tree() {
	GtkWidget *navigation_view;
	GtkTreeStore *store;
	GtkTreeIter parent, child;
	GtkCellRenderer *icon_renderer, *text_renderer;
	GtkTreeViewColumn *icon_column, *text_column;
	GdkPixbuf *a, *b;

	navigation_view = glade_xml_get_widget(gui.xml, "server channel list");

	store = gtk_tree_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(navigation_view), GTK_TREE_MODEL(store));

	icon_renderer = gtk_cell_renderer_pixbuf_new();
	icon_column = gtk_tree_view_column_new_with_attributes("icon", icon_renderer, "pixbuf", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), icon_column);
	text_renderer = gtk_cell_renderer_text_new();
	text_column = gtk_tree_view_column_new_with_attributes("name", text_renderer, "text", 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(navigation_view), text_column);

	a = gdk_pixbuf_new_from_file("data/logout.png", NULL);
	b = gdk_pixbuf_new_from_file("data/notauthorized.png", NULL);
	gtk_tree_store_append(store, &parent, NULL);
	gtk_tree_store_set(store, &parent, 0, a, 1, "FreeNode", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 0, b, 1, "#bzflag", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 0, b, 1, "#commits", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 1, "#tacobeam", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 1, "Diablo-D3", -1);
	gtk_tree_store_append(store, &parent, NULL);
	gtk_tree_store_set(store, &parent, 0, b, 1, "GIMPNet", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 1, "#bounties", -1);
	gtk_tree_store_append(store, &child, &parent);
	gtk_tree_store_set(store, &child, 1, "#evolution", -1);
}

void on_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}

void on_connect_menu_activate(GtkWidget *widget, gpointer data) {
	display_connection_dialog();
}
