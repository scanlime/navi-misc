#include "application.h"
#include "globals.h"
#include <iostream>

App::App() : app(gnome_app_new("BZEdit", "BZFlag Editor")), prefbox(app), position("position"), scale("scale"), rotation("rotation"), other("other")
{
  app = gnome_app_new("BZEdit", "BZFlag Editor");
  dock = gdl_dock_new();

  gnome_app_set_contents(GNOME_APP(app), dock);
  gnome_app_add_toolbar(GNOME_APP(app), GTK_TOOLBAR(toolbar.getWidget()), "main.toolbar", BONOBO_DOCK_ITEM_BEH_EXCLUSIVE, BONOBO_DOCK_TOP, 1, 0, 0);
  gtk_signal_connect(GTK_OBJECT(app), "delete_event", GTK_SIGNAL_FUNC(on_file_exit), this);
  gtk_widget_set_size_request(app, 620, 460);
  statusbar = gnome_appbar_new(FALSE, TRUE, GNOME_PREFERENCES_USER);
  gnome_app_set_statusbar(GNOME_APP(app), statusbar);
  build_menus();

  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(list.getWidget()), GDL_DOCK_LEFT);
  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(position.getWidget()), GDL_DOCK_BOTTOM);
  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(scale.getWidget()), GDL_DOCK_BOTTOM);
  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(rotation.getWidget()), GDL_DOCK_BOTTOM);
  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(other.getWidget()), GDL_DOCK_BOTTOM);
  gdl_dock_add_item(GDL_DOCK(dock), GDL_DOCK_ITEM(view.getWidget()), GDL_DOCK_RIGHT);

  layout = gdl_dock_layout_new(GDL_DOCK(dock));
  gdl_dock_layout_load_from_file(layout, "layout.xml");
  gdl_dock_layout_load_layout(layout, NULL);
  g_signal_connect(G_OBJECT(layout), "notify::dirty", G_CALLBACK(on_dock_layout_changed), layout);

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("data/layout-manager.png", NULL);
  prefbox.add_page("User Interface", pixbuf, create_ui_preferences());

  gtk_widget_show_all(app);

  view.readied();
  view.make_current();
  doc->init();
}

void App::build_menus()
{
  static GnomeUIInfo file_menu[] = {
    GNOMEUIINFO_MENU_NEW_ITEM("New", "Create a new BZFlag world", on_file_new, this),
    GNOMEUIINFO_MENU_OPEN_ITEM(on_file_open, this),
    GNOMEUIINFO_MENU_SAVE_ITEM(on_file_save, this),
    GNOMEUIINFO_MENU_SAVE_AS_ITEM(on_file_save_as, this),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_EXIT_ITEM(on_file_exit, this),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo edit_menu[] = {
    GNOMEUIINFO_MENU_CUT_ITEM(on_edit_cut, this),
    GNOMEUIINFO_MENU_COPY_ITEM(on_edit_copy, this),
    GNOMEUIINFO_MENU_PASTE_ITEM(on_edit_paste, this),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_SELECT_ALL_ITEM(on_edit_select_all, this),
    GNOMEUIINFO_MENU_CLEAR_ITEM(on_edit_clear, this),
    GNOMEUIINFO_MENU_UNDO_ITEM(on_edit_undo, this),
    GNOMEUIINFO_MENU_REDO_ITEM(on_edit_redo, this),
    GNOMEUIINFO_SEPARATOR,
    GNOMEUIINFO_MENU_PROPERTIES_ITEM(on_edit_properties, this),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo view_menu[] = {
    GNOMEUIINFO_ITEM_DATA("New view", "Create a new view window", on_view_new_view, this, NULL),
    GNOMEUIINFO_ITEM_NONE("Move to selection", "Moves the focus to the current selection", on_view_movetosel),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo utility_menu[] = {
    GNOMEUIINFO_ITEM_NONE("Convert to degrees", "Converts the angles in the map to degrees", on_util_convtod),
    GNOMEUIINFO_ITEM_NONE("Convert to radians", "Converts the angles in the map to radians", on_util_convtor),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo settings_menu[] = {
    GNOMEUIINFO_MENU_PREFERENCES_ITEM(on_settings_preferences, this),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo help_menu[] = {
    GNOMEUIINFO_MENU_ABOUT_ITEM(on_help_about, this),
    GNOMEUIINFO_END
  };
  static GnomeUIInfo main_menu[] = {
    GNOMEUIINFO_MENU_FILE_TREE(file_menu),
    GNOMEUIINFO_MENU_EDIT_TREE(edit_menu),
    GNOMEUIINFO_MENU_VIEW_TREE(view_menu),
    GNOMEUIINFO_SUBTREE("Utility", utility_menu),
    GNOMEUIINFO_MENU_SETTINGS_TREE(settings_menu),
    GNOMEUIINFO_MENU_HELP_TREE(help_menu),
    GNOMEUIINFO_END
  };
  gnome_app_create_menus(GNOME_APP(app), main_menu);
  gnome_app_install_menu_hints(GNOME_APP(app), main_menu);
}

GtkWidget *App::create_ui_preferences()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  GtkWidget *label = gtk_label_new("Configure the appearance and layout of the user interface");
  GtkWidget *notebook = gtk_notebook_new();
  GtkWidget *page = gdl_dock_layout_get_items_ui(layout);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("Dock Items"));
  gtk_widget_show(page);
  page = gdl_dock_layout_get_layouts_ui(layout);
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("Dock Layouts"));
  gtk_widget_show(page);
  gtk_widget_show (notebook);
  gtk_widget_show(label);
  gtk_widget_show(vbox);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
  return vbox;
}

gint App::on_file_new(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_file_open(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_file_save(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_file_save_as(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_file_exit(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
  return FALSE;
}

gint App::on_edit_cut(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_copy(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_paste(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_select_all(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_clear(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_undo(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_redo(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_edit_properties(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_view_new_view(GtkWidget *widget, gpointer data)
{
  App *app = static_cast<App *>(data);
  ViewBox *box = new ViewBox(app->view);
  app->boxes.push_back(box);
  gdl_dock_add_floating_item(GDL_DOCK(app->dock), GDL_DOCK_ITEM(box->getWidget()), 100, 100, 640, 480);
  box->readied();
  return TRUE;
}

gint App::on_view_movetosel(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_util_convtod(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_util_convtor(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

gint App::on_settings_preferences(GtkWidget *widget, gpointer data)
{
  App *app = static_cast<App *>(data);
  app->prefbox.show();
  return TRUE;
}

gint App::on_help_about(GtkWidget *widget, gpointer data)
{
  return TRUE;
}

void App::on_dock_layout_changed(GdlDockLayout *layout, GParamSpec *pspec, gpointer data)
{
  if(!strcmp(pspec->name, "dirty")) {
    gboolean dirty;
    g_object_get(G_OBJECT(layout), "dirty", &dirty, NULL);
    if (dirty) {
      gdl_dock_layout_save_to_file(layout, "layout.xml");
    }
  }
}
