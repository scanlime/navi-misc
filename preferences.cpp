#include "preferences.h"
#include "cell-renderer-captioned-image.h"

Preferences::Preferences(GtkWidget *parent)
{
  dialog = gtk_dialog_new_with_buttons("BZEdit Preferences", GTK_WINDOW(parent), GTK_DIALOG_DESTROY_WITH_PARENT, NULL);

  gtk_signal_connect(GTK_OBJECT(gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, 0)), "clicked", GTK_SIGNAL_FUNC(on_button_ok), this);
  gtk_signal_connect(GTK_OBJECT(gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_APPLY, 0)), "clicked", GTK_SIGNAL_FUNC(on_button_apply), this);
  gtk_signal_connect(GTK_OBJECT(gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CLOSE, 0)), "clicked", GTK_SIGNAL_FUNC(on_button_close), this);

  gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 350);
  g_signal_connect(dialog, "delete_event", G_CALLBACK(on_delete_event), this);

  hbox = gtk_hbox_new(FALSE, 0);

  treeview = gtk_tree_view_new();
  gtk_widget_show(treeview);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
  store = gtk_list_store_new(LAST_COL, G_TYPE_STRING, GDK_TYPE_PIXBUF, GTK_TYPE_WIDGET);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));

  GtkCellRenderer *renderer = anjuta_cell_renderer_captioned_image_new();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Category", renderer, "text", COL_NAME, "pixbuf", COL_PIXBUF, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  swindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(swindow), GTK_SHADOW_IN);
  gtk_widget_show(swindow);

  gtk_container_add(GTK_CONTAINER(swindow), treeview);
  gtk_box_pack_start(GTK_BOX(hbox), swindow, FALSE, FALSE, 0);

  notebook = gtk_notebook_new();
  gtk_widget_show(notebook);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(notebook), GNOME_PAD_SMALL);
  gtk_box_pack_start(GTK_BOX(hbox), notebook, TRUE, TRUE, 0);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
  g_signal_connect(selection, "changed", G_CALLBACK(on_selection_changed), this);

  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

  gtk_widget_show(hbox);
}

void Preferences::show()
{
  gtk_widget_show(dialog);
}

void Preferences::hide()
{
  gtk_widget_hide(dialog);
}

gint Preferences::on_selection_changed(GtkWidget *widget, gpointer data)
{
  Preferences *p = static_cast<Preferences*>(data);

  GtkTreeIter iter;

  if(gtk_tree_selection_get_selected(p->selection, NULL, &iter)) {
    GtkWidget *widget;
    gtk_tree_model_get(GTK_TREE_MODEL(p->store), &iter, COL_WIDGET, &widget, -1);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(p->notebook), gtk_notebook_page_num(GTK_NOTEBOOK(p->notebook), widget));
  }
  return TRUE;
}

gint Preferences::on_button_close(GtkWidget *widget, gpointer data)
{
  Preferences *p = static_cast<Preferences*>(data);
  gtk_widget_hide(p->dialog);
  return TRUE;
}

gint Preferences::on_button_apply(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint Preferences::on_button_ok(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint Preferences::on_delete_event(GtkWidget *widget, gpointer data)
{
  gtk_widget_hide(widget);
  return TRUE;
}

void Preferences::add_page(const char *name, GdkPixbuf *icon, GtkWidget *page)
{
  GtkTreeIter iter;

  gtk_widget_show(page);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, NULL);
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, COL_NAME, name, COL_PIXBUF, icon, COL_WIDGET, page, -1);
}

void Preferences::remove_page(const char *name)
{
  // FIXME
}
