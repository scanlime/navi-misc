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

/* These spline solving and evaluation functions were separated from GtkCurve.
 * See the CurveEditor widget for more information.
 */

#ifndef __SPLINE_H__
#define __SPLINE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define SPLINE_TYPE  (spline_get_type ())

typedef gfloat SplineControlPoint[2];

typedef struct {
  SplineControlPoint *points;
  gint num_points;
} Spline;


extern const Spline spline_template_linear;
extern const Spline spline_template_smooth;


GType    spline_get_type(void);
Spline*  spline_copy(const Spline *spline);
void     spline_free(Spline *spline);

guchar* spline_serialize(Spline *spline, gsize *size);
Spline* spline_unserialize(const guchar *data, gsize size);

Spline* spline_find_active_points(Spline *spline);

/* Solve the spline and evaluate one point on it */
gfloat  spline_solve_and_eval(Spline *spline, gfloat val);

/* Solve the spline and fill a provided vector with
 * points evaluated from the given range.
 */
void    spline_solve_and_eval_range(Spline *spline,
				    int     veclen,
				    gfloat  vector[],
				    float   lower,
				    float   upper);

void    spline_solve_and_eval_all(Spline *spline,
				  int     veclen,
				  gfloat  vector[]);

void    spline_solve(Spline *spline, gfloat y2[]);
gfloat  spline_eval(Spline *spline, gfloat y2[], gfloat val);

G_END_DECLS

#endif /* __CURVE_EDITOR_H__ */

/* The End */
