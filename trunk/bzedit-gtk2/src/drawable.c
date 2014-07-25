/*
 * drawable.h - Abstract base class for an object that can be drawn to an OpenGL
 *              context. This is the basic unit used by the view to sort object by
 *              texture, so its required that the drawable have at most one texture.
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

#include "drawable.h"

static void drawable_class_init (DrawableClass *klass);
static void drawable_init       (Drawable *drawable);

enum
{
  DIRTY,
  LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

GType
drawable_get_type (void)
{
  static GType drawable_type = 0;

  if (!drawable_type)
    {
      static const GTypeInfo drawable_info =
      {
	sizeof (DrawableClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    drawable_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (Drawable),
	0,                  /* n preallocs */
	(GInstanceInitFunc) drawable_init,
      };

      drawable_type = g_type_register_static (G_TYPE_OBJECT, "Drawable", &drawable_info, 0);
    }

  return drawable_type;
}

static void
drawable_class_init (DrawableClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  signals[DIRTY] = g_signal_new ("dirty", G_OBJECT_CLASS_TYPE (klass),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				 G_STRUCT_OFFSET (DrawableClass, dirty),
				 NULL, NULL,
				 g_cclosure_marshal_VOID__VOID,
				 G_TYPE_NONE, 0);
}

static void
drawable_init (Drawable *drawable)
{
  drawable->texture = NULL;

  drawable->render.blended    = FALSE;
  drawable->render.overlay    = FALSE;
  drawable->render.statico    = TRUE;
  drawable->render.decal      = FALSE;
  drawable->render.camera     = FALSE;
  drawable->render.reflection = FALSE;
  drawable->render.background = FALSE;
}

void
drawable_draw (Drawable *drawable, RenderState *rstate)
{
  DrawableClass *class = DRAWABLE_CLASS (G_OBJECT_GET_CLASS (drawable));
  class->draw (drawable, rstate);
}
