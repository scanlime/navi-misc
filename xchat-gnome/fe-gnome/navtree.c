/*
 * navtree.c - functions to create and maintain the navigation tree
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#include "navtree.h"

/***** NavTree *****/
static void navigation_tree_init       (NavTree *navtree);
static void navigation_tree_class_init (NavTreeClass *klass);
static void navigation_tree_dispose    (GObject *object);
static void navigation_tree_finalize   (GObject *object);

void navigation_selection_changed (GtkTreeSelection *treeselection, gpointer user_data);
static gboolean click             (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
static gboolean declick           (GtkWidget *widget, GdkEventButton *event, gpointer user_data);

GType
navigation_tree_get_type (void)
{
  static GType navigation_tree_type = 0;
  if (!navigation_tree_type) {
    static const GTypeInfo navigation_tree_info =
    {
      sizeof (NavTreeClass),
      NULL,
      NULL,
      (GClassInitFunc) navigation_tree_class_init,
      NULL,
      NULL,
      sizeof (NavTree),
      0,
      (GInstanceInitFunc) navigation_tree_init,
    };

    navigation_tree_type = g_type_register_static(G_TYPE_OBJECT, "NavTree", &navigation_tree_info, 0);
  }

  return navigation_tree_type;
}

static void
navigation_tree_init (NavTree *navtree)
{
	navtree->current_path = gtk_tree_path_new();
	navtree->model = NULL;
}

static void
navigation_tree_class_init (NavTreeClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;

	object_class->dispose = navigation_tree_dispose;
	object_class->finalize = navigation_tree_finalize;
}

static void
navigation_tree_dispose (GObject *object)
{
	NavTree *navtree = (NavTree*) object;
	if (navtree->model) {
	  g_object_unref((gpointer)navtree->model);
		navtree->model = NULL;
	}
}

static void
navigation_tree_finalize (GObject *object)
{
	NavTree *navtree = (NavTree*) object;
	gtk_tree_path_free(navtree->current_path);
}

/* New NavTree. */
NavTree*
navigation_tree_new (NavModel *model)
{
	NavTree *new_tree;
  GtkCellRenderer *icon_renderer, *text_renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *select;

	new_tree = NAVTREE(g_object_new(navigation_tree_get_type(), NULL));
	new_tree->model = model;
  gtk_tree_view_set_model(GTK_TREE_VIEW(new_tree), new_tree->model->sorted);

  column = gtk_tree_view_column_new();
  icon_renderer = gtk_cell_renderer_pixbuf_new();
  text_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(column, icon_renderer, FALSE);
  gtk_tree_view_column_set_attributes(column, icon_renderer, "pixbuf", 0, NULL);
  gtk_tree_view_column_pack_start(column, text_renderer, TRUE);
  gtk_tree_view_column_set_attributes(column, text_renderer, "text", 1, "foreground-gdk", 4, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(new_tree),column);

  select = gtk_tree_view_get_selection(GTK_TREE_VIEW(new_tree));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
/*
  g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(navigation_selection_changed), NULL);
  g_signal_connect(G_OBJECT(new_tree), "button_press_event", G_CALLBACK(click), NULL);
  g_signal_connect(G_OBJECT(new_tree), "button_release_event", G_CALLBACK(declick), NULL);*/

	return new_tree;
}

/* Add/remove server/channel functions. */

/* Channel/server selection functions. */

/***** NavModel *****/

static void navigation_model_init       (NavModel *navmodel);
static void navigation_model_class_init (NavModelClass *klass);
static void navigation_model_dispose    (GObject *object);

GType
navigation_model_get_type (void)
{
  static GType navigation_model_type = 0;
  if (!navigation_model_type) {
    static const GTypeInfo navigation_model_info =
    {
      sizeof (NavTreeClass),
      NULL,
      NULL,
      (GClassInitFunc) navigation_model_class_init,
      NULL,
      NULL,
      sizeof(NavModel),
      0,
      (GInstanceInitFunc) navigation_model_init,
    };

    navigation_model_type = g_type_register_static(G_TYPE_OBJECT, "NavModel", &navigation_model_info, 0);
  }

  return navigation_model_type;
}

static void
navigation_model_init (NavModel *navmodel)
{
  navmodel->store = gtk_tree_store_new(5, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_INT, GDK_TYPE_COLOR);
	navmodel->sorted = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(navmodel->store));
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(navmodel->sorted), 1, GTK_SORT_ASCENDING);
}

static void
navigation_model_class_init (NavModelClass *klass)
{
	GObjectClass *object_class = (GObjectClass*) klass;
	object_class->dispose = navigation_model_dispose;
}

static void
navigation_model_dispose (GObject *object)
{
  NavModel *navmodel = NAVMODEL(object);
  g_object_unref((gpointer)navmodel->store);
	g_object_unref((gpointer)navmodel->sorted);
}

/* New NavModel. */
NavModel*
navigation_model_new ()
{
	return NAVMODEL(g_object_new(navigation_model_get_type(), NULL));
}
