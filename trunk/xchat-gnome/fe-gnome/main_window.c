#include "main_window.h"

void initialize_main_window() {
  gui.main_window = GNOME_APP(glade_xml_get_widget(gui.xml, "xchat-gnome"));
  gtk_widget_show_all(GTK_WIDGET(gui.main_window));
}
