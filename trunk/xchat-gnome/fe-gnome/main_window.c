#include "main_window.h"
#include "xtext.h"
#include "palette.h"

void on_preferences_menu_activate(GtkWidget *widget, gpointer data);

void initialize_main_window() {
	GtkWidget *text;
	GtkBox *box;

	gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
	g_signal_connect(G_OBJECT(glade_xml_get_widget(gui.xml, "preferences menu")), "activate", (GCallback) on_preferences_menu_activate, NULL);
	gtk_widget_show_all(GTK_WIDGET(gui.main_window));

	text = gtk_xtext_new(colors, 0);
	box = GTK_BOX(glade_xml_get_widget(gui.xml, "main gui vbox"));
	gtk_box_pack_start(box, text, TRUE, TRUE, 0);
	gtk_box_reorder_child(box, text, 1);
}

void on_preferences_menu_activate(GtkWidget *widget, gpointer data) {
	g_print("activating preferences\n");
	gtk_widget_show_all(GTK_WIDGET(gui.preferences_dialog));
}
