#include "about.h"

void initialize_about_dialog() {
	GtkWidget *about;
	GdkPixbuf *soap;

	about = glade_xml_get_widget(gui.xml, "about dialog");
	soap = gdk_pixbuf_new_from_file("data/soap.png", NULL);
	g_object_set(G_OBJECT(about), "logo", soap, NULL);
}

void show_about_dialog() {
	GtkWidget *about;

	about = glade_xml_get_widget(gui.xml, "about dialog");
	gtk_widget_show_all(about);
}
