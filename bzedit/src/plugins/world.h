/*
 * world.h - Definition for the world object
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

#ifndef __WORLD_H__
#define __WORLD_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define WORLD_TYPE		(world_get_type ())
#define WORLD(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), WORLD_TYPE, World))
#define WORLD_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), WORLD_TYPE, WorldClass))
#define IS_WORLD(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), WORLD_TYPE))
#define IS_WORLD_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), WORLD_TYPE))

typedef struct _World      World;
typedef struct _WorldClass WorldClass;

typedef struct
{
  gdouble size[2];
  gdouble gravity;
} WorldParams;

struct _World
{
  SceneObject parent;

  WorldParams param;
  gboolean state_dirty;
  Drawable *ground;
  GList *drawables;
};

struct _WorldClass
{
  SceneObjectClass parent_class;
};

GType  world_get_type (void) G_GNUC_CONST;
World* world_new      (void);

#define GROUND_DRAWABLE_TYPE            (ground_drawable_get_type ())
#define GROUND_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GROUND_DRAWABLE_TYPE, GroundDrawable))
#define GROUND_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass, GROUND_DRAWABLE_TYPE, GroundDrawableClass))
#define IS_GROUND_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GROUND_DRAWABLE_TYPE))
#define IS_GROUND_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GROUND_DRAWABLE_TYPE))

typedef struct _GroundDrawable      GroundDrawable;
typedef struct _GroundDrawableClass GroundDrawableClass;

struct _GroundDrawable
{
  DisplayList parent;

  guint base_texture_repeat, overlay_texture_repeat;
  guint size[2];
};

struct _GroundDrawableClass
{
  DisplayListClass parent_class;
};

GType     ground_drawable_get_type (void) G_GNUC_CONST;
Drawable* ground_drawable_new (void);

G_END_DECLS

#endif
