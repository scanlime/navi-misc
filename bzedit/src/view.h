/*
 * view.h - A generic 3D view with multiple rendering passes
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

#ifndef __VIEW_H__
#define __VIEW_H__

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <GL/gl.h>
#include "scene.h"
#include "camera.h"
#include "light.h"
#include "gldrawingarea.h"

G_BEGIN_DECLS

#define VIEW_TYPE            (view_get_type ())
#define VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), VIEW_TYPE, View))
#define VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), VIEW_TYPE, ViewClass))
#define IS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VIEW_TYPE))
#define IS_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VIEW_TYPE))

typedef struct _View      View;
typedef struct _ViewClass ViewClass;

struct _View
{
  GtkVBox parent;

  Camera *camera;
  Scene *scene;
  Light **lights;
  guint nlights;
  GLDrawingArea *context;
  GladeXML *xml;

  gdouble mouse[2];
};

struct _ViewClass
{
  GtkVBoxClass parent_class;
};

GType        view_get_type (void) G_GNUC_CONST;
GtkWidget*   view_new      (Scene *scene, GLDrawingArea *context);
void         view_render   (View *view);
SceneObject* view_pick     (View *view, guint pos[2]);

G_END_DECLS

#endif
