#include "about.h"

void on_about_close(GtkWidget *widget, gpointer data);

void initialize_about_dialog() {
	GtkWidget *about;
	GdkPixbuf *soap;
	gchar *authors[] = {
		"David Trowbridge",
		"Dan Kuester",
		NULL
	};
	gchar *documentors[] = {
		NULL
	};

	soap = gdk_pixbuf_new_from_file("data/xchat-gnome-small.png", NULL);
	gui.about = GNOME_ABOUT(gnome_about_new(
		"X-Chat GNOME",
		"x.x",
		"Copyright (c) 2004\nDavid Trowbridge and Daniel Kuester",
		"It has been well observed that a trombone\nis not a suitable instrument for a gentleman",
		authors,
		documentors,
		"",
		soap));
}

void show_about_dialog() {
	initialize_about_dialog();
	gtk_widget_show_all(GTK_WIDGET(gui.about));
}

void on_about_close(GtkWidget *widget, gpointer data) {
	gtk_widget_hide_all(GTK_WIDGET(gui.about));
}
