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

#define NAVTREE_TYPE 						(navigation_tree_get_type())
#define NAVTREE(obj)						(G_TYPE_CHECK_INSTANCE_CAST((obj), NAVTREE_TYPE, NavTree))
#define NAVTREE_CLASS(klass)					(G_TYPE_CHECK_CLASS_CAST((klass), NAVTREE_TYPE, NavTreeClass))
#define IS_NAVTREE(obj)					(G_TYPE_CHECK_INSTANCE_TYPE((obj), NAVTREE_TYPE))
#define IS_NAVTREE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), NAVTREE_TYPE))

/***** NavTree *****/
typedef struct _NavTree			NavTree;
typedef struct _NavTreeClass NavTreeClass;

struct _NavTree
{
	GtkTreeView parent;
	GtkTreePath current_path;
};

struct _NavTreeClass
{
	/* FIXME: Is this right? */
	GObjectClass parent_class;
};


/***** NavModel *****/
#define NAVMODEL_TYPE						 (navigation_model_get_type())
#define NAVMODEL(obj)						 (G_TYPE_CHECK_INSTANCE_CAST((obj), NAVMODEL_TYPE, NavModel))
#define NAVMODEL_CLASS(klass)		 (G_TYPE_CHECK_CLASS_CAST((klass), NAVMODEL_TYPE, NavModelClass))
#define IS_NAVMODEL(obj)				 (G_TYPE_CHECK_INSTANCE_TYPE((obj), NAVMODEL_TYPE))
#define IS_NAVMODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NAVMODEL_TYPE))

typedef struct _NavModel NavModel;
typedef struct _NavModelClass NavModelClass;

struct _NavModel
{
	GObject parent;
	GtkTreeModel model;
};

struct _NavModelClass
{
	GObjectClass parent;
};

G_END_DECLS

#endif
