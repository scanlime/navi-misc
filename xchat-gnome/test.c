#include <glade/glade.h>
#include <gnome.h>

void settings_page_changed(GtkTreeSelection *selection, gpointer data);
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

void settings_page_changed(GtkTreeSelection *selection, gpointer data) {
  GtkTreeIter iter;
  GtkTreeModel *model;
  gchar *page;

  if(gtk_tree_selection_get_selected(selection, &model, &iter)) {
    gtk_tree_model_get(model, &iter, 1, &page, -1);
    g_print("Selected settings page %s\n", page);
    g_free(page);
  }
}

void init_settings(GladeXML *xml) {
  GtkWidget *widget, *options_list;
  GtkSizeGroup *group;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *icon, *text;
  GtkTreeViewColumn *icon_column, *text_column;
  GtkTreeSelection *select;
  GdkPixbuf *file_transfers, *irc_prefs, *user_interface;

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

  options_list = glade_xml_get_widget(xml, "settings page list");
  store = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
  icon = gtk_cell_renderer_pixbuf_new();
  text = gtk_cell_renderer_text_new();
  icon_column = gtk_tree_view_column_new_with_attributes("icon", icon, "pixbuf", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(options_list), icon_column);
  text_column = gtk_tree_view_column_new_with_attributes("name", text, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(options_list), text_column);

  gtk_tree_view_set_model(GTK_TREE_VIEW(options_list), GTK_TREE_MODEL(store));
  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(options_list));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
  g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(settings_page_changed), NULL);

  file_transfers = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/dcc.png", NULL), 24, 24, GDK_INTERP_BILINEAR);
  irc_prefs = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/irc.png", NULL), 24, 24, GDK_INTERP_BILINEAR);
  user_interface = gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("data/ui.png", NULL), 24, 24, GDK_INTERP_BILINEAR);

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, file_transfers, 1, "File Transfers", -1);
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, irc_prefs, 1, "IRC Preferences", -1);
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, user_interface, 1, "User Interface", -1);
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
