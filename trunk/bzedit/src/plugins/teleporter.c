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

static void       teleporter_class_init                  (TeleporterClass *klass);
static void       teleporter_init                        (Teleporter *teleporter);
static void       teleporter_set_property                (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       teleporter_get_property                (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       teleporter_finalize                    (GObject *object);
static void       teleporter_init_position_params        (GObjectClass *object_class);
static void       teleporter_init_size_params            (GObjectClass *object_class);
static void       teleporter_init_other_params           (GObjectClass *object_class);
static GdkPixbuf* teleporter_get_icon                    (void);
static GList*     teleporter_get_drawables               (SceneObject *self);

static void       teleporter_field_drawable_class_init   (TeleporterFieldDrawableClass *klass);
static void       teleporter_field_drawable_init         (TeleporterFieldDrawable *tfd);
static void       teleporter_field_drawable_draw_to_list (DisplayList *dl);

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

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = teleporter_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = teleporter_get_drawables;

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
  teleporter->drawables = NULL;
  teleporter->field = teleporter_field_drawable_new ((SceneObject*) teleporter);

  teleporter->drawables = g_list_append (teleporter->drawables, (gpointer) teleporter->field);
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
update_float_if_necessary (gdouble new_value, gboolean *dirty, gfloat *param, gfloat epsilon)
{
  if (fabs ((gfloat)new_value - *param) > epsilon)
  {
    *param = (gfloat) new_value;
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
  if (self->state_dirty)
  {
    DISPLAY_LIST (self->field)->dirty = TRUE;
    g_signal_emit_by_name (object, "dirty");
    g_signal_emit_by_name (G_OBJECT (self->field), "dirty");
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
				   -360, 360, 0,
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
    icon = gdk_pixbuf_new_from_file ("data/teleporter.png", NULL);
  }

  return gdk_pixbuf_ref (icon);
}

static gboolean
teleporter_creatable (void)
{
  return TRUE;
}

static GList*
teleporter_get_drawables (SceneObject *self)
{
  Teleporter *t = TELEPORTER (self);
  return t->drawables;
}

GType
teleporter_field_drawable_get_type (void)
{
  static GType tfd_type = 0;
  if (!tfd_type)
  {
    static const GTypeInfo tfd_info =
    {
      sizeof (TeleporterFieldDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    teleporter_field_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (TeleporterFieldDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) teleporter_field_drawable_init,
    };

    tfd_type = g_type_register_static (DISPLAY_LIST_TYPE, "TeleporterFieldDrawable", &tfd_info, 0);
  }

  return tfd_type;
}

static void
teleporter_field_drawable_class_init (TeleporterFieldDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = teleporter_field_drawable_draw_to_list;
}

static void
teleporter_field_drawable_init (TeleporterFieldDrawable *tfd)
{
  Drawable *d = DRAWABLE (tfd);

  d->texture = "";
  d->render.blended = TRUE;
}

Drawable*
teleporter_field_drawable_new (SceneObject *parent)
{
  Drawable *d = DRAWABLE (g_object_new (teleporter_field_drawable_get_type (), NULL));
  d->parent = parent;
  return d;
}

static void
teleporter_field_drawable_draw_to_list (DisplayList *dl)
{
  TeleporterFieldDrawable *tfd = TELEPORTER_FIELD_DRAWABLE (dl);
  Teleporter *t = TELEPORTER (DRAWABLE (dl)->parent);
  gdouble width, height;

  width = t->param.size[0];
  height = t->param.size[1];

  glPushMatrix ();
  glTranslated (t->param.position[0], t->param.position[1], t->param.position[2]);
  glRotated (t->param.rotation, 0.0, 0.0, 1.0);

  glColor4f (0.0f, 0.0f, 0.0f, 0.6f);

  glBegin (GL_QUADS);
  {
    /* X+ side */
    glNormal3f (1.0f, 0.0f, 0.0f);
    glVertex3d (0,  width, 0);
    glVertex3d (0,  width, height);
    glVertex3d (0, -width, height);
    glVertex3d (0, -width, 0);

    /* X- side */
    glNormal3f (-1.0f, 0.0f, 0.0f);
    glVertex3d (0, -width, 0);
    glVertex3d (0, -width, height);
    glVertex3d (0,  width, height);
    glVertex3d (0,  width, 0);
  }
  glEnd ();

  glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
  glPopMatrix ();
}
