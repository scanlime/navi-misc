#include "main_window.h"
#include "connect_dialog.h"
#include "about.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_about_menu_activate(GtkWidget *widget, gpointer data);
void on_text_entry_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	GtkWidget *entry;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_about_menu_activate), NULL);

	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	entry = glade_xml_get_widget(gui.xml, "text entry");
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(on_text_entry_activate), NULL);
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

void on_text_entry_activate(GtkWidget *widget, gpointer data) {
	const gchar *entry_text;
	entry_text = gtk_entry_get_text(GTK_ENTRY(widget));
	g_print("Entry contents: %s\n", entry_text);
	handle_multiline(gui.current_session, entry_text, TRUE, FALSE);
	gtk_entry_set_text(GTK_ENTRY(widget), "");
}
