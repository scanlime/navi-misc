#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <vector>
#include <gnome.h>
#include <gdl/gdl-dock.h>
#include <gdl/gdl-dock-layout.h>
#include <gdl/gdl-dock-master.h>
#include "toolbar.h"
#include "editboxes.h"
#include "listbox.h"
#include "viewbox.h"
#include "preferences.h"

class App
{
  public:
    App();

    static gint on_file_new(GtkWidget *widget, gpointer data);
    static gint on_file_open(GtkWidget *widget, gpointer data);
    static gint on_file_save(GtkWidget *widget, gpointer data);
    static gint on_file_save_as(GtkWidget *widget, gpointer data);
    static gint on_file_exit(GtkWidget *widget, gpointer data);

    static gint on_edit_cut(GtkWidget *widget, gpointer data);
    static gint on_edit_copy(GtkWidget *widget, gpointer data);
    static gint on_edit_paste(GtkWidget *widget, gpointer data);
    static gint on_edit_select_all(GtkWidget *widget, gpointer data);
    static gint on_edit_clear(GtkWidget *widget, gpointer data);
    static gint on_edit_undo(GtkWidget *widget, gpointer data);
    static gint on_edit_redo(GtkWidget *widget, gpointer data);
    static gint on_edit_properties(GtkWidget *widget, gpointer data);

    static gint on_view_new_view(GtkWidget *widget, gpointer data);
    static gint on_view_movetosel(GtkWidget *widget, gpointer data);

    static gint on_util_convtod(GtkWidget *widget, gpointer data);
    static gint on_util_convtor(GtkWidget *widget, gpointer data);

    static gint on_settings_preferences(GtkWidget *widget, gpointer data);

    static gint on_help_about(GtkWidget *widget, gpointer data);

    static void on_dock_layout_changed(GdlDockLayout *layout, GParamSpec *pspec, gpointer data);

  protected:
    void		build_menus();
    GtkWidget		*create_ui_preferences();

    GtkWidget		*app;
    Toolbar		toolbar;
    GtkWidget		*dock;
    GdlDockLayout	*layout;
    GtkWidget		*statusbar;

    Preferences		prefbox;
    EditBox		position;
    EditBox		scale;
    EditBox		rotation;
    EditBox		other;
    ListBox		list;
    ViewBox		view;

    std::vector<ViewBox*>	boxes;
};

#endif
