#include "main_window.h"
#include "xtext.h"
#include "palette.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	GtkXText *text;
	GtkBox *box;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "preferences menu")), "activate", (GCallback) on_preferences_menu_activate, NULL);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	text = gtk_xtext_new(colors, 1);
	box = GTK_BOX(glade_xml_get_widget(gui.xml, "main gui vbox"));
	gtk_box_pack_start(box, GTK_WIDGET(text), TRUE, TRUE, 0);
	gtk_box_reorder_child(box, GTK_WIDGET(text), TRUE);

	/* FIXME */
	gtk_xtext_set_show_separator(text, TRUE);
	gtk_xtext_set_indent(text, TRUE);
	gtk_xtext_set_max_lines(text, 3000);
	gtk_xtext_set_font(text, "Bitstream Vera Sans Mono 8");
	gtk_xtext_set_background(text, NULL, FALSE, FALSE);
	gtk_xtext_set_palette(text, colors);
	gtk_xtext_append_indent(text->buffer, "<hello>", 7, "yyyyyyep", 8);
	gtk_xtext_append(text->buffer, "test", 4);

	gtk_widget_show_all(GTK_WIDGET(text));
	gtk_xtext_buffer_show(text, text->buffer, 1);
	gtk_xtext_refresh(text, FALSE);
}

void on_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	g_print("activating preferences\n");
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}
