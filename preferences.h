#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include <gnome.h>

class Preferences
{
  public:
    Preferences(GtkWidget *parent);

    void	show();
    void	hide();

    static gint on_selection_changed(GtkWidget *widget, gpointer data);
    static gint on_delete_event(GtkWidget *widget, gpointer data);
    static gint on_button_close(GtkWidget *widget, gpointer data);
    static gint on_button_apply(GtkWidget *widget, gpointer data);
    static gint on_button_ok(GtkWidget *widget, gpointer data);

    void	add_page(const char *name, GdkPixbuf *icon, GtkWidget *page);
    void	remove_page(const char *name);

  protected:
    GtkWidget		*dialog;
    GtkWidget		*hbox;
    GtkWidget		*swindow;
    GtkWidget		*treeview;
    GtkWidget		*notebook;

    GtkListStore	*store;
    GtkTreeSelection	*selection;

    enum {
      COL_NAME,
      COL_PIXBUF,
      COL_WIDGET,
      LAST_COL
    };
};

#endif
