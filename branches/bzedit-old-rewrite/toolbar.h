#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__
#include <gnome.h>

class Toolbar
{
  public:
    Toolbar();
    ~Toolbar();

    GtkWidget *getWidget();

    static gint on_abox(GtkWidget *widget, Toolbar *data);
    static gint on_apyr(GtkWidget *widget, Toolbar *data);
    static gint on_atel(GtkWidget *widget, Toolbar *data);
    static gint on_alnk(GtkWidget *widget, Toolbar *data);
    static gint on_abas(GtkWidget *widget, Toolbar *data);

  protected:
    GtkWidget *toolbar;
    GtkWidget *newf, *open, *save;
    GtkWidget *zplus;
    GtkWidget *yplus, *yminus;
    GtkWidget *xplus, *xminus;
    GtkWidget *iso, *target;
    GtkWidget *abox, *apyr, *atel, *alnk, *abas;
    GtkWidget *selmode, *rotatemode, *scalemode;

    void append_element_stock(const gchar *stockid, gchar *desc);
    void append_element_file(gchar *filename, gchar *desc);
    void append_element_file(gchar *filename, gchar *desc, gint (*cb)(GtkWidget*, Toolbar*));
    void append_toggle_file(gchar *filename, gchar *desc);
    void append_space();
};

#endif
