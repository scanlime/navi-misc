#include <glade/glade.h>
#include <gnome.h>

int main(int argc, char **argv) {
  GladeXML *xml;
  GtkWidget *topic_bar, *user_list, *server_list, *text_box;

  gnome_program_init("xchat test", "0.1", LIBGNOMEUI_MODULE, argc, argv, NULL);
  xml = glade_xml_new("xchat-gnome.glade", NULL, NULL);

  topic_bar = glade_xml_get_widget(xml, "topic entry");
  gtk_entry_set_text(GTK_ENTRY(topic_bar), "this is a topic");

  gtk_main();
  return 0;
}
