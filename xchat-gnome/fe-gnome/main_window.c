#include "main_window.h"
#include "xtext.h"
#include "palette.h"
#include "connect_dialog.h"
#include "about.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);
void on_connect_menu_activate(GtkWidget *widget, gpointer data);
void on_about_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	GtkXText *text;
	GtkBox *box;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "settings1")), "activate", G_CALLBACK(on_preferences_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "connect1")), "activate", G_CALLBACK(on_connect_menu_activate), NULL);
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "about1")), "activate", G_CALLBACK(on_about_menu_activate), NULL);
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
