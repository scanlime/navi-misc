/*
 * base.c - Definition for the base object
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

#include "base.h"

static void       base_class_init           (BaseClass *klass);
static void       base_init                 (Base *base);
static void       base_set_property         (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       base_get_property         (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       base_finalize             (GObject *object);
static void       base_init_position_params (GObjectClass *object_class);
static void       base_init_size_params     (GObjectClass *object_class);
static void       base_init_other_params    (GObjectClass *object_class);
static GdkPixbuf* base_get_icon             (void);
static GList*     base_get_drawables        (SceneObject *self);

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
  PROP_TEAM,
  PROP_SHOOT_THROUGH,
  PROP_DRIVE_THROUGH,
};

static gpointer parent_class = NULL;
static GType team_enum_type;

static const
GEnumValue team_enum[] =
{
  { 0, "red",    "Red" },
  { 1, "green",  "Green" },
  { 2, "blue",   "Blue" },
  { 3, "purple", "Purple" },
  { 0, 0,        0 },
};

GType
base_get_type (void)
{
  static GType base_type = 0;
  if (!base_type)
  {
    static const GTypeInfo base_info =
    {
      sizeof (BaseClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    base_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Base),
      0,                  /* n preallocs */
      (GInstanceInitFunc) base_init,
    };

    base_type = g_type_register_static (SCENE_OBJECT_TYPE, "Base", &base_info, 0);
    team_enum_type = g_enum_register_static ("BaseTeam", team_enum);
  }

  return base_type;
}

static void
base_class_init (BaseClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = base_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = base_get_drawables;

  object_class->set_property = base_set_property;
  object_class->get_property = base_get_property;
  object_class->finalize = base_finalize;

  base_init_position_params (object_class);
  base_init_size_params (object_class);
  base_init_other_params (object_class);
}

static void
base_init (Base *base)
{
  base->drawables = NULL;
}

static void
update_double_if_necessary (gdouble new_value, gboolean *dirty, gdouble *param, gdouble epsilon)
{
  if (fabs (new_value - *param) > epsilon)
  {
    *param = new_value;
    *dirty = TRUE;
  }
}

static void
update_enum_if_necessary (gint new_value, gboolean *dirty, gint *param)
{
  if (new_value != *param)
  {
    *param = new_value;
    *dirty = TRUE;
  }
}

static void
update_boolean_if_necessary (gboolean new_value, gboolean *dirty, gboolean *param)
{
  if (new_value != *param)
  {
    *param = new_value;
    *dirty = TRUE;
  }
}

static void
base_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  Base *self = BASE (object);

  switch (prop_id)
  {
    case PROP_POSITION_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[0], 0.09);
      break;

    case PROP_POSITION_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[1], 0.09);
      break;

    case PROP_POSITION_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[2], 0.09);
      break;

    case PROP_ROTATION:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.rotation, 0.09);
      break;

    case PROP_SIZE_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.09);
      break;

    case PROP_SIZE_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[1], 0.09);
      break;

    case PROP_SIZE_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[2], 0.09);
      break;

    case PROP_TEAM:
      update_enum_if_necessary (g_value_get_enum (value), &self->state_dirty, &self->param.team);
      break;

    case PROP_DRIVE_THROUGH:
      update_boolean_if_necessary (g_value_get_boolean (value), &self->state_dirty, &self->param.drive_through);
      break;

    case PROP_SHOOT_THROUGH:
      update_boolean_if_necessary (g_value_get_boolean (value), &self->state_dirty, &self->param.shoot_through);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  if (self->state_dirty)
  {
    g_signal_emit_by_name (object, "dirty");
  }
}

static void
base_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  Base *self = BASE (object);

  switch (prop_id)
  {
    case PROP_POSITION_X:
      g_value_set_double (value, self->param.position[0]);
      break;

    case PROP_POSITION_Y:
      g_value_set_double (value, self->param.position[1]);
      break;

    case PROP_POSITION_Z:
      g_value_set_double (value, self->param.position[2]);
      break;

    case PROP_ROTATION:
      g_value_set_double (value, self->param.rotation);
      break;

    case PROP_SIZE_X:
      g_value_set_double (value, self->param.size[0]);
      break;

    case PROP_SIZE_Y:
      g_value_set_double (value, self->param.size[1]);
      break;

    case PROP_SIZE_Z:
      g_value_set_double (value, self->param.size[2]);
      break;

    case PROP_TEAM:
      g_value_set_enum (value, self->param.team);
      break;

    case PROP_DRIVE_THROUGH:
      g_value_set_boolean (value, self->param.drive_through);
      break;

    case PROP_SHOOT_THROUGH:
      g_value_set_boolean (value, self->param.shoot_through);
      break;
  }
}

static void
base_finalize (GObject *object)
{
}

static void
base_init_position_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Position";

  spec = g_param_spec_double       ("x",
                                   "X",
			           "Position of the base along the X axis",
			           -1000, 1000, 0,
			           G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_X, spec);

  spec = g_param_spec_double      ("y",
                                   "Y",
				   "Position of the base along the Y axis",
				   -1000, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Y, spec);

  spec = g_param_spec_double      ("z",
                                   "Z",
				   "Position of the base along the Z axis",
				   0, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Z, spec);

  spec = g_param_spec_double      ("rotation",
                                   "Rotation",
				   "Rotation of the base around it's origin",
				   0, 360, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_ROTATION, spec);
}

static void
base_init_size_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Size";

  spec = g_param_spec_double      ("width",
                                   "Width",
				   "Width of the box",
				   0, 1000, 30,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_X, spec);

  spec = g_param_spec_double      ("depth",
                                   "Depth",
				   "Depth of the box",
				   0, 1000, 30,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Y, spec);

  spec = g_param_spec_double      ("height",
                                   "Height",
				   "Height of the box",
				   0, 1000, 9.42,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Z, spec);
}

static void
base_init_other_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Options";

  spec = g_param_spec_enum        ("team",
                                   "Team",
				   "Which team the base is associated with",
				   team_enum_type,
				   0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_TEAM, spec);

  spec = g_param_spec_boolean     ("drive-through",
                                   "Drive through",
				   "Whether or not tanks can drive through the box",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_DRIVE_THROUGH, spec);

  spec = g_param_spec_boolean     ("shoot-through",
                                   "Shoot through",
				   "Whether or not bullets pass through the box",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_SHOOT_THROUGH, spec);
}

static GdkPixbuf*
base_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/base.png", NULL);
  }

  return gdk_pixbuf_ref (icon);
}

static GList*
base_get_drawables (SceneObject *self)
{
  Base *b = BASE (self);
  return b->drawables;
}
