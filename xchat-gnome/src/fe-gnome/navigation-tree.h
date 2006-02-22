/*
 * navigation-tree.h - functions to create and maintain the navigation tree
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include <gtk/gtk.h>
#include "../common/xchat.h"
#include "../common/servlist.h"

#ifndef __XCHAT_GNOME_NAVTREE_H__
#define __XCHAT_GNOME_NAVTREE_H__

G_BEGIN_DECLS

typedef struct _NavTree NavTree;
typedef struct _NavTreeClass NavTreeClass;
typedef struct _NavModel NavModel;
typedef struct _NavModelClass NavModelClass;

/***** NavTree *****/
#define NAVTREE_TYPE            (navigation_tree_get_type ())
#define NAVTREE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAVTREE_TYPE, NavTree))
#define NAVTREE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAVTREE_TYPE, NavTreeClass))
#define IS_NAVTREE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAVTREE_TYPE))
#define IS_NAVTREE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVTREE_TYPE))

struct _NavTree
{
	GtkTreeView          parent;

	NavModel            *model;

	/* current_rowref stores a GtkTreeRowReference to the most recently selected
	 * item in the NavTree.
 	 */
	GtkTreeRowReference  *current_rowref;

	/* We need the handler id for the selection_changed call back so that we
	 * can block it sometimes.
	 */
	gulong               selection_changed_id;

	/* Store row references for the last server and the last channel in the
	 * sorted model. It's cheaper to maintain these references than to
	 * recalculate it everytime we're trying to wrap in
	 * navigation_tree_prev_channel.
	 */
	GtkTreeRowReference *last_server;
	GtkTreeRowReference *last_channel;
};

struct _NavTreeClass
{
    GtkTreeViewClass parent_class;
};

GType
navigation_tree_get_type (void)
    G_GNUC_CONST;
     NavTree *navigation_tree_new (NavModel *model);

/* Add/remove server/channel functions. */
void navigation_tree_create_new_network_entry (NavTree *navtree, struct session *sess);
void navigation_tree_create_new_channel_entry (NavTree *navtree, struct session *sess);
void navigation_tree_remove_channel           (NavTree *navtree, struct session *sess);
void navigation_tree_remove_server            (NavTree *navtree, struct session *sess);
void navigation_tree_server_rm_chans          (NavTree *navtree, GtkTreeIter *parent);

/* Channel/server selection functions. */
void navigation_tree_select_nth_channel       (NavTree *navtree, gint chan_num);
void navigation_tree_select_session           (NavTree *navtree, struct session *sess);
void navigation_tree_select_next_channel      (NavTree *navtree);
void navigation_tree_select_prev_channel      (NavTree *navtree);
void navigation_tree_select_next_network      (NavTree *navtree);
void navigation_tree_select_prev_network      (NavTree *navtree);

/* Misc. functions. */
void navigation_tree_set_channel_name         (NavTree *navtree, struct session *sess);
gboolean navigation_tree_server_is_connected  (NavTree *navtree, gchar *name);
NavTree *get_navigation_tree                  ();


/***** NavModel *****/
#define NAVMODEL_TYPE            (navigation_model_get_type ())
#define NAVMODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAVMODEL_TYPE, NavModel))
#define NAVMODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAVMODEL_TYPE, NavModelClass))
#define IS_NAVMODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAVMODEL_TYPE))
#define IS_NAVMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVMODEL_TYPE))

struct _NavModel
{
    GObject			parent;
    GtkTreeModel*		sorted;
    GtkTreeStore*		store;

    /* Stores an iter for each session in the model. These iters are for the unsorted
     * store.
     */
    GHashTable*			session_rows;

    /* Number of servers we're connected to. */
    gint			servers;
};

struct _NavModelClass
{
    GObjectClass parent;
};

GType navigation_model_get_type (void) G_GNUC_CONST;
NavModel *navigation_model_new (void);

/* Add/remove server/channel functions. */
void navigation_model_add_new_network           (NavModel *model, struct session *sess);
void navigation_model_add_new_channel           (NavModel *model, struct session *sess);
void navigation_model_remove                    (NavModel *model, struct session *sess);

void navigation_model_set_disconn               (NavModel *model, struct session *sess);
void navigation_model_set_hilight               (NavModel *model, struct session *sess);

/* Ref counting for the selected items in the model. */
/* Find the row by path. */
void navigation_model_rowref_ref                  (GtkTreeRowReference *rowref);
void navigation_model_rowref_deref                (GtkTreeRowReference *rowref);

/* Find the row by iter. XXX: The iter becomes invalid after a call to this
 * function, make sure if you're going to use it again that you re-get the
 * iter.
 */
void navigation_model_sorted_iter_ref           (NavModel *model, GtkTreeIter *iter);
void navigation_model_sorted_iter_unref         (NavModel *model, GtkTreeIter *iter);

/* Returns a GtkTreeIter* to the row containing sess. The iter is for the
 * GtkTreeModelSort, not the unsorted TreeStore.
 */
GtkTreeIter *navigation_model_get_sorted_iter   (NavModel *model, struct session *sess);
GtkTreeIter *navigation_model_get_unsorted_iter (NavModel *model, struct session *sess);

/* Convenience function to find the server* which matches an ircnet*, assuming that
 * the network in question is connected to.  We need this in order to do things like
 * change character encoding at runtime */
struct server *navigation_model_get_server      (NavModel *model, ircnet *network);

G_END_DECLS

#endif
