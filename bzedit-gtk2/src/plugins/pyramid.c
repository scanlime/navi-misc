/*
 * pyramid.c - Definition for the pyramid object
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

#include "pyramid.h"

static void       pyramid_class_init            (PyramidClass *klass);
static void       pyramid_init                  (Pyramid *pyramid);
static void       pyramid_set_property          (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       pyramid_get_property          (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       pyramid_finalize              (GObject *object);
static void       pyramid_init_position_params  (GObjectClass *object_class);
static void       pyramid_init_size_params      (GObjectClass *object_class);
static void       pyramid_init_other_params     (GObjectClass *object_class);
static GdkPixbuf* pyramid_get_icon              (void);
static GList*     pyramid_get_drawables         (SceneObject *self);
static void       pyramid_select                (SceneObject *self);
static void       pyramid_deselect              (SceneObject *self);

static void       pyramid_drawable_class_init   (PyramidDrawableClass *klass);
static void       pyramid_drawable_init         (PyramidDrawable *pd);
static void       pyramid_drawable_draw_to_list (DisplayList *dl);

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
  PROP_INVERTED,
  PROP_DRIVE_THROUGH,
  PROP_SHOOT_THROUGH,
};

static gpointer parent_class = NULL;

GType
pyramid_get_type (void)
{
  static GType pyramid_type = 0;

  if (!pyramid_type)
  {
    static const GTypeInfo pyramid_info =
    {
      sizeof (PyramidClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    pyramid_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (Pyramid),
      0,                  /* n preallocs */
      (GInstanceInitFunc) pyramid_init,
    };

    pyramid_type = g_type_register_static (SCENE_OBJECT_TYPE, "Pyramid", &pyramid_info, 0);
  }

  return pyramid_type;
}

static void
pyramid_class_init (PyramidClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  object_class->set_property = pyramid_set_property;
  object_class->get_property = pyramid_get_property;
  object_class->finalize = pyramid_finalize;

  so_class->get_icon = pyramid_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = pyramid_get_drawables;
  so_class->select = pyramid_select;
  so_class->deselect = pyramid_deselect;

  pyramid_init_position_params (object_class);
  pyramid_init_size_params (object_class);
  pyramid_init_other_params (object_class);
}

static void
pyramid_init (Pyramid *pyramid)
{
  pyramid->drawables = NULL;
  pyramid->selected = FALSE;
  pyramid->drawable = g_object_ref (pyramid_drawable_new ((SceneObject*) pyramid));

  pyramid->drawables = g_list_append (pyramid->drawables, (gpointer) pyramid->drawable);
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
pyramid_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  Pyramid *self = PYRAMID (object);
  SceneObject *so = SCENE_OBJECT (object);

  switch (prop_id)
  {
    case PROP_POSITION_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[0], 0.09);
      so->bb.position[0] = self->param.position[0];
      break;

    case PROP_POSITION_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[1], 0.09);
      so->bb.position[1] = self->param.position[1];
      break;

    case PROP_POSITION_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[2], 0.09);
      so->bb.position[2] = self->param.position[2] + self->param.size[2] / 2.0;
      break;

    case PROP_ROTATION:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.rotation, 0.09);
      so->bb.rotation = self->param.rotation;
      break;

    case PROP_SIZE_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.09);
      so->bb.size[0] = self->param.size[0];
      break;

    case PROP_SIZE_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[1], 0.09);
      so->bb.size[1] = self->param.size[1];
      break;

    case PROP_SIZE_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[2], 0.09);
      so->bb.position[2] = self->param.position[2] + self->param.size[2] / 2.0;
      so->bb.size[2] = self->param.size[2] / 2.0;
      break;

    case PROP_INVERTED:
      update_boolean_if_necessary (g_value_get_boolean (value), &self->state_dirty, &self->param.inverted);
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
    DISPLAY_LIST (self->drawable)->dirty = TRUE;
    g_signal_emit_by_name (object, "dirty");
    g_signal_emit_by_name (G_OBJECT (self->drawable), "dirty");
  }
}

static void
pyramid_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  Pyramid *self = PYRAMID (object);

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

    case PROP_INVERTED:
      g_value_set_boolean(value, self->param.inverted);
      break;

    case PROP_DRIVE_THROUGH:
      g_value_set_boolean(value, self->param.drive_through);
      break;

    case PROP_SHOOT_THROUGH:
      g_value_set_boolean(value, self->param.shoot_through);
      break;
  }
}

static void
pyramid_finalize (GObject *object)
{
}

static void
pyramid_init_position_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Position";

  spec = g_param_spec_double       ("x",
                                   "X",
			           "Position of the pyramid along the X axis",
			           -1000, 1000, 0,
			           G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_X, spec);

  spec = g_param_spec_double      ("y",
                                   "Y",
				   "Position of the pyramid along the Y axis",
				   -1000, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Y, spec);

  spec = g_param_spec_double      ("z",
                                   "Z",
				   "Position of the pyramid along the Z axis",
				   0, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Z, spec);

  spec = g_param_spec_double      ("rotation",
                                   "Rotation",
				   "Rotation of the pyramid around it's origin",
				   -360, 360, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_ROTATION, spec);
}

static void
pyramid_init_size_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Size";

  spec = g_param_spec_double      ("width",
                                   "Width",
				   "Width of the pyramid",
				   0, 1000, 8.2,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_X, spec);

  spec = g_param_spec_double      ("depth",
                                   "Depth",
				   "Depth of the pyramid",
				   0, 1000, 8.2,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Y, spec);

  spec = g_param_spec_double      ("height",
                                   "Height",
				   "Height of the pyramid",
				   0, 1000, 10.25,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_SIZE_Z, spec);
}

static void
pyramid_init_other_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Doopy";

  spec = g_param_spec_boolean     ("inverted",
                                   "Inverted",
				   "Whether the pyramid is inverted or not",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_INVERTED, spec);

  spec = g_param_spec_boolean     ("drive-through",
                                   "Drive through",
				   "Whether or not tanks can drive through the pyramid",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_DRIVE_THROUGH, spec);

  spec = g_param_spec_boolean     ("shoot-through",
                                   "Shoot through",
				   "Whether or not bullets pass through the pyramid",
				   FALSE,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  g_object_class_install_property (object_class, PROP_SHOOT_THROUGH, spec);
}

Pyramid*
pyramid_new (void)
{
  return PYRAMID (g_object_new (pyramid_get_type (), NULL));
}

static GdkPixbuf*
pyramid_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/pyramid.png", NULL);
  }

  return icon;
}

static GList*
pyramid_get_drawables (SceneObject *self)
{
  Pyramid *p = PYRAMID (self);
  return p->drawables;
}

static void
pyramid_select (SceneObject *self)
{
  Pyramid *p = PYRAMID (self);
  p->selected = TRUE;
  g_signal_emit_by_name (G_OBJECT (p->drawable), "dirty");
  g_signal_emit_by_name (G_OBJECT (p), "dirty");
}

static void
pyramid_deselect (SceneObject *self)
{
  Pyramid *p = PYRAMID (self);
  p->selected = FALSE;
  g_signal_emit_by_name (G_OBJECT (p->drawable), "dirty");
  g_signal_emit_by_name (G_OBJECT (p), "dirty");
}

GType
pyramid_drawable_get_type (void)
{
  static GType pd_type = 0;
  if (!pd_type)
  {
    static const GTypeInfo pd_info =
    {
      sizeof (PyramidDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    pyramid_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (PyramidDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) pyramid_drawable_init,
    };

    pd_type = g_type_register_static (DISPLAY_LIST_TYPE, "PyramidDrawable", &pd_info, 0);
  }

  return pd_type;
}

static void
pyramid_drawable_class_init (PyramidDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = pyramid_drawable_draw_to_list;
}

static void
pyramid_drawable_init (PyramidDrawable *pd)
{
  Drawable *d = DRAWABLE (pd);

  d->texture = g_strdup ("data/textures/black_marble.jpeg");
}

Drawable*
pyramid_drawable_new (SceneObject *parent)
{
  Drawable *d = DRAWABLE (g_object_new (pyramid_drawable_get_type (), NULL));
  d->parent = parent;
  return d;
}

static void
vector3_cross (const gdouble a[3], const gdouble b[3], gdouble *c)
{
  c[0] = a[2]*b[1] - a[1]*b[2];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}

static void
pyramid_drawable_draw_to_list (DisplayList *dl)
{
  PyramidDrawable *pd = PYRAMID_DRAWABLE (dl);
  Pyramid *p = PYRAMID (DRAWABLE (dl)->parent);
  gdouble width, depth, height;
  gdouble wrep, drep, hrep;
  gdouble a[3], b[3], normal[3];

  width = p->param.size[0];
  depth = p->param.size[1];
  height = p->param.size[2];

  wrep = width / 8;
  drep = depth / 8;
  hrep = height / 8;

  glPushMatrix ();

  glTranslated (p->param.position[0], p->param.position[1], p->param.position[2]);
  glRotated (p->param.rotation, 0.0, 0.0, 1.0);

  if (p->param.inverted)
  {
    glBegin (GL_QUADS);
    {
      glNormal3f (0.0f, 0.0f, 1.0f);
      glTexCoord2d ( wrep,   0);
      glVertex3d   ( width, -depth, height);
      glTexCoord2d ( wrep,   drep);
      glVertex3d   ( width,  depth, height);
      glTexCoord2d ( 0,      drep);
      glVertex3d   (-width,  depth, height);
      glTexCoord2d ( 0,      0);
      glVertex3d   (-width, -depth, height);
    }
    glEnd ();

    glBegin (GL_TRIANGLES);
    {
    };
    glEnd ();
  }
  else
  {
    glBegin (GL_QUADS);
    {
      glNormal3f (0.0f, 0.0f, -1.0f);
      glTexCoord2d ( 0,      0);
      glVertex3d   (-width, -depth, 0);
      glTexCoord2d ( 0,      drep);
      glVertex3d   (-width,  depth, 0);
      glTexCoord2d ( wrep,   drep);
      glVertex3d   ( width,  depth, 0);
      glTexCoord2d ( wrep,   0);
      glVertex3d   ( width, -depth, 0);
    }
    glEnd ();

    glBegin (GL_TRIANGLES);
    {
      /* X+ side */
      a[0] = width;     a[1] = -depth; a[2] = height;
      b[0] = width * 2; b[1] = 0;      b[2] = 0;
      vector3_cross (a, b, normal);
      glNormal3dv (normal);
      glTexCoord2d ( 0,        0);
      glVertex3d   (-width,    depth, 0);
      glTexCoord2d ( wrep,     hrep);
      glVertex3d   ( 0,        0,     height);
      glTexCoord2d ( wrep * 2, 0);
      glVertex3d   ( width,    depth, 0);

      /* Y+ side */
      a[0] = width; a[1] = -depth;     a[2] = height;
      b[0] = 0;     b[1] =  depth * 2; b[2] = 0;
      vector3_cross (a, b, normal);
      glNormal3dv (normal);
      glTexCoord2d (drep * 2,  0);
      glVertex3d   (width,     depth, 0);
      glTexCoord2d (drep,      hrep);
      glVertex3d   (0,         0,     height);
      glTexCoord2d (0,         0);
      glVertex3d   (width,    -depth, 0);

      /* X- side */
      a[0] = -width;     a[1] = depth; a[2] = height;
      b[0] = -width * 2; b[1] = 0;     b[2] = 0;
      vector3_cross (a, b, normal);
      glNormal3dv (normal);
      glTexCoord2d ( 0,         0);
      glVertex3d   ( width,    -depth, 0);
      glTexCoord2d ( wrep,      hrep);
      glVertex3d   ( 0,         0,     height);
      glTexCoord2d ( wrep * 2,  0);
      glVertex3d   (-width,    -depth, 0);

      /* Y- side */
      a[0] = -width; a[1] = depth;     a[2] = height;
      b[0] = 0;      b[1] = depth * 2; b[2] = 0;
      vector3_cross (a, b, normal);
      glNormal3dv (normal);
      glTexCoord2d (drep * 2,  0);
      glVertex3d   (-width,   -depth, 0);
      glTexCoord2d (drep,      hrep);
      glVertex3d   (0,         0,     height);
      glTexCoord2d (0,         0);
      glVertex3d   (-width,    depth, 0);
    };
    glEnd ();
  }

  glPopMatrix ();
}
