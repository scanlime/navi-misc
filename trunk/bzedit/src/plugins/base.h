/*
 * base.h - Definition for the base object
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

#ifndef __BASE_H__
#define __BASE_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define BASE_TYPE            (base_get_type ())
#define BASE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASE_TYPE, Base))
#define BASE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASE_TYPE, BaseClass))
#define IS_BASE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASE_TYPE))
#define IS_BASE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASE_TYPE))

typedef struct _Base      Base;
typedef struct _BaseClass BaseClass;

enum
{
  TEAM_RED,
  TEAM_BLUE,
  TEAM_GREEN,
  TEAM_PURPLE,
};

typedef struct
{
  gdouble position[3];
  gdouble size[3];
  gdouble rotation;
  gint team;
  gboolean drive_through;
  gboolean shoot_through;
} BaseParams;

struct _Base
{
  SceneObject parent;

  BaseParams param;
  gboolean state_dirty;
  GList *drawables;
  Drawable *sides, *top;
};

struct _BaseClass
{
  SceneObjectClass parent_class;
};

GType base_get_type (void) G_GNUC_CONST;
Base* base_new      (void);

G_END_DECLS

#endif
