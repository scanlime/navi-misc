#ifndef __LIST_H__
#define __LIST_H__
#include <gnome.h>

class ListBox
{
  public:
    ListBox();

    GtkWidget *getWidget();

  protected:
    GtkWidget *dockitem;
    GtkWidget *scrolledwindow, *treeview;
};

#endif
