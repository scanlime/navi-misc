/*
 * worldweapon.c - Definition for the world weapon object
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

#include "worldweapon.h"

static void       world_weapon_class_init           (WorldWeaponClass *klass);
static void       world_weapon_init                 (WorldWeapon *ww);
static void       world_weapon_set_property         (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       world_weapon_get_property         (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       world_weapon_finalize             (GObject *object);
static void       world_weapon_init_position_params (GObjectClass *object_class);
static void       world_weapon_init_other_params    (GObjectClass *object_class);
static GdkPixbuf* world_weapon_get_icon             (void);
static GList*     world_weapon_get_drawables        (SceneObject *self);

enum
{
  PROP_0,
  PROP_POSITION_X,
  PROP_POSITION_Y,
  PROP_POSITION_Z,
  PROP_TYPE,
  PROP_DELAY,
};

static gpointer parent_class = NULL;

GType
world_weapon_get_type (void)
{
  static GType ww_type = 0;
  if (!ww_type)
  {
    static const GTypeInfo ww_info =
    {
      sizeof (WorldWeaponClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    world_weapon_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (WorldWeapon),
      0,                  /* n preallocs */
      (GInstanceInitFunc) world_weapon_init,
    };

    ww_type = g_type_register_static (SCENE_OBJECT_TYPE, "WorldWeapon", &ww_info, 0);
  }

  return ww_type;
}

static void
world_weapon_class_init (WorldWeaponClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = world_weapon_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = world_weapon_get_drawables;

  object_class->set_property = world_weapon_set_property;
  object_class->get_property = world_weapon_get_property;
  object_class->finalize = world_weapon_finalize;

  world_weapon_init_position_params (object_class);
  world_weapon_init_other_params (object_class);
}

static void
world_weapon_init (WorldWeapon *ww)
{
  ww->drawables = NULL;
}

static void
world_weapon_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
world_weapon_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
world_weapon_finalize (GObject *object)
{
}

static void
world_weapon_init_position_params (GObjectClass *object_class)
{
}

static void
world_weapon_init_other_params (GObjectClass *object_class)
{
}

static GdkPixbuf*
world_weapon_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
    icon = gdk_pixbuf_new_from_file ("data/worldweapon.png", NULL);

  return gdk_pixbuf_ref (icon);
}

static GList*
world_weapon_get_drawables (SceneObject *self)
{
  WorldWeapon *ww = WORLD_WEAPON (self);
  return ww->drawables;
}
