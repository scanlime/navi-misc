/*
 * gldrawingarea.h - A Widget for drawing OpenGL stuff, based on gtkglext
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

#ifndef __GL_DRAWING_AREA_H__
#define __GL_DRAWING_AREA_H__

#include <gtk/gtkdrawingarea.h>
#include <gtk/gtkgl.h>

G_BEGIN_DECLS

#define GL_DRAWING_AREA_TYPE		(gl_drawing_area_get_type ())
#define GL_DRAWING_AREA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GL_DRAWING_AREA_TYPE, GLDrawingArea))
#define GL_DRAWING_AREA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GL_DRAWING_AREA_TYPE, GLDrawingAreaClass))
#define IS_GL_DRAWING_AREA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GL_DRAWING_AREA_TYPE))
#define IS_GL_DRAWING_AREA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GL_DRAWING_AREA_TYPE))
#define GL_DRAWING_AREA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GL_DRAWING_AREA_TYPE, GLDrawingAreaClass))

typedef struct _GLDrawingArea		GLDrawingArea;
typedef struct _GLDrawingAreaClass	GLDrawingAreaClass;

struct _GLDrawingArea
{
  GtkDrawingArea widget;

  GdkGLConfig *config;
  GdkGLContext *context;
  GdkGLDrawable *gldrawable;
};

struct _GLDrawingAreaClass
{
  GtkDrawingAreaClass parent_class;
};

GType      gl_drawing_area_get_type     (void) G_GNUC_CONST;
GtkWidget* gl_drawing_area_new          (GdkGLConfig *config);
GtkWidget* gl_drawing_area_new_shared   (GLDrawingArea *base);
void       gl_drawing_area_make_current (GLDrawingArea *glarea);
void       gl_drawing_area_swap_buffers (GLDrawingArea *glarea);

G_END_DECLS

#endif /* __GL_DRAWING_AREA_H__ */
