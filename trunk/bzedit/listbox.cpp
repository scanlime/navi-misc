#include "listbox.h"
#include "globals.h"
#include "cell-renderer-toggle-image.h"
#include <gdl/gdl-dock-item.h>

ListBox::ListBox()
{
  dockitem = gdl_dock_item_new("listbox", "list of elements", GDL_DOCK_ITEM_BEH_NORMAL);
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  treeview = gtk_tree_view_new_with_model(doc->model);

  GtkCellRenderer *pixbufrender, *textrender;
//  GtkCellRenderer *pixbufrender, *togglerender, *textrender;
  GtkTreeViewColumn *column;

  pixbufrender = gtk_cell_renderer_pixbuf_new();
//  togglerender = bze_cell_renderer_toggle_image_new();
  textrender = gtk_cell_renderer_text_new();
  g_object_set(G_OBJECT(textrender), "editable", TRUE, NULL);
  // FIXME: connect signal

  column = gtk_tree_view_column_new_with_attributes("name", textrender, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  column = gtk_tree_view_column_new_with_attributes("icon", pixbufrender, "pixbuf", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  column = gtk_tree_view_column_new_with_attributes("visible", pixbufrender, "pixbuf", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
  gtk_tree_view_column_set_resizable(gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 0), TRUE);
  gtk_tree_view_column_set_resizable(gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 1), FALSE);
  gtk_tree_view_column_set_resizable(gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 2), FALSE);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledwindow), treeview);
  gtk_container_add(GTK_CONTAINER(dockitem), scrolledwindow);

  gtk_widget_set_size_request(scrolledwindow, 150, 400);
  gtk_widget_show_all(dockitem);
}

GtkWidget *ListBox::getWidget()
{
  return dockitem;
}
