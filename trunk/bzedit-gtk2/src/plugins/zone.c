/*
 * link.c - Definition for the link object
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

#include "zone.h"

static void   zone_class_init           (ZoneClass *klass);
static void   zone_init                 (Zone *zone);
static void   zone_set_property         (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void   zone_get_property         (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void   zone_init_position_params (GObjectClass *object_class);
static void   zone_init_size_params     (GObjectClass *object_class);
static GList* zone_get_drawables        (SceneObject *self);

enum
{
  PROP_0,
  PROP_POSITION_X,
  PROP_POSITION_Y,
  PROP_POSITION_Z,
  PROP_ROTATION,
  PROP_SIZE_X,
  PROP_SIZE_Y,
  PROP_SIZE_Z,
};

static gpointer parent_class = NULL;

GType
zone_get_type (void)
{
  static GType zone_type = 0;
  if (!zone_type)
  {
    static const GTypeInfo zone_info =
    {
      sizeof (ZoneClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    zone_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Zone),
      0,                  /* n preallocs */
      (GInstanceInitFunc) zone_init,
    };

    zone_type = g_type_register_static (SCENE_OBJECT_TYPE, "Zone", &zone_info, 0);
  }

  return zone_type;
}

static void
zone_class_init (ZoneClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_drawables = zone_get_drawables;

  object_class->set_property = zone_set_property;
  object_class->get_property = zone_get_property;

  zone_init_position_params (object_class);
  zone_init_size_params (object_class);
}

static void
zone_init (Zone *zone)
{
  zone->drawables = NULL;
}

static void
zone_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
zone_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
zone_init_position_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Position";

  spec = g_param_spec_double       ("x",
                                   "X",
			           "Position of the zone along the X axis",
			           -1000, 1000, 0,
			           G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_X, spec);

  spec = g_param_spec_double      ("y",
                                   "Y",
				   "Position of the zone along the Y axis",
				   -1000, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Y, spec);

  spec = g_param_spec_double      ("z",
                                   "Z",
				   "Position of the zone along the Z axis",
				   0, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Z, spec);

  spec = g_param_spec_double      ("rotation",
                                   "Rotation",
				   "Rotation of the zone around it's origin",
				   0, 360, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_ROTATION, spec);
}

static void
zone_init_size_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Size";

  spec = g_param_spec_double      ("width",
                                   "Width",
				   "Width of the zone",
				   0, 1000, 60,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_X, spec);

  spec = g_param_spec_double      ("depth",
                                   "Depth",
				   "Depth of the zone",
				   0, 1000, 60,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Y, spec);

  spec = g_param_spec_double      ("height",
                                   "Height",
				   "Height of the zone",
				   0, 1000, 60,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Z, spec);
}

static GList*
zone_get_drawables (SceneObject *self)
{
  Zone *z = ZONE (self);
  return z->drawables;
}
