/*
 * tankzone.c - Definition for the tez object
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

#include "tankzone.h"

static void       tank_zone_class_init    (TankZoneClass *klass);
static void       tank_zone_init          (TankZone *tez);
static void       tank_zone_set_property  (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       tank_zone_get_property  (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       tank_zone_finalize      (GObject *object);
static void       tank_zone_init_params   (GObjectClass *object_class);
static GdkPixbuf* tank_zone_get_icon      (void);

enum
{
  PROP_0,
};

static gpointer parent_class = NULL;

GType
tank_zone_get_type (void)
{
  static GType tez_type = 0;
  if (!tez_type)
  {
    static const GTypeInfo tez_info =
    {
      sizeof (TankZoneClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    tank_zone_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (TankZone),
      0,                  /* n preallocs */
      (GInstanceInitFunc) tank_zone_init,
    };

    tez_type = g_type_register_static (ZONE_TYPE, "TankEntryZone", &tez_info, 0);
  }

  return tez_type;
}

static void
tank_zone_class_init (TankZoneClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (ZONE_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = tank_zone_get_icon;
  so_class->creatable = TRUE;

  object_class->set_property = tank_zone_set_property;
  object_class->get_property = tank_zone_get_property;
  object_class->finalize = tank_zone_finalize;

  tank_zone_init_params (object_class);
}

static void
tank_zone_init (TankZone *tez)
{
}

static void
tank_zone_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
tank_zone_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
tank_zone_finalize (GObject *object)
{
}

static void
tank_zone_init_params (GObjectClass *object_class)
{
}

static GdkPixbuf*
tank_zone_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
    icon = gdk_pixbuf_new_from_file ("data/tez.png", NULL);

  return gdk_pixbuf_ref (icon);
}
