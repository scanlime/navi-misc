/*
 * world.c - Definition for the world object
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

#include "world.h"

static void       world_class_init             (WorldClass *klass);
static void       world_init                   (World *world);
static void       world_set_property           (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       world_get_property           (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       world_finalize               (GObject *object);
static void       world_init_params            (GObjectClass *object_class);
static GdkPixbuf* world_get_icon               (void);
static gboolean   world_creatable              (void);
static GList*     world_get_drawables          (SceneObject *self);

static void       ground_drawable_class_init   (GroundDrawableClass *klass);
static void       ground_drawable_init         (GroundDrawable *gd);
static void       ground_drawable_draw_to_list (DisplayList *dl);

enum
{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_GRAVITY
};

static gpointer parent_class = NULL;

GType
world_get_type (void)
{
  static GType world_type = 0;
  if (!world_type)
    {
      static const GTypeInfo world_info =
      {
	sizeof (WorldClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    world_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (World),
	0,                  /* n preallocs */
	(GInstanceInitFunc) world_init,
      };

      world_type = g_type_register_static (SCENE_OBJECT_TYPE, "World", &world_info, 0);
    }

  return world_type;
}

static void
world_class_init (WorldClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  object_class->set_property = world_set_property;
  object_class->get_property = world_get_property;
  object_class->finalize = world_finalize;

  so_class->get_icon = world_get_icon;
  so_class->creatable = world_creatable;
  so_class->get_drawables = world_get_drawables;

  world_init_params (object_class);
}

static void
world_init (World *world)
{
  world->drawables = g_list_alloc ();
  Drawable *ground = ground_drawable_new ();

  g_list_append (world->drawables, (gpointer) ground);
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
world_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  World *self = WORLD (object);

  switch (prop_id)
  {
    case PROP_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.9);
      break;

    case PROP_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.9);
      break;

    case PROP_GRAVITY:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.gravity, 0.009);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
world_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  World *self = WORLD (object);

  switch (prop_id)
  {
    case PROP_X:
      g_value_set_double (value, self->param.size[0]);
      break;

    case PROP_Y:
      g_value_set_double (value, self->param.size[0]);
      break;

    case PROP_GRAVITY:
      g_value_set_double (value, self->param.gravity);
      break;

  }
}

static void
world_finalize (GObject *object)
{
}

static void
world_init_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Parameters";

  spec = g_param_spec_double      ("width",
                                   "Width",
				   "Width of the world",
				   0, 1000, 400,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 1, 1, 1);
  g_object_class_install_property (object_class, PROP_X, spec);

  spec = g_param_spec_double      ("height",
                                   "Height",
				   "Height of the world",
				   0, 1000, 400,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 1, 1, 1);
  g_object_class_install_property (object_class, PROP_Y, spec);

  spec = g_param_spec_double      ("gravity",
                                   "Gravity",
				   "Default gravity in the world",
				   -20, -0.01, -9.8,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.01, 0.1, 3);
  g_object_class_install_property (object_class, PROP_GRAVITY, spec);
}

World*
world_new (void)
{
  return WORLD (g_object_new (world_get_type (), NULL));
}

static GdkPixbuf*
world_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/groundicon.png", NULL);
  }

  return icon;
}

static gboolean
world_creatable (void)
{
  return FALSE;
}

static GList*
world_get_drawables (SceneObject *self)
{
}

GType
ground_drawable_get_type (void)
{
  static GType gd_type = 0;
  if (!gd_type)
    {
      static const GTypeInfo gd_info =
      {
	sizeof (GroundDrawableClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    ground_drawable_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (GroundDrawable),
	0,                  /* n preallocs */
	(GInstanceInitFunc) ground_drawable_init,
      };

      gd_type = g_type_register_static (DISPLAY_LIST_TYPE, "GroundDrawable", &gd_info, 0);
    }

  return gd_type;
}

static void
ground_drawable_class_init (GroundDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = ground_drawable_draw_to_list;
}

static void
ground_drawable_init (GroundDrawable *gd)
{
  gd->base_texture_repeat = 90;
  gd->overlay_texture_repeat = 1;
}

static void
ground_drawable_draw_to_list (DisplayList *dl)
{
  GroundDrawable *gd = GROUND_DRAWABLE (dl);

  glPushMatrix ();

  /* We want to draw both sides of the surface. This will have OpenGL
     automatically flip the surface normals when drawing the back side */
  glDisable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 1);

  glBegin(GL_QUADS);
  glNormal3f (0, 0, 1);

  glTexCoord2f (gd->base_texture_repeat, gd->base_texture_repeat);
  glVertex3f (gd->size, gd->size, 0);

  glTexCoord2f (0, gd->base_texture_repeat);
  glVertex3f (-gd->size, gd->size, 0);

  glTexCoord2f (0, 0);
  glVertex3f (-gd->size, -gd->size, 0);

  glTexCoord2f (gd->base_texture_repeat, 0);
  glVertex3f (gd->size, -gd->size, 0);

  glEnd();
  glEnable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 0);
  glPopMatrix ();
}
