#include "main_window.h"
#include "connect_dialog.h"
#include "about.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_about_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_about_menu_activate), NULL);

	gtk_widget_show_all(GTK_WIDGET(gui.main_window));
}

void on_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}

void on_connect_menu_activate(GtkWidget *widget, gpointer data) {
	display_connection_dialog();
}

void on_about_menu_activate(GtkWidget *widget, gpointer data) {
	show_about_dialog();
}
