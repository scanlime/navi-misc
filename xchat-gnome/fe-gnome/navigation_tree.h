/*
 * navtree.h - functions to create and maintain the navigation tree
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

#include <gnome.h>
#include "../common/xchat.h"

#ifndef __XCHAT_GNOME_NAVTREE_H__
#define __XCHAT_GNOME_NAVTREE_H__

G_BEGIN_DECLS

typedef struct _NavTree       NavTree;
typedef struct _NavTreeClass  NavTreeClass;
typedef struct _NavModel      NavModel;
typedef struct _NavModelClass NavModelClass;
/***** NavTree *****/
#define NAVTREE_TYPE            (navigation_tree_get_type ())
#define NAVTREE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAVTREE_TYPE, NavTree))
#define NAVTREE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAVTREE_TYPE, NavTreeClass))
#define IS_NAVTREE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAVTREE_TYPE))
#define IS_NAVTREE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVTREE_TYPE))

struct _NavTree
{
  GtkTreeView parent;
	/* current_path stores a GtkTreePath to the most recently selected
	 * item in the NavTree.
	 */
  GtkTreePath *current_path;
	/* This is the model for the GtkTreeView. */
  NavModel *model;
	/* We need the handler id for the selection_changed call back so that we
	 * can block it sometimes.
	 */
  gulong selection_changed_id;
};

struct _NavTreeClass
{
  GtkTreeViewClass parent_class;
};

GType navigation_tree_get_type (void) G_GNUC_CONST;
NavTree* navigation_tree_new   (NavModel *model);

/* Add/remove server/channel functions. */
void navigation_tree_create_new_network_entry (NavTree *navtree, struct session *sess);
void navigation_tree_create_new_channel_entry (NavTree *navtree, struct session *sess);
void navigation_tree_remove                   (NavTree *navtree, struct session *sess);

/* Channel/server selection functions. */
void navigation_tree_select_nth_channel  (NavTree *navtree, gint chan_num);
/* FIXME: Not done. */
void navigation_tree_select_session      (NavTree *navtree, struct session *sess);
void navigation_tree_select_next_channel (NavTree *navtree);
void navigation_tree_select_prev_channel (NavTree *navtree);
void navigation_tree_select_next_network (NavTree *navtree);
void navigation_tree_select_prev_network (NavTree *navtree);

/* Misc. functions. */
void navigation_tree_set_channel_name (NavTree *navtree, struct session *sess);

/***** NavModel *****/
#define NAVMODEL_TYPE            (navigation_model_get_type ())
#define NAVMODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAVMODEL_TYPE, NavModel))
#define NAVMODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAVMODEL_TYPE, NavModelClass))
#define IS_NAVMODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAVMODEL_TYPE))
#define IS_NAVMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVMODEL_TYPE))

struct _NavModel
{
  GObject parent;
  GtkTreeModel *sorted;
  GtkTreeStore *store;
	/* Stores an iter for each session in the model. These iters are for the unsorted
	 * store.
	 */
  GHashTable *session_iters;
};

struct _NavModelClass
{
  GObjectClass parent;
};

GType navigation_model_get_type (void) G_GNUC_CONST;
NavModel* navigation_model_new  (void);

/* Add/remove server/channel functions. */
void navigation_model_add_new_network  (NavModel *model, struct session *sess);
void navigation_model_add_new_channel  (NavModel *model, struct session *sess);
void navigation_model_remove           (NavModel *model, struct session *sess);

void navigation_model_set_disconn      (NavModel *model, struct session *sess);
void navigation_model_set_hilight      (NavModel *model, struct session *sess);

/* Ref counting for the selected items in the model. */
/* Find the row by path. */
void navigation_model_path_ref         (NavModel *model, GtkTreePath *path);
void navigation_model_path_deref       (NavModel *model, GtkTreePath *path);
/* Find the row by iter. XXX: The iter becomes invalid after a call to this
 * function, make sure if you're going to use it again that you re-get the
 * iter.
 */
void navigation_model_sorted_iter_ref   (NavModel *model, GtkTreeIter *iter);
void navigation_model_sorted_iter_unref (NavModel *model, GtkTreeIter *iter);

/* Returns a GtkTreeIter* to the row containing sess. The iter is for the
 * GtkTreeModelSort, not the unsorted TreeStore.
 */
GtkTreeIter* navigation_model_get_iter (NavModel *model, struct session *sess);

G_END_DECLS

#endif
