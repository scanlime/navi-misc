#include "preferences_dialog.h"

void initialize_preferences_dialog() {
  gui.preferences_dialog = GTK_DIALOG(glade_xml_get_widget(gui.xml, "preferences"));
  gtk_window_set_transient_for(GTK_WINDOW(gui.preferences_dialog), GTK_WINDOW(gui.main_window));
  gtk_widget_hide_all(GTK_WIDGET(gui.preferences_dialog));
}
