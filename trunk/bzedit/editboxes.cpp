#include "editboxes.h"
#include <gdl/gdl-dock-item.h>

EditBox::EditBox(std::string name)
{
  dockitem = gdl_dock_item_new(name.c_str(), name.c_str(), GDL_DOCK_ITEM_BEH_NORMAL);
  gtk_widget_show_all(dockitem);
  haschild = false;
}

GtkWidget *EditBox::getWidget()
{
  return dockitem;
}

void EditBox::replace(GtkWidget *newWidget)
{
  if(haschild)
    gtk_container_remove(GTK_CONTAINER(dockitem), child);
  if(newWidget != NULL)
    gtk_container_add(GTK_CONTAINER(dockitem), newWidget);
  child = newWidget;
  haschild = (newWidget != NULL);
}
