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
#include "plugins.h"
#include "sceneobject.h"

static void editor_class_init   (EditorClass *klass);
static void editor_init         (Editor      *editor);
static void editor_dispose      (GObject     *object);

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

static void object_selection_changed (GtkTreeSelection *selection, gpointer data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
    SceneObject *object;
    gtk_tree_model_get (model, &iter, 3, &object, -1);
    scene_object_select (object);
  }
}

static void
editor_selected (SceneObject *object, Editor *editor)
{
  GtkWidget *viewport = glade_xml_get_widget (editor->xml, "property editor viewport");

  if (editor->selected)
  {
    scene_object_deselect (editor->selected);
    gtk_widget_destroy (editor->pe);
  }

  editor->pe = parameter_editor_new (PARAMETER_HOLDER (object));
  gtk_container_add (GTK_CONTAINER (viewport), editor->pe);
  gtk_widget_show_all (viewport);

  editor->selected = object;
}

static void
object_create_toolbar (GtkToolButton *button, Editor *editor)
{
  GType type = GPOINTER_TO_UINT (g_object_get_data (G_OBJECT(button), "create-type"));
  SceneObject *object = SCENE_OBJECT (g_object_new (type, NULL));
  scene_add (editor->scene, object);
  g_signal_connect (G_OBJECT (object), "selected", G_CALLBACK (editor_selected), (gpointer) editor);
  scene_object_select (object);
}

static gboolean
tree_model_row_draggable (GtkTreeDragSource *drag_source, GtkTreePath *source)
{
  GtkTreeModel *model = GTK_TREE_MODEL (drag_source);
  GtkTreeIter iter;
  SceneObject *object;
  SceneObjectClass *klass;

  gtk_tree_model_get_iter (model, &iter, source);
  gtk_tree_model_get (model, &iter, 3, &object, -1);
  klass = SCENE_OBJECT_CLASS (G_OBJECT_GET_CLASS (object));
  return (klass->parentable);
}

static gboolean
tree_model_drag_data_get (GtkTreeDragSource *drag_source, GtkTreePath *path, GtkSelectionData *selection_data)
{
  if (gtk_tree_set_row_drag_data (selection_data, GTK_TREE_MODEL (drag_source), path))
  {
    g_print ("filled data!\n");
    return TRUE;
  }
  else
  {
    g_print ("failed to fill data\n");
    return FALSE;
  }
}

static gboolean
tree_model_drag_data_delete (GtkTreeDragSource *drag_source, GtkTreePath *path)
{
  g_print ("drag_data_delete ()\n");
  return TRUE;
}

static gboolean
tree_model_drag_data_received (GtkTreeDragDest *drag_dest, GtkTreePath *dest, GtkSelectionData *selection_data)
{
  g_print ("drag_data_received ()\n");
  return FALSE;
}

static gboolean
tree_model_row_drop_possible (GtkTreeDragDest *drag_dest, GtkTreePath *dest, GtkSelectionData *selection_data)
{
  g_print ("row_drop_possible ()\n");
  return TRUE;
}

static void
editor_init (Editor *editor)
{
  GdkGLConfig *config;
  GtkCellRenderer *icon_renderer, *text_renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;
  GtkMenuItem *addmenu;
  GtkMenu *am;
  GtkToolbar *tbar;
  ParameterHolder *p;
  GtkWidget *e, *v, *l;
  GList *types, *t;
  GtkTargetEntry targets =
  {
    "reordering",
    GTK_TARGET_SAME_WIDGET,
    0
  };

  load_plugins();

  editor->xml = glade_xml_new (GLADEDIR "/bzedit.glade", NULL, NULL);
  editor->window = glade_xml_get_widget (editor->xml, "editor window");

  config = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGBA  |
                                      GDK_GL_MODE_DEPTH |
                                      GDK_GL_MODE_DOUBLE);

  editor->glarea = gl_drawing_area_new(config);
  editor->eventbox = glade_xml_get_widget (editor->xml, "event box");

  editor->scene = scene_new ();

  editor->view = VIEW (view_new (editor->scene, GL_DRAWING_AREA (editor->glarea)));
  gtk_container_add (GTK_CONTAINER (editor->eventbox), GTK_WIDGET (editor->view));

  editor->element_list = GTK_TREE_VIEW (glade_xml_get_widget (editor->xml, "element list"));
  gtk_tree_view_set_model (editor->element_list, GTK_TREE_MODEL (editor->scene->element_store));

  select = gtk_tree_view_get_selection (editor->element_list);
  gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT (select), "changed", G_CALLBACK (object_selection_changed), NULL);

  gtk_tree_view_enable_model_drag_source (editor->element_list, GDK_BUTTON1_MASK, &targets, 1, GDK_ACTION_MOVE);
  gtk_tree_view_enable_model_drag_dest   (editor->element_list, &targets, 1, GDK_ACTION_MOVE);
  GTK_TREE_DRAG_SOURCE_GET_IFACE (editor->scene->element_store)->row_draggable = tree_model_row_draggable;
//  GTK_TREE_DRAG_SOURCE_GET_IFACE (editor->scene->element_store)->drag_data_get = tree_model_drag_data_get;
//  GTK_TREE_DRAG_SOURCE_GET_IFACE (editor->scene->element_store)->drag_data_delete = tree_model_drag_data_delete;
  GTK_TREE_DRAG_DEST_GET_IFACE (editor->scene->element_store)->row_drop_possible = tree_model_row_drop_possible;
  GTK_TREE_DRAG_DEST_GET_IFACE (editor->scene->element_store)->drag_data_received = tree_model_drag_data_received;

  icon_renderer = gtk_cell_renderer_pixbuf_new();
  text_renderer = gtk_cell_renderer_text_new();
  g_object_set (G_OBJECT (text_renderer), "editable", TRUE, NULL);
  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_spacing (column, 4);
  gtk_tree_view_column_pack_start (column, icon_renderer, FALSE);
  gtk_tree_view_column_set_attributes (column, icon_renderer, "pixbuf", 1, NULL);
  gtk_tree_view_column_pack_start (column, text_renderer, TRUE);
  gtk_tree_view_column_set_attributes (column, text_renderer, "text", 0, NULL);
  gtk_tree_view_append_column (editor->element_list, column);

  editor->statusbar = GTK_STATUSBAR (glade_xml_get_widget (editor->xml, "statusbar"));
  editor->editor_status_context = gtk_statusbar_get_context_id (editor->statusbar, "Editor status");

  editor->selected = NULL;

  types = find_type_leaves (PARAMETER_HOLDER_TYPE);
  addmenu = GTK_MENU_ITEM (glade_xml_get_widget (editor->xml, "addmenu"));
  am = GTK_MENU (gtk_menu_new ());
  tbar = GTK_TOOLBAR (glade_xml_get_widget (editor->xml, "object toolbar"));
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
	g_object_set_data (G_OBJECT (item), "create-type", t->data);
	g_object_set_data (G_OBJECT (titem), "create-type", t->data);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), image1);
        gtk_menu_shell_append (GTK_MENU_SHELL (am), GTK_WIDGET (item));
        gtk_toolbar_insert (tbar, titem, -1);
	g_signal_connect (G_OBJECT (titem), "clicked", G_CALLBACK (object_create_toolbar), (gpointer) editor);
      }
      if (klass->autocreate)
      {
        SceneObject *object = SCENE_OBJECT (g_object_new (type, NULL));
        scene_add (editor->scene, object);
	g_signal_connect (G_OBJECT (object), "selected", G_CALLBACK (editor_selected), (gpointer) editor);
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
