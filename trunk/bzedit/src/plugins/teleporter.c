/*
 * teleporter.c - Definition for the teleporter object
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

#include "teleporter.h"

static void       teleporter_class_init           (TeleporterClass *klass);
static void       teleporter_init                 (Teleporter *teleporter);
static void       teleporter_set_property         (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       teleporter_get_property         (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       teleporter_finalize             (GObject *object);
static void       teleporter_init_position_params (GObjectClass *object_class);
static void       teleporter_init_size_params     (GObjectClass *object_class);
static void       teleporter_init_other_params    (GObjectClass *object_class);
static GdkPixbuf* teleporter_get_icon             (void);
static gboolean   teleporter_creatable            (void);

enum
{
  PROP_0,
  PROP_POSITION_X,
  PROP_POSITION_Y,
  PROP_POSITION_Z,
  PROP_ROTATION,
  PROP_SIZE_WIDTH,
  PROP_SIZE_HEIGHT,
  PROP_BORDER_WIDTH,
  PROP_DRIVE_THROUGH,
  PROP_SHOOT_THROUGH,
};

static gpointer parent_class = NULL;

GType
teleporter_get_type (void)
{
  static GType teleporter_type = 0;

  if (!teleporter_type)
    {
      static const GTypeInfo teleporter_info =
      {
	sizeof (TeleporterClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    teleporter_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Teleporter),
	0,                  /* n preallocs */
	(GInstanceInitFunc) teleporter_init,
      };

      teleporter_type = g_type_register_static (SCENE_OBJECT_TYPE, "Teleporter", &teleporter_info, 0);
    }

  return teleporter_type;
}

static void
teleporter_class_init (TeleporterClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = teleporter_get_icon;
  so_class->creatable = teleporter_creatable;

  object_class->set_property = teleporter_set_property;
  object_class->get_property = teleporter_get_property;
  object_class->finalize = teleporter_finalize;

  teleporter_init_position_params (object_class);
  teleporter_init_size_params (object_class);
  teleporter_init_other_params (object_class);
}

static void
teleporter_init (Teleporter *teleporter)
{
  /* nothing to do here yet, since everything is set up by the G_PARAM_CONSTRUCT properties */
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
update_boolean_if_necessary (gboolean new_value, gboolean *dirty, gboolean *param)
{
  if (new_value != *param)
  {
    *param = new_value;
    *dirty = TRUE;
  }
}

static void
teleporter_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  Teleporter *self = TELEPORTER (object);

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

    case PROP_SIZE_WIDTH:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.09);
      break;

    case PROP_SIZE_HEIGHT:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[1], 0.09);
      break;

    case PROP_BORDER_WIDTH:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.border, 0.09);
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
}

static void
teleporter_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  Teleporter *self = TELEPORTER (object);

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

    case PROP_SIZE_WIDTH:
      g_value_set_double (value, self->param.size[0]);
      break;

    case PROP_SIZE_HEIGHT:
      g_value_set_double (value, self->param.size[1]);
      break;

    case PROP_BORDER_WIDTH:
      g_value_set_double (value, self->param.border);
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
teleporter_finalize (GObject *object)
{
}

static void
teleporter_init_position_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Position";

  spec = g_param_spec_double       ("x",
                                   "X",
			           "Position of the teleporter along the X axis",
			           -1000, 1000, 0,
			           G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_X, spec);

  spec = g_param_spec_double      ("y",
                                   "Y",
				   "Position of the teleporter along the Y axis",
				   -1000, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Y, spec);

  spec = g_param_spec_double      ("z",
                                   "Z",
				   "Position of the teleporter along the Z axis",
				   0, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Z, spec);

  spec = g_param_spec_double      ("rotation",
                                   "Rotation",
				   "Rotation of the teleporter around it's origin",
				   0, 360, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_ROTATION, spec);
}

static void
teleporter_init_size_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Size";

  spec = g_param_spec_double      ("width",
                                   "Width",
				   "Width of the teleporter field",
				   0, 1000, 4.48,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_WIDTH, spec);

  spec = g_param_spec_double      ("height",
                                   "Height",
				   "Height of the teleporter field",
				   0, 1000, 20.16,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_HEIGHT, spec);

  spec = g_param_spec_double      ("border",
                                   "Border Width",
				   "Width of the teleporter border",
				   0, 1000, 1,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_BORDER_WIDTH, spec);
}

static void
teleporter_init_other_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Doopy";

  spec = g_param_spec_boolean     ("drive-through",
                                   "Drive through",
				   "Whether or not tanks can drive through the teleporter border",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_DRIVE_THROUGH, spec);

  spec = g_param_spec_boolean     ("shoot-through",
                                   "Shoot through",
				   "Whether or not bullets pass through the teleporter border",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_SHOOT_THROUGH, spec);
}

Teleporter*
teleporter_new (void)
{
  return TELEPORTER (g_object_new (teleporter_get_type (), NULL));
}

static GdkPixbuf*
teleporter_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/teleicon.png", NULL);
  }

  return gdk_pixbuf_ref (icon);
}

static gboolean
teleporter_creatable (void)
{
  return TRUE;
}
