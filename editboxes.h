#ifndef __EDITBOXES_H__
#define __EDITBOXES_H__
#include <gnome.h>
#include <string>

class EditBox
{
  public:
    EditBox(std::string name);

    GtkWidget *getWidget();

    void replace(GtkWidget *newWidget);

  protected:
    GtkWidget *dockitem;
    GtkWidget *child;
    bool haschild;
};

#endif
