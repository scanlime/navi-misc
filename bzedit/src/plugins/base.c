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
}

static void
base_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
base_finalize (GObject *object)
{
}

static void
base_init_position_params (GObjectClass *object_class)
{
}

static void
base_init_size_params (GObjectClass *object_class)
{
}

static void
base_init_other_params (GObjectClass *object_class)
{
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
