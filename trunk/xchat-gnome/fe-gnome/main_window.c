#include "main_window.h"
#include "connect_dialog.h"
#include "about.h"
#include "userlist.h"
#include "../common/xchatc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_about_menu_activate(GtkWidget *widget, gpointer data);
void on_quit_menu_activate(GtkWidget *widget, gpointer data);
void on_text_entry_activate(GtkWidget *widget, GdkEventKey *event, gpointer data);

void initialize_main_window() {
	GtkWidget *entry;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_about_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "quit1")), "activate", G_CALLBACK(on_quit_menu_activate), NULL);

	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	entry = glade_xml_get_widget(gui.xml, "text entry");
	g_signal_connect(G_OBJECT(entry), "key-release-event", G_CALLBACK(on_text_entry_activate), NULL);
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

void on_quit_menu_activate(GtkWidget *widget, gpointer data) {
	xchat_exit();
}

void on_text_entry_activate(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	GtkTextBuffer *buffer;
	const gchar *entry_text;
	GtkTextIter start, end;

	if(event->keyval == GDK_Return) {
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		gtk_text_buffer_get_start_iter(buffer, &start);
		gtk_text_buffer_get_end_iter(buffer, &end);
		entry_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		handle_multiline(gui.current_session, entry_text, TRUE, FALSE);
		gtk_text_buffer_set_text(buffer, "", -1);
		/*
		entry_text = gtk_entry_get_text(GTK_ENTRY(widget));
		handle_multiline(gui.current_session, entry_text, TRUE, FALSE);
		gtk_entry_set_text(GTK_ENTRY(widget), "");
		*/
	}
}
