#include <glade/glade.h>
#include <gnome.h>

void init_settings(GladeXML *xml);
void init_userlist(GladeXML *xml);

int main(int argc, char **argv) {
  GladeXML *xml;
  GtkWidget *topic_bar, *user_list, *server_list, *text_box;

  gnome_program_init("xchat test", "0.1", LIBGNOMEUI_MODULE, argc, argv, NULL);
  xml = glade_xml_new("xchat-gnome.glade", NULL, NULL);

  init_settings(xml);
  init_userlist(xml);

  topic_bar = glade_xml_get_widget(xml, "topic entry");
  gtk_entry_set_text(GTK_ENTRY(topic_bar), "this is a topic");

  gtk_main();
  return 0;
}

void init_settings(GladeXML *xml) {
  GtkWidget *widget;
  GtkSizeGroup *group;

  group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  widget = glade_xml_get_widget(xml, "dcc transfer destination");
  gtk_size_group_add_widget(group, widget);
  widget = glade_xml_get_widget(xml, "completed dcc destination");
  gtk_size_group_add_widget(group, widget);
  widget = glade_xml_get_widget(xml, "dcc ip address");
  gtk_size_group_add_widget(group, widget);

  widget = glade_xml_get_widget(xml, "individual send throttle");
  gtk_size_group_add_widget(group, widget);
  widget = glade_xml_get_widget(xml, "global send throttle");
  gtk_size_group_add_widget(group, widget);
  widget = glade_xml_get_widget(xml, "individual receive throttle");
  gtk_size_group_add_widget(group, widget);
  widget = glade_xml_get_widget(xml, "global receive throttle");
  gtk_size_group_add_widget(group, widget);
  g_object_unref(group);
}

void init_userlist(GladeXML *xml) {
  GtkWidget *user_list;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  int i;

  user_list = glade_xml_get_widget(xml, "userlist");
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(user_list), GTK_TREE_MODEL(store));

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("User", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(user_list), column);

  for(i = 0; i < 10; i++) {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "jimmy", -1);
  }
}
