/*
 * worldweapon.h - Definition for the world weapon object
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

#ifndef __WORLD_WEAPON_H__
#define __WORLD_WEAPON_H__

#include "sceneobject.h"
#include "displaylist.h"

G_BEGIN_DECLS

#define WORLD_WEAPON_TYPE            (world_weapon_get_type ())
#define WORLD_WEAPON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), WORLD_WEAPON_TYPE, WorldWeapon))
#define WORLD_WEAPON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), WORLD_WEAPON_TYPE, WorldWeaponClass))
#define IS_WORLD_WEAPON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WORLD_WEAPON_TYPE))
#define IS_WORLD_WEAPON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WORLD_WEAPON_TYPE))

typedef struct _WorldWeapon      WorldWeapon;
typedef struct _WorldWeaponClass WorldWeaponClass;

struct _WorldWeapon
{
  SceneObject parent;
  GList *drawables;
};

struct _WorldWeaponClass
{
  SceneObjectClass parent_class;
};

GType world_weapon_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif
