#ifndef __DOCKWINDOW_H__
#define __DOCKWINDOW_H__

#include <gnome.h>
#include <gdl/gdl-dock-layout.h>

class DockWindow
{
  public:
    DockWindow();
    ~DockWindow();

    GtkWidget *getDock();

  protected:
    GtkWidget *dock;
    GdlDockLayout *layout_manager;
};

#endif
