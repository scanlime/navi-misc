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

#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

#include <gtk/gtk.h>
#include <GL/gl.h>
#include "renderstate.h"

G_BEGIN_DECLS

#define DRAWABLE_TYPE            (drawable_get_type ())
#define DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DRAWABLE_TYPE, Drawable))
#define DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DRAWABLE_TYPE, DrawableClass))
#define IS_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DRAWABLE_TYPE))
#define IS_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DRAWABLE_TYPE))

typedef struct _Drawable      Drawable;
typedef struct _DrawableClass DrawableClass;

typedef struct
{
  gboolean blended;       /* This primitive uses blending, and should be rendered after solid objects */
  gboolean overlay;       /* This object is on top of the scene, rather than part of it */
  gboolean statico;       /* This object doesn't change, so it can be cached in a display list */
  gboolean decal;         /* This object is a decal, to be drawn with a depth offset */
  gboolean camera;        /* This object is a camera-induced effect */
  gboolean reflection;    /* This object is a cube-mapped reflection */
  gboolean background;    /* This object is part of a location-independent background behind the scene */
} RenderSettings;

struct _Drawable
{
  GObject object;

  gchar *texture;
  gpointer parent;
  RenderSettings render;
};

struct _DrawableClass
{
  GObjectClass parent_class;

  void (*draw)  (Drawable *drawable, RenderState *rstate);
  void (*dirty) (Drawable *drawable);
};

GType drawable_get_type (void) G_GNUC_CONST;
void  drawable_draw     (Drawable *drawable, RenderState *rstate);

G_END_DECLS

#endif
