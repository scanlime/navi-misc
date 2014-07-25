/*
 * zone.h - Definition for the zone object
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

#ifndef __ZONE_H__
#define __ZONE_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define ZONE_TYPE            (zone_get_type ())
#define ZONE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZONE_TYPE, Zone))
#define ZONE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ZONE_TYPE, ZoneClass))
#define IS_ZONE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZONE_TYPE))
#define IS_ZONE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZONE_TYPE))

typedef struct _Zone      Zone;
typedef struct _ZoneClass ZoneClass;

typedef struct
{
  gdouble position[3];
  gdouble size[3];
  gdouble rotation;
} ZoneParams;

struct _Zone
{
  SceneObject parent;

  ZoneParams param;
  gboolean state_dirty;
  GList *drawables;
};

struct _ZoneClass
{
  SceneObjectClass parent_class;
};

GType zone_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
