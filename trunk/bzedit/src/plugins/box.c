/*
 * box.c - Definition for the box object
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

#include "box.h"

static void       box_class_init                  (BoxClass *klass);
static void       box_init                        (Box *box);
static void       box_set_property                (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void       box_get_property                (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void       box_finalize                    (GObject *object);
static void       box_init_position_params        (GObjectClass *object_class);
static void       box_init_size_params            (GObjectClass *object_class);
static void       box_init_other_params           (GObjectClass *object_class);
static GdkPixbuf* box_get_icon                    (void);
static GList*     box_get_drawables               (SceneObject *self);

static void       box_sides_drawable_class_init   (BoxSidesDrawableClass *klass);
static void       box_sides_drawable_init         (BoxSidesDrawable *bsd);
static void       box_sides_drawable_draw_to_list (DisplayList *dl);

static void       box_top_drawable_class_init     (BoxTopDrawableClass *klass);
static void       box_top_drawable_init           (BoxTopDrawable *btd);
static void       box_top_drawable_draw_to_list   (DisplayList *dl);

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
  PROP_DRIVE_THROUGH,
  PROP_SHOOT_THROUGH,
};

static gpointer parent_class = NULL;

GType
box_get_type (void)
{
  static GType box_type = 0;

  if (!box_type)
    {
      static const GTypeInfo box_info =
      {
	sizeof (BoxClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    box_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Box),
	0,                  /* n preallocs */
	(GInstanceInitFunc) box_init,
      };

      box_type = g_type_register_static (SCENE_OBJECT_TYPE, "Box", &box_info, 0);
    }

  return box_type;
}

static void
box_class_init (BoxClass *klass)
{
  GObjectClass *object_class;
  SceneObjectClass *so_class;

  parent_class = g_type_class_ref (SCENE_OBJECT_TYPE);
  object_class = (GObjectClass*) klass;
  so_class = (SceneObjectClass*) klass;

  so_class->get_icon = box_get_icon;
  so_class->creatable = TRUE;
  so_class->get_drawables = box_get_drawables;

  object_class->set_property = box_set_property;
  object_class->get_property = box_get_property;
  object_class->finalize = box_finalize;

  box_init_position_params (object_class);
  box_init_size_params (object_class);
  box_init_other_params (object_class);
}

static void
box_init (Box *box)
{
  box->drawables = NULL;
  box->sides = g_object_ref (box_sides_drawable_new ());
  box->top = g_object_ref (box_top_drawable_new ());

  box->drawables = g_list_append (box->drawables, (gpointer) box->sides);
  box->drawables = g_list_append (box->drawables, (gpointer) box->top);
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
box_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  Box *self = BOX (object);

  switch (prop_id)
  {
    case PROP_POSITION_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[0], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->position[0], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->position[0], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_POSITION_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[1], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->position[1], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->position[1], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_POSITION_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.position[2], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->position[2], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->position[2], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_ROTATION:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.rotation, 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->rotation, 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->rotation, 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_SIZE_X:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[0], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->size[0], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->size[0], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_SIZE_Y:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[1], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->size[1], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->size[1], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
      break;

    case PROP_SIZE_Z:
      update_double_if_necessary (g_value_get_double (value), &self->state_dirty, &self->param.size[2], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->sides)->dirty,
                                  &BOX_SIDES_DRAWABLE (self->sides)->size[2], 0.09);
      update_float_if_necessary  (g_value_get_double (value), &DISPLAY_LIST (self->top)->dirty,
                                  &BOX_TOP_DRAWABLE (self->top)->size[2], 0.09);
      if (self->state_dirty)
	g_signal_emit_by_name (object, "dirty");
      if (DISPLAY_LIST (self->sides)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->sides), "dirty");
      if (DISPLAY_LIST (self->top)->dirty)
	g_signal_emit_by_name (G_OBJECT (self->top), "dirty");
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
box_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  Box *self = BOX (object);

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

    case PROP_DRIVE_THROUGH:
      g_value_set_boolean(value, self->param.drive_through);
      break;

    case PROP_SHOOT_THROUGH:
      g_value_set_boolean(value, self->param.shoot_through);
      break;
  }
}

static void
box_finalize (GObject *object)
{
}

static void
box_init_position_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Position";

  spec = g_param_spec_double       ("x",
                                   "X",
			           "Position of the box along the X axis",
			           -1000, 1000, 0,
			           G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_X, spec);

  spec = g_param_spec_double      ("y",
                                   "Y",
				   "Position of the box along the Y axis",
				   -1000, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Y, spec);

  spec = g_param_spec_double      ("z",
                                   "Z",
				   "Position of the box along the Z axis",
				   0, 1000, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_POSITION_Z, spec);

  spec = g_param_spec_double      ("rotation",
                                   "Rotation",
				   "Rotation of the box around it's origin",
				   0, 360, 0,
				   G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
				   G_PARAM_LAX_VALIDATION | PARAM_IN_GUI);
  param_spec_set_group            (spec, current_group);
  param_spec_set_increments       (spec, 0.1, 1, 2);
  g_object_class_install_property (object_class, PROP_ROTATION, spec);
}

static void
box_init_size_params (GObjectClass *object_class)
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
box_init_other_params (GObjectClass *object_class)
{
  GParamSpec *spec;
  const gchar *current_group = "Doopy";

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

Box*
box_new (void)
{
  return BOX (g_object_new (box_get_type (), NULL));
}

static GdkPixbuf*
box_get_icon (void)
{
  static GdkPixbuf *icon = NULL;

  if (icon == NULL)
  {
    icon = gdk_pixbuf_new_from_file ("data/box.png", NULL);
  }

  return gdk_pixbuf_ref (icon);
}

static GList*
box_get_drawables (SceneObject *self)
{
  Box *b = BOX (self);
  return b->drawables;
}

GType
box_sides_drawable_get_type (void)
{
  static GType bsd_type = 0;
  if (!bsd_type)
  {
    static const GTypeInfo bsd_info =
    {
      sizeof (BoxSidesDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    box_sides_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (BoxSidesDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) box_sides_drawable_init,
    };

    bsd_type = g_type_register_static (DISPLAY_LIST_TYPE, "BoxSidesDrawable", &bsd_info, 0);
  }

  return bsd_type;
}

static void
box_sides_drawable_class_init (BoxSidesDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = box_sides_drawable_draw_to_list;
}

static void
box_sides_drawable_init (BoxSidesDrawable *bsd)
{
  Drawable *d = DRAWABLE (bsd);

  d->texture = g_strdup ("data/textures/brick.jpeg");
}

Drawable*
box_sides_drawable_new (void)
{
  return DRAWABLE (g_object_new (box_sides_drawable_get_type (), NULL));
}

static void
box_sides_drawable_draw_to_list (DisplayList *dl)
{
  BoxSidesDrawable *bsd = BOX_SIDES_DRAWABLE (dl);
  float width, depth, height;
  float wrep, drep, hrep;

  width = bsd->size[0];
  depth = bsd->size[1];
  height = bsd->size[2];

  wrep = width / 5;
  drep = depth / 5;
  hrep = height / 5;

  glPushMatrix ();
  glTranslatef (bsd->position[0], bsd->position[1], bsd->position[2]);
  glRotatef (bsd->rotation, 0.0, 0.0, 1.0);

  glBegin (GL_QUADS);
  {
    /* Y+ side */
    glNormal3f (0.0, 1.0, 0.0);
    glTexCoord2f ( 0,     0);
    glVertex3f   (-width, depth, 0);
    glTexCoord2f ( 0,     hrep);
    glVertex3f   (-width, depth, height);
    glTexCoord2f (wrep,   hrep);
    glVertex3f   ( width, depth, height);
    glTexCoord2f (wrep,   0);
    glVertex3f   ( width, depth, 0);

    /* Y- side */
    glNormal3f (0.0, -1.0, 0.0);
    glTexCoord2f (wrep, 0);
    glVertex3f   ( width, -depth, 0);
    glTexCoord2f (wrep, hrep);
    glVertex3f   ( width, -depth, height);
    glTexCoord2f ( 0,      hrep);
    glVertex3f   (-width, -depth, height);
    glTexCoord2f ( 0,      0);
    glVertex3f   (-width, -depth, 0);

    /* X+ side */
    glNormal3f (1.0, 0.0, 0.0);
    glTexCoord2f (0,      0);
    glVertex3f   (width,  depth, 0);
    glTexCoord2f (0,      hrep);
    glVertex3f   (width,  depth, height);
    glTexCoord2f (drep,   hrep);
    glVertex3f   (width, -depth, height);
    glTexCoord2f (drep,   0);
    glVertex3f   (width, -depth, 0);

    /* X- side */
    glNormal3f (-1.0, 0.0, 0.0);
    glTexCoord2f ( 0,      0);
    glVertex3f   (-width, -depth, 0);
    glTexCoord2f ( 0,      hrep);
    glVertex3f   (-width, -depth, height);
    glTexCoord2f ( drep,   hrep);
    glVertex3f   (-width,  depth, height);
    glTexCoord2f ( drep,   0);
    glVertex3f   (-width,  depth, 0);
  }
  glEnd ();

  glPopMatrix();
}

GType
box_top_drawable_get_type (void)
{
  static GType btd_type = 0;
  if (!btd_type)
  {
    static const GTypeInfo btd_info =
    {
      sizeof (BoxTopDrawableClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    box_top_drawable_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (BoxTopDrawable),
      0,                  /* n preallocs */
      (GInstanceInitFunc) box_top_drawable_init,
    };

    btd_type = g_type_register_static (DISPLAY_LIST_TYPE, "BoxTopDrawable", &btd_info, 0);
  }

  return btd_type;
}

static void
box_top_drawable_class_init (BoxTopDrawableClass *klass)
{
  DisplayListClass *dlc;

  dlc = (DisplayListClass*) klass;
  dlc->draw_to_list = box_top_drawable_draw_to_list;
}

static void
box_top_drawable_init (BoxTopDrawable *btd)
{
  Drawable *d = DRAWABLE (btd);

  d->texture = g_strdup ("data/textures/concrete.jpeg");
}

Drawable*
box_top_drawable_new (void)
{
  return DRAWABLE (g_object_new (box_top_drawable_get_type (), NULL));
}

static void
box_top_drawable_draw_to_list (DisplayList *dl)
{
  BoxTopDrawable *btd = BOX_TOP_DRAWABLE (dl);
  float width, depth, height;
  float wrep, hrep;

  width = btd->size[0];
  depth = btd->size[1];
  height = btd->size[2];
  wrep = width / 20;
  hrep = height / 20;

  glPushMatrix ();
  glTranslatef (btd->position[0], btd->position[1], btd->position[2]);
  glRotatef (btd->rotation, 0.0, 0.0, 1.0);

  /* FIXME: should align texcoords with the world */
  glBegin (GL_QUADS);
  {
    /* Z+ side */
    glNormal3f (0.0, 0.0, 1.0);
    glTexCoord2f ( wrep,   0);
    glVertex3f   ( width, -depth, height);
    glTexCoord2f ( wrep,   hrep);
    glVertex3f   ( width,  depth, height);
    glTexCoord2f ( 0,      hrep);
    glVertex3f   (-width,  depth, height);
    glTexCoord2f ( 0,      0);
    glVertex3f   (-width, -depth, height);

    /* Z- side */
    glNormal3f (0.0, 0.0, -1.0);
    glTexCoord2f ( 0,      0);
    glVertex3f   (-width, -depth, 0);
    glTexCoord2f ( 0,      hrep);
    glVertex3f   (-width,  depth, 0);
    glTexCoord2f ( wrep,   hrep);
    glVertex3f   ( width,  depth, 0);
    glTexCoord2f ( wrep,   0);
    glVertex3f   ( width, -depth, 0);
  }
  glEnd ();

  glPopMatrix ();
}
