/*
 * tankzone.h - Definition for the tez object
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

#ifndef __TANK_ZONE_H__
#define __TANK_ZONE_H__

#include "zone.h"

G_BEGIN_DECLS

#define TANK_ZONE_TYPE            (tank_zone_get_type ())
#define TANK_ZONE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TANK_ZONE_TYPE, TankZone))
#define TANK_ZONE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TANK_ZONE_TYPE, TankZoneClass))
#define IS_TANK_ZONE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TANK_ZONE_TYPE))
#define IS_TANK_ZONE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TANK_ZONE_TYPE))

typedef struct _TankZone      TankZone;
typedef struct _TankZoneClass TankZoneClass;

struct _TankZone
{
  Zone parent;

  GList *drawables;
};

struct _TankZoneClass
{
  ZoneClass parent_class;
};

GType tank_zone_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
