/*
 * editor.c - Automatically constructs a GUI for editing the
 *            parameters of a ParameterHolder instance.
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <GL/gl.h>
#include "editor.h"
#include "gldrawingarea.h"
#include "parameter-editor.h"
#include "plugins.h"
#include "sceneobject.h"

static void editor_class_init   (EditorClass *klass);
static void editor_init         (Editor      *editor);
static void editor_dispose      (GObject     *object);

static void on_glarea_realize   (GtkWidget *widget, gpointer data);

GType
editor_get_type (void)
{
  static GType editor_type = 0;

  if (!editor_type)
    {
      static const GTypeInfo editor_info =
      {
	sizeof (EditorClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    editor_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Editor),
	0,                  /* n preallocs */
	(GInstanceInitFunc) editor_init,
      };

      editor_type = g_type_register_static (G_TYPE_OBJECT, "Editor", &editor_info, 0);
    }

  return editor_type;
}

static void
editor_class_init (EditorClass *klass)
{
  GObjectClass *object_class = (GObjectClass*) klass;

  object_class->dispose = editor_dispose;

  glade_init();
}


static void
editor_init (Editor *editor)
{
  GdkGLConfig *config;
  GtkCellRenderer *icon_renderer, *text_renderer;
  GtkTreeViewColumn *column;
  GtkMenuItem *addmenu;
  GtkMenu *am;
  GtkScrolledWindow *swin;
  GtkToolbar *tbar;
  GtkBox *box;
  ParameterHolder *p;
  GtkWidget *e, *v, *l;
  GList *types, *t;

  load_plugins();

  editor->xml = glade_xml_new (GLADEDIR "/bzedit.glade", NULL, NULL);
  editor->window = glade_xml_get_widget (editor->xml, "editor window");

  config = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGBA  |
                                      GDK_GL_MODE_DEPTH |
                                      GDK_GL_MODE_DOUBLE);

  editor->glarea = gl_drawing_area_new(config);
  g_signal_connect_after(G_OBJECT(editor->glarea), "realize", G_CALLBACK(on_glarea_realize), (gpointer) editor);
  editor->eventbox = glade_xml_get_widget (editor->xml, "event box");
  gtk_container_add (GTK_CONTAINER (editor->eventbox), editor->glarea);

  editor->element_list = GTK_TREE_VIEW (glade_xml_get_widget (editor->xml, "element list"));
  editor->element_store = gtk_tree_store_new(5, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_POINTER, G_TYPE_BOOLEAN, GDK_TYPE_PIXBUF);
  gtk_tree_view_set_model (editor->element_list, GTK_TREE_MODEL (editor->element_store));

  icon_renderer = gtk_cell_renderer_pixbuf_new();
  text_renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new();
  gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
  gtk_tree_view_column_pack_start (column, text_renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, icon_renderer, "pixbuf", 1, NULL);
  gtk_tree_view_column_set_attributes (column, text_renderer, "text", 0, "editable", TRUE, NULL);
  gtk_tree_view_append_column (editor->element_list, column);

  editor->statusbar = GTK_STATUSBAR (glade_xml_get_widget (editor->xml, "statusbar"));
  editor->editor_status_context = gtk_statusbar_get_context_id (editor->statusbar, "Editor status");

  editor->scene = scene_new ();

  types = find_type_leaves (PARAMETER_HOLDER_TYPE);
  addmenu = GTK_MENU_ITEM (glade_xml_get_widget (editor->xml, "addmenu"));
  am = GTK_MENU (gtk_menu_new ());
  box = GTK_BOX (gtk_vbox_new (0, 6));
  swin = GTK_SCROLLED_WINDOW (glade_xml_get_widget (editor->xml, "property editor swin"));
  tbar = GTK_TOOLBAR (glade_xml_get_widget (editor->xml, "object toolbar"));
  gtk_scrolled_window_add_with_viewport (swin, GTK_WIDGET (box));
  for (t = types; t; t = t->next)
  {
    GType type = GPOINTER_TO_UINT (t->data);
    if (type != 0)
    {
      SceneObjectClass *klass = SCENE_OBJECT_CLASS (g_type_class_ref (type));
      if (klass->creatable)
      {
        GtkMenuItem *item = GTK_MENU_ITEM (gtk_image_menu_item_new_with_label (g_type_name (type)));
        GtkWidget *image1 = gtk_image_new_from_pixbuf (((SceneObjectClass*) klass)->get_icon ());
        GtkWidget *image2 = gtk_image_new_from_pixbuf (((SceneObjectClass*) klass)->get_icon ());
        GtkToolItem *titem = GTK_TOOL_ITEM (gtk_tool_button_new (image2, g_type_name (type)));
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image1);
        gtk_menu_shell_append (GTK_MENU_SHELL (am), GTK_WIDGET (item));
        gtk_toolbar_insert (tbar, titem, -1);
      }
      if (klass->autocreate)
      {
        v = gtk_hseparator_new();
        gtk_box_pack_start (box, v, FALSE, TRUE, 0);
        l = gtk_label_new (g_type_name (type));
        gtk_box_pack_start (box, l, FALSE, TRUE, 0);
        v = gtk_hseparator_new();
        gtk_box_pack_start (box, v, FALSE, TRUE, 0);
        SceneObject *object = SCENE_OBJECT (g_object_new (type, NULL));
        p = PARAMETER_HOLDER (object);
        scene_add (editor->scene, object);
        e = parameter_editor_new (p);
        gtk_box_pack_start (box, e, FALSE, TRUE, 0);
      }
      g_type_class_unref (klass);
    }
  }
  gtk_menu_item_set_submenu (addmenu, GTK_WIDGET (am));

  g_list_free (types);
}

static void
editor_dispose (GObject *object)
{
}

Editor*
editor_new (void)
{
  Editor *self = EDITOR (g_object_new (editor_get_type(), NULL));

  gtk_widget_show_all(self->window);

  return self;
}

static void
on_glarea_realize (GtkWidget *widget, gpointer data)
{
  Editor *editor = EDITOR (data);

  editor->view = view_new (editor->scene, GL_DRAWING_AREA (widget));

  editor->view->camera->azimuth = 45;
  editor->view->camera->elevation = 25;
  editor->view->camera->distance = 900;
  editor->view->camera->position[2] = 4;
}
