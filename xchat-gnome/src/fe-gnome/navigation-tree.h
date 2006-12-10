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
typedef struct _NavTreePriv NavTreePriv;
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
	GtkTreeView parent;

	NavTreePriv *priv;
};

struct _NavTreeClass
{
	GtkTreeViewClass parent_class;
};

GType    navigation_tree_get_type       (void) G_GNUC_CONST;
NavTree *navigation_tree_new            (NavModel *model);
void     navigation_tree_select_session (NavTree *tree, session *sess);
void     navigation_tree_remove_session (NavTree *tree, session *sess);
void     navigation_tree_add_accels     (NavTree *navtree, GtkWindow *window);


/***** NavModel *****/
#define NAVMODEL_TYPE            (navigation_model_get_type ())
#define NAVMODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAVMODEL_TYPE, NavModel))
#define NAVMODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NAVMODEL_TYPE, NavModelClass))
#define IS_NAVMODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAVMODEL_TYPE))
#define IS_NAVMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVMODEL_TYPE))

struct _NavModel
{
	GtkTreeStore parent;
};

struct _NavModelClass
{
	GtkTreeStoreClass parent;
};

GType     navigation_model_get_type         (void) G_GNUC_CONST;
NavModel *navigation_model_new              (void);

void      navigation_model_add_server       (NavModel *model, session *sess);
void      navigation_model_add_channel      (NavModel *model, session *sess);
void      navigation_model_update           (NavModel *model, session *sess);
void      navigation_model_set_disconnected (NavModel *model, session *sess);
void      navigation_model_set_hilight      (NavModel *model, session *sess);
void      navigation_model_set_current      (NavModel *model, session *sess);

G_END_DECLS

#endif
