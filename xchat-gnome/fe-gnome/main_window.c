#include "main_window.h"

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "preferences menu")), "activate", (GCallback) on_preferences_menu_activate, NULL);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));
}

void on_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	g_print("activating preferences\n");
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}
