/*
 * light.h - Abstraction for an OpenGL light's position and intensity
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

#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <gtk/gtk.h>
#include <GL/gl.h>

G_BEGIN_DECLS

#define LIGHT_TYPE            (light_get_type ())
#define LIGHT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIGHT_TYPE, Light))
#define LIGHT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), LIGHT_TYPE, LightClass))
#define IS_LIGHT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIGHT_TYPE))
#define IS_LIGHT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), LIGHT_TYPE))

typedef struct _Light      Light;
typedef struct _LightClass LightClass;

struct _Light
{
  GObject parent;

  gboolean enabled;
  gfloat ambient[4];
  gfloat diffuse[4];
  gfloat position[4];
  guint lightnum;
};

struct _LightClass
{
  GObjectClass parent_class;
};

GType  light_get_type (void) G_GNUC_CONST;
Light* light_new      (guint lightnum);
void   light_reset    (Light* light);
void   light_set      (Light* light);

G_END_DECLS

#endif
