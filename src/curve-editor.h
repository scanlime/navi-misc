/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

/* This was originally the GtkCurve widget from the GTK+ 2.3.2 release.
 * It has been modified in two major ways:
 *
 *   - It has been simplified by removing support for linear and free-form curves,
 *     and support for user-definable X and Y range. The range is always [0,1] now.
 *
 *   - Functions allowing access to the spline itself have been added, replacing
 *     the functions that only allowed access to interpolated points.
 *
 *   - The spline functions were split into a separate boxed type and source file.
 *     An easy evaluation function and serialization functions were added.
 *
 * -- Micah Dowty
 */

#ifndef __CURVE_EDITOR_H__
#define __CURVE_EDITOR_H__


#include <gdk/gdk.h>
#include <gtk/gtkdrawingarea.h>
#include "spline.h"

G_BEGIN_DECLS

#define TYPE_CURVE_EDITOR               (curve_editor_get_type ())
#define CURVE_EDITOR(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CURVE_EDITOR, CurveEditor))
#define CURVE_EDITOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_CURVE_EDITOR, CurveEditorClass))
#define IS_CURVE_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CURVE_EDITOR))
#define IS_CURVE_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_CURVE_EDITOR))
#define CURVE_EDITOR_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CURVE_EDITOR, CurveEditorClass))


typedef struct _CurveEditor   	  CurveEditor;
typedef struct _CurveEditorClass  CurveEditorClass;

struct _CurveEditor
{
  GtkDrawingArea parent;

  gint cursor_type;
  GdkPixmap *pixmap;
  gint height;                  /* (cached) graph height in pixels */
  gint grab_point;              /* point currently grabbed */
  gint last;

  /* (cached) curve points: */
  gint num_points;
  GdkPoint *point;

  Spline spline;
};

struct _CurveEditorClass
{
  GtkDrawingAreaClass parent_class;

  void (* curve_editor) (CurveEditor *cb);
};


GType		curve_editor_get_type	(void);
GtkWidget*	curve_editor_new	(void);

void     curve_editor_set_spline(CurveEditor* self, const Spline *spline);
Spline*  curve_editor_get_spline(CurveEditor* self);

G_END_DECLS

#endif /* __CURVE_EDITOR_H__ */

/* The End */
