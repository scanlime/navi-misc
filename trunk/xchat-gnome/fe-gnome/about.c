#include "about.h"

void intialize_about_dialog() {
	GtkWidget *about;
	GdkPixbuf *soap;

	about = glade_xml_get_widget(gui.xml, "about1");
	soap = gdk_pixbuf_new_from_file("data/soap.png", NULL);
	g_object_set(G_OBJECT(about), "logo", soap);
}

void show_about_dialog() {
	GtkWidget *about;

	about = glade_xml_get_widget(gui.xml, "about1");
	gtk_widget_show_all(about);
}
