/*
 * group.h - Definition for the group object
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

#ifndef __GROUP_H__
#define __GROUP_H__

#include "sceneobject.h"

G_BEGIN_DECLS

#define GROUP_TYPE            (group_get_type ())
#define GROUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GROUP_TYPE, Group))
#define GROUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GROUP_TYPE, GroupClass))
#define IS_GROUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GROUP_TYPE))
#define IS_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GROUP_TYPE))

typedef struct _Group      Group;
typedef struct _GroupClass GroupClass;

struct _Group
{
  SceneObject parent;

  GList *children;
  GHashTable *children_handlers;
  gdouble max[3], min[3];
};

struct _GroupClass
{
  SceneObjectClass parent_class;
};

GType  group_get_type (void) G_GNUC_CONST;
Group* group_new      (void);

G_END_DECLS

#endif
