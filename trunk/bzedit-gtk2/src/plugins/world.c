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

static void       world_class_init                 (WorldClass *klass);
static void       world_init                       (World *world);
static void       world_set_property               (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       world_get_property               (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       world_finalize                   (GObject *object);
static void       world_init_params                (GObjectClass *object_class);
static GdkPixbuf* world_get_icon                   (void);
static GList*     world_get_drawables              (SceneObject *self);

static void       ground_drawable_class_init       (GroundDrawableClass *klass);
static void       ground_drawable_init             (GroundDrawable *gd);
static void       ground_drawable_draw_to_list     (DisplayList *dl);

static void       wall_sides_drawable_class_init   (WallSidesDrawableClass *klass);
static void       wall_sides_drawable_init         (WallSidesDrawable *wsd);
static void       wall_sides_drawable_draw_to_list (DisplayList *dl);

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
  so_class->creatable = FALSE;
  so_class->autocreate = TRUE;
  so_class->parentable = FALSE;
  so_class->get_drawables = world_get_drawables;

  world_init_params (object_class);
}

static void
world_init (World *world)
{
  world->drawables = NULL;
  world->ground = ground_drawable_new ((SceneObject*) world);
  world->wallsides = wall_sides_drawable_new ((SceneObject*) world);

  world->drawables = g_list_append (world->drawables, (gpointer) world->ground);
  world->drawables = g_list_append (world->drawables, (gpointer) world->wallsides);

  SCENE_OBJECT (world)->bb.position[2] = 4.0;
  SCENE_OBJECT (world)->bb.size[2] = 4.0;
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
  SceneObject *so = SCENE_OBJECT (object);

  switch (prop_id)
  {
    case PROP_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.9);
      so->bb.size[0] = self->param.size[0];
      break;

    case PROP_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[1], 0.9);
      so->bb.size[1] = self->param.size[1];
      break;

    case PROP_GRAVITY:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.gravity, 0.009);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  if (self->state_dirty)
  {
    DISPLAY_LIST (self->ground)->dirty = TRUE;
    DISPLAY_LIST (self->wallsides)->dirty = TRUE;
    g_signal_emit_by_name (object, "dirty");
    g_signal_emit_by_name (G_OBJECT (self->ground), "dirty");
    g_signal_emit_by_name (G_OBJECT (self->wallsides), "dirty");
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

  spec = g_param_spec_double      ("breadth",
                                   "Breadth",
				   "Breadth of the world",
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
    icon = gdk_pixbuf_new_from_file ("data/ground.png", NULL);
  }

  return icon;
}

static GList*
world_get_drawables (SceneObject *self)
{
  World *world = WORLD (self);
  return world->drawables;
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
  Drawable *d = DRAWABLE (gd);

  d->texture = g_strdup ("data/textures/ground.png");
  gd->base_texture_repeat = 4.5;
  gd->overlay_texture_repeat = 1;
}

static void
ground_drawable_draw_to_list (DisplayList *dl)
{
  GroundDrawable *gd = GROUND_DRAWABLE (dl);
  World *w = WORLD (DRAWABLE (dl)->parent);
  gdouble width, height;
  gdouble wrep, hrep;

  width = w->param.size[0];
  height = w->param.size[1];
  wrep = width * 2 / gd->base_texture_repeat;
  hrep = height * 2 / gd->base_texture_repeat;

  glPushMatrix ();

  /* We want to draw both sides of the surface. This will have OpenGL
     automatically flip the surface normals when drawing the back side */
  glDisable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 1);

  glBegin(GL_QUADS);
  glNormal3f (0, 0, 1);

  glTexCoord2d ( wrep,   hrep);
  glVertex3d   ( width,  height, 0);

  glTexCoord2d ( 0,      hrep);
  glVertex3d   (-width,  height, 0);

  glTexCoord2d ( 0,      0);
  glVertex3d   (-width, -height, 0);

  glTexCoord2d ( wrep,   0);
  glVertex3d   ( width, -height, 0);

  glEnd();
  glEnable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 0);
  glPopMatrix ();
}

Drawable*
ground_drawable_new (SceneObject *parent)
{
  Drawable *d = DRAWABLE (g_object_new (ground_drawable_get_type (), NULL));
  d->parent = parent;
  return d;
}

GType
wall_sides_drawable_get_type (void)
{
  static GType wsd_type = 0;
  if (!wsd_type)
  {
    static const GTypeInfo wsd_info =
    {
      sizeof (WallSidesDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    wall_sides_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (WallSidesDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) wall_sides_drawable_init,
    };

    wsd_type = g_type_register_static (DISPLAY_LIST_TYPE, "WallSidesDrawable", &wsd_info, 0);
  }

  return wsd_type;
}

static void
wall_sides_drawable_class_init (WallSidesDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = wall_sides_drawable_draw_to_list;
}

static void
wall_sides_drawable_init (WallSidesDrawable *wsd)
{
  Drawable *d = DRAWABLE (wsd);

  d->texture = g_strdup ("data/textures/outer_wall.jpeg");
}

Drawable*
wall_sides_drawable_new (SceneObject *parent)
{
  Drawable *d = DRAWABLE (g_object_new (wall_sides_drawable_get_type (), NULL));
  d->parent = parent;
  return d;
}

static void
wall_sides_drawable_draw_to_list (DisplayList *dl)
{
  WallSidesDrawable *wsd = WALL_SIDES_DRAWABLE (dl);
  World *w = WORLD (DRAWABLE (dl)->parent);
  gdouble width, depth, height;
  gdouble wrep, hrep;

  width = w->param.size[0];
  depth = w->param.size[1];
  height = 8.0;
  wrep = (width * 2) / height;
  hrep = (depth * 2) / height;

  glPushMatrix ();

  /* We want to draw both sides of the surface. This will have OpenGL
     automatically flip the surface normals when drawing the back side */
  glDisable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 1);

  glBegin (GL_QUADS);
  {
    glNormal3f (0, -1, 0);
    glTexCoord2d ( wrep,  0);
    glVertex3d   ( width, depth, height);
    glTexCoord2d ( wrep,  1);
    glVertex3d   ( width, depth, 0);
    glTexCoord2d ( 0,     1);
    glVertex3d   (-width, depth, 0);
    glTexCoord2d ( 0,     0);
    glVertex3d   (-width, depth, height);

    glNormal3f (1, 0, 0);
    glTexCoord2d ( hrep,   0);
    glVertex3d   (-width,  depth, height);
    glTexCoord2d ( hrep,   1);
    glVertex3d   (-width,  depth, 0);
    glTexCoord2d ( 0,      1);
    glVertex3d   (-width, -depth, 0);
    glTexCoord2d ( 0,      0);
    glVertex3d   (-width, -depth, height);

    glNormal3f (0, 1, 0);
    glTexCoord2d ( 0,      0);
    glVertex3d   (-width, -depth, height);
    glTexCoord2d ( 0,      1);
    glVertex3d   (-width, -depth, 0);
    glTexCoord2d ( wrep,   1);
    glVertex3d   ( width, -depth, 0);
    glTexCoord2d ( wrep,   0);
    glVertex3d   ( width, -depth, height);

    glNormal3f (-1, 0, 0);
    glTexCoord2d ( 0,     0);
    glVertex3d   (width, -depth, height);
    glTexCoord2d ( 0,     1);
    glVertex3d   (width, -depth, 0);
    glTexCoord2d ( hrep,  1);
    glVertex3d   (width,  depth, 0);
    glTexCoord2d ( hrep,  0);
    glVertex3d   (width,  depth, height);
  }
  glEnd ();

  glEnable (GL_CULL_FACE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, 0);
  glPopMatrix ();
}
