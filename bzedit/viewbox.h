#ifndef __VIEW_H__
#define __VIEW_H__
#include <gnome.h>
#include <gtkgl/gtkglarea.h>
#include "camera.h"

class ViewBox
{
  public:
    ViewBox();
    ViewBox(const ViewBox &share);

    GtkWidget	*getWidget();

    void	readied();
    void	clear();
    void	draw_scene();
    void	make_current();
    void	swap_buffers();

    static	gint on_glarea_realize(GtkWidget *widget, gpointer data);
    static	gint on_glarea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    static	gint on_glarea_button(GtkWidget *widget, GdkEventButton *event, gpointer data);
    static	gint on_glarea_configure(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    static	gint on_glarea_motion(GtkWidget *widget, GdkEventMotion *event, gpointer data);
    static	gint on_glarea_keypress(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    static	gint on_glarea_keyrelease(GtkWidget *widget, GdkEventExpose *event, gpointer data);
    static	gint on_glarea_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);

  protected:
    GtkWidget	*dockitem;
    GtkWidget	*glarea;

    static int	number;
    double	mouse_x, mouse_y;

    bool	ready;

    Camera	camera;
};

#endif
