/*
 * pyramid.h - Definition for the pyramid object
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

#ifndef __PYRAMID_H__
#define __PYRAMID_H__

#include "sceneobject.h"

G_BEGIN_DECLS

#define PYRAMID_TYPE		(pyramid_get_type())
#define PYRAMID(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PYRAMID_TYPE, Pyramid))
#define PYRAMID_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PYRAMID_TYPE, Pyramid))
#define IS_PYRAMID(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PYRAMID_TYPE))
#define IS_PYRAMID_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PYRAMID_TYPE))

typedef struct _Pyramid      Pyramid;
typedef struct _PyramidClass PyramidClass;

typedef struct
{
  gdouble position[3];
  gdouble size[3];
  gdouble rotation;
  gboolean inverted;
  gboolean drive_through;
  gboolean shoot_through;
} PyramidParams;

struct _Pyramid
{
  SceneObject parent;

  PyramidParams param;
  gboolean state_dirty;
};

struct _PyramidClass
{
  SceneObjectClass parent_class;
};

GType    pyramid_get_type (void) G_GNUC_CONST;
Pyramid* pyramid_new      (void);

G_END_DECLS

#endif
