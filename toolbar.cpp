#include "toolbar.h"
#include "globals.h"

Toolbar::Toolbar()
{
  toolbar = gtk_toolbar_new();

  append_element_stock(GTK_STOCK_NEW, "Create a new BZFlag world");
  append_element_stock(GTK_STOCK_OPEN, "Open a BZFlag world");
  append_element_stock(GTK_STOCK_SAVE, "Save the current world");
  append_space();
  append_element_file("data/zplus.png", "View from above");
  append_space();
  append_element_file("data/yplus.png", "View from front");
  append_element_file("data/yminus.png", "View from back");
  append_space();
  append_element_file("data/xplus.png", "View from right side");
  append_element_file("data/xminus.png", "View from left side");
  append_space();
  append_element_file("data/iso.png", "Isometric view");
  append_element_file("data/target.png", "Center view on (0, 0, 0)");
  append_space();
  append_element_file("data/addbox.png", "Add a box", on_abox);
  append_element_file("data/addpyr.png", "Add a pyramid", on_apyr);
  append_element_file("data/addtel.png", "Add a teleporter", on_atel);
  append_element_file("data/addlnk.png", "Add a link");
  append_element_file("data/addbas.png", "Add a base", on_abas);
  append_space();
  append_toggle_file("data/selectmode.png", "Selection view");
  append_toggle_file("data/rotatemode.png", "Rotate mode");
  append_toggle_file("data/scalemode.png", "Scale mode");
}

Toolbar::~Toolbar()
{
}

GtkWidget *Toolbar::getWidget()
{
  return toolbar;
}

void Toolbar::append_element_stock(const gchar *stockid, gchar *desc)
{
  GtkWidget *tmp_icon = gtk_image_new_from_stock(stockid, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_toolbar_append_element(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_CHILD_BUTTON, NULL, NULL, desc, NULL, tmp_icon, NULL, NULL);
}

void Toolbar::append_element_file(gchar *filename, gchar *desc)
{
  GtkWidget *tmp_icon = gtk_image_new_from_file(filename);
  gtk_toolbar_append_element(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_CHILD_BUTTON, NULL, NULL, desc, NULL, tmp_icon, NULL, NULL);
}

void Toolbar::append_element_file(gchar *filename, gchar *desc, gint (*cb)(GtkWidget*, Toolbar*))
{
  GtkWidget *tmp_icon = gtk_image_new_from_file(filename);
  GtkWidget *w = gtk_toolbar_append_element(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_CHILD_BUTTON, NULL, NULL, desc, NULL, tmp_icon, NULL, NULL);
  gtk_signal_connect(GTK_OBJECT(w), "clicked", GTK_SIGNAL_FUNC(cb), this);
}

void Toolbar::append_toggle_file(gchar *filename, gchar *desc)
{
  GtkWidget *tmp_icon = gtk_image_new_from_file(filename);
  gtk_toolbar_append_element(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_CHILD_TOGGLEBUTTON, NULL, NULL, desc, NULL, tmp_icon, NULL, NULL);
}

void Toolbar::append_space()
{
  gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));
}

gint Toolbar::on_abox(GtkWidget *widget, Toolbar *data)
{
  doc->addbox();
  return TRUE;
}

gint Toolbar::on_apyr(GtkWidget *widget, Toolbar *data)
{
  doc->addpyr();
  return TRUE;
}

gint Toolbar::on_atel(GtkWidget *widget, Toolbar *data)
{
  doc->addtel();
  return TRUE;
}

gint Toolbar::on_alnk(GtkWidget *widget, Toolbar *data)
{
  doc->addlnk();
  return TRUE;
}

gint Toolbar::on_abas(GtkWidget *widget, Toolbar *data)
{
  doc->addbas();
  return TRUE;
}
