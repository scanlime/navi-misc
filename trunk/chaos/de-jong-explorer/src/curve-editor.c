/* GTK - The GIMP Toolkit
 * Copyright (C) 1997 David Mosberger
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
 *   - The spline functions were made public, and spline_solve_and_eval was added
 *
 * -- Micah Dowty
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <gtk/gtk.h>
#include "curve-editor.h"

#define RADIUS		3	/* radius of the control points */
#define MIN_DISTANCE	8	/* min distance between control points */

#define GRAPH_MASK	(GDK_EXPOSURE_MASK |		\
			 GDK_POINTER_MOTION_MASK |	\
			 GDK_POINTER_MOTION_HINT_MASK |	\
			 GDK_ENTER_NOTIFY_MASK |	\
			 GDK_BUTTON_PRESS_MASK |	\
			 GDK_BUTTON_RELEASE_MASK |	\
			 GDK_BUTTON1_MOTION_MASK)

static GtkDrawingAreaClass *parent_class = NULL;


/* forward declarations: */
static void curve_editor_class_init   (CurveEditorClass *class);
static void curve_editor_init         (CurveEditor      *curve);
static void curve_editor_finalize     (GObject       *object);
static gint curve_editor_graph_events (GtkWidget     *widget,
				       GdkEvent      *event,
				       CurveEditor   *c);
static void curve_editor_size_graph   (CurveEditor   *curve);
static void curve_editor_get_vector   (CurveEditor   *c,
				       int           veclen,
				       gfloat        vector[]);

enum {
  CHANGED_SIGNAL,
  LAST_SIGNAL
};

static guint curve_editor_signals[LAST_SIGNAL] = { 0 };


GType
curve_editor_get_type (void)
{
  static GType curve_type = 0;

  if (!curve_type)
    {
      static const GTypeInfo curve_info =
      {
	sizeof (CurveEditorClass),
	NULL,		/* base_init */
	NULL,		/* base_finalize */
	(GClassInitFunc) curve_editor_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	sizeof (CurveEditor),
	0,		/* n_preallocs */
	(GInstanceInitFunc) curve_editor_init,
      };

      curve_type = g_type_register_static (GTK_TYPE_DRAWING_AREA, "CurveEditor",
					   &curve_info, 0);
    }
  return curve_type;
}

static void
curve_editor_class_init (CurveEditorClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
  parent_class = g_type_class_peek_parent (class);

  gobject_class->finalize = curve_editor_finalize;

  curve_editor_signals[CHANGED_SIGNAL] = g_signal_new("changed",
						      G_TYPE_FROM_CLASS(class),
						      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
						      G_STRUCT_OFFSET(CurveEditorClass, curve_editor),
						      NULL,
						      NULL,
						      g_cclosure_marshal_VOID__VOID,
						      G_TYPE_NONE, 0);
}

static void
curve_editor_init (CurveEditor *curve)
{
  gint old_mask;

  curve->cursor_type = GDK_TOP_LEFT_ARROW;
  curve->pixmap = NULL;
  curve->height = 0;
  curve->grab_point = -1;

  curve->num_points = 0;
  curve->point = 0;

  curve->num_ctlpoints = 0;
  curve->ctlpoint = NULL;

  old_mask = gtk_widget_get_events (GTK_WIDGET (curve));
  gtk_widget_set_events (GTK_WIDGET (curve), old_mask | GRAPH_MASK);
  g_signal_connect (curve, "event",
		    G_CALLBACK (curve_editor_graph_events), curve);
}

static int
project (gfloat value, gfloat min, gfloat max, int norm)
{
  return (norm - 1) * ((value - min) / (max - min)) + 0.5;
}

static gfloat
unproject (gint value, gfloat min, gfloat max, int norm)
{
  return value / (gfloat) (norm - 1) * (max - min) + min;
}

/* Solve the tridiagonal equation system that determines the second
   derivatives for the interpolation points.  (Based on Numerical
   Recipies 2nd Edition.) */
void
spline_solve (int n, gfloat x[], gfloat y[], gfloat y2[])
{
  gfloat p, sig, *u;
  gint i, k;

  u = g_malloc ((n - 1) * sizeof (u[0]));

  y2[0] = u[0] = 0.0;	/* set lower boundary condition to "natural" */

  for (i = 1; i < n - 1; ++i)
    {
      sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
      p = sig * y2[i - 1] + 2.0;
      y2[i] = (sig - 1.0) / p;
      u[i] = ((y[i + 1] - y[i])
	      / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]));
      u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
    }

  y2[n - 1] = 0.0;
  for (k = n - 2; k >= 0; --k)
    y2[k] = y2[k] * y2[k + 1] + u[k];

  g_free (u);
}

gfloat
spline_eval (int n, gfloat x[], gfloat y[], gfloat y2[], gfloat val)
{
  gint k_lo, k_hi, k;
  gfloat h, b, a;

  /* do a binary search for the right interval: */
  k_lo = 0; k_hi = n - 1;
  while (k_hi - k_lo > 1)
    {
      k = (k_hi + k_lo) / 2;
      if (x[k] > val)
	k_hi = k;
      else
	k_lo = k;
    }

  h = x[k_hi] - x[k_lo];
  g_assert (h > 0.0);

  a = (x[k_hi] - val) / h;
  b = (val - x[k_lo]) / h;
  return a*y[k_lo] + b*y[k_hi] +
    ((a*a*a - a)*y2[k_lo] + (b*b*b - b)*y2[k_hi]) * (h*h)/6.0;
}

gfloat
spline_solve_and_eval (CurveControlPoint *ctlpoints,
		       gint               num_points,
		       gfloat             val)
{
  /* Solve a spline and evaluate one point from it. This is
   * the most convenient way to interpolate given a set of control
   * points saved from a CurveEditor widget. Note that this
   * assumes all control points are active.
   */
  gfloat *mem, *xv, *yv, *y2v, result;
  gint i;

  /* handle degenerate case */
  if (num_points < 2)
    {
      if (num_points > 0)
	return ctlpoints[0][1];
      else
	return 0;
    }

  mem = g_malloc (3 * num_points * sizeof (gfloat));
  xv  = mem;
  yv  = mem + num_points;
  y2v = mem + 2*num_points;

  for (i=0; i<num_points; i++)
    {
      xv[i] = ctlpoints[i][0];
      yv[i] = ctlpoints[i][1];
    }

  spline_solve (num_points, xv, yv, y2v);
  result = spline_eval (num_points, xv, yv, y2v, val);

  if (result < 0) result = 0;
  if (result > 1) result = 1;

  g_free (mem);
  return result;
}


static void
curve_editor_interpolate (CurveEditor *c, gint width, gint height)
{
  gfloat *vector;
  int i;

  vector = g_malloc (width * sizeof (vector[0]));

  curve_editor_get_vector (c, width, vector);

  c->height = height;
  if (c->num_points != width)
    {
      c->num_points = width;
      if (c->point)
	g_free (c->point);
      c->point = g_malloc (c->num_points * sizeof (c->point[0]));
    }

  for (i = 0; i < width; ++i)
    {
      c->point[i].x = RADIUS + i;
      c->point[i].y = RADIUS + height
	- project (vector[i], 0, 1, height);
    }

  g_free (vector);
}

static void
curve_editor_draw (CurveEditor *c, gint width, gint height)
{
  GtkStateType state;
  GtkStyle *style;
  gint i;

  if (!c->pixmap)
    return;

  if (c->height != height || c->num_points != width)
    curve_editor_interpolate (c, width, height);

  state = GTK_STATE_NORMAL;
  if (!GTK_WIDGET_IS_SENSITIVE (GTK_WIDGET (c)))
    state = GTK_STATE_INSENSITIVE;

  style = GTK_WIDGET (c)->style;

  /* clear the pixmap: */
  gtk_paint_flat_box (style, c->pixmap, GTK_STATE_NORMAL, GTK_SHADOW_NONE,
		      NULL, GTK_WIDGET (c), "curve_bg",
		      0, 0, width + RADIUS * 2, height + RADIUS * 2);
  /* draw the grid lines: (XXX make more meaningful) */
  for (i = 0; i < 5; i++)
    {
      gdk_draw_line (c->pixmap, style->dark_gc[state],
		     RADIUS, i * (height / 4.0) + RADIUS,
		     width + RADIUS, i * (height / 4.0) + RADIUS);
      gdk_draw_line (c->pixmap, style->dark_gc[state],
		     i * (width / 4.0) + RADIUS, RADIUS,
		     i * (width / 4.0) + RADIUS, height + RADIUS);
    }

  gdk_draw_lines (c->pixmap, style->fg_gc[state], c->point, c->num_points);

  for (i = 0; i < c->num_ctlpoints; ++i)
    {
      gint x, y;

      if (c->ctlpoint[i][0] < 0)
	continue;

      x = project (c->ctlpoint[i][0], 0, 1,
		     width);
      y = height -
	project (c->ctlpoint[i][1], 0, 1,
		 height);

      /* draw a bullet: */
      gdk_draw_arc (c->pixmap, style->fg_gc[state], TRUE, x, y,
		    RADIUS * 2, RADIUS*2, 0, 360*64);
    }
  gdk_draw_drawable (GTK_WIDGET (c)->window, style->fg_gc[state], c->pixmap,
		     0, 0, 0, 0, width + RADIUS * 2, height + RADIUS * 2);
}

static gint
curve_editor_graph_events (GtkWidget *widget, GdkEvent *event, CurveEditor *c)
{
  GdkCursorType new_type = c->cursor_type;
  gint i, src, dst, leftbound, rightbound;
  GdkEventButton *bevent;
  GdkEventMotion *mevent;
  GtkWidget *w;
  gint tx, ty;
  gint cx, x, y, width, height;
  gint closest_point = 0;
  gfloat rx, ry, min_x;
  guint distance;
  gint x1, x2, y1, y2;
  gint retval = FALSE;

  w = GTK_WIDGET (c);
  width = w->allocation.width - RADIUS * 2;
  height = w->allocation.height - RADIUS * 2;

  if ((width < 0) || (height < 0))
    return FALSE;

  /*  get the pointer position  */
  gdk_window_get_pointer (w->window, &tx, &ty, NULL);
  x = CLAMP ((tx - RADIUS), 0, width-1);
  y = CLAMP ((ty - RADIUS), 0, height-1);

  min_x = 0;

  distance = ~0U;
  for (i = 0; i < c->num_ctlpoints; ++i)
    {
      cx = project (c->ctlpoint[i][0], min_x, 1, width);
      if ((guint) abs (x - cx) < distance)
	{
	  distance = abs (x - cx);
	  closest_point = i;
	}
    }

  switch (event->type)
    {
    case GDK_CONFIGURE:
      if (c->pixmap)
	g_object_unref (c->pixmap);
      c->pixmap = NULL;
      /* fall through */
    case GDK_EXPOSE:
      if (!c->pixmap)
	c->pixmap = gdk_pixmap_new (w->window,
				    w->allocation.width,
				    w->allocation.height, -1);
      curve_editor_draw (c, width, height);
      break;

    case GDK_BUTTON_PRESS:
      gtk_grab_add (widget);

      bevent = (GdkEventButton *) event;
      new_type = GDK_TCROSS;

      if (distance > MIN_DISTANCE)
	{
	  /* insert a new control point */
	  if (c->num_ctlpoints > 0)
	    {
	      cx = project (c->ctlpoint[closest_point][0], min_x,
			    1, width);
	      if (x > cx)
		++closest_point;
	    }
	  ++c->num_ctlpoints;
	  c->ctlpoint =
	    g_realloc (c->ctlpoint,
		       c->num_ctlpoints * sizeof (*c->ctlpoint));
	  for (i = c->num_ctlpoints - 1; i > closest_point; --i)
	    memcpy (c->ctlpoint + i, c->ctlpoint + i - 1,
		    sizeof (*c->ctlpoint));
	}
      c->grab_point = closest_point;
      c->ctlpoint[c->grab_point][0] =
	unproject (x, min_x, 1, width);
      c->ctlpoint[c->grab_point][1] =
	unproject (height - y, 0, 1, height);

      curve_editor_interpolate (c, width, height);

      curve_editor_draw (c, width, height);
      retval = TRUE;
      break;

    case GDK_BUTTON_RELEASE:
      gtk_grab_remove (widget);

      /* delete inactive points: */
      for (src = dst = 0; src < c->num_ctlpoints; ++src)
	{
	  if (c->ctlpoint[src][0] >= min_x)
	    {
	      memcpy (c->ctlpoint + dst, c->ctlpoint + src,
		      sizeof (*c->ctlpoint));
	      ++dst;
	    }
	}
      if (dst < src)
	{
	  c->num_ctlpoints -= (src - dst);
	  if (c->num_ctlpoints <= 0)
	    {
	      c->num_ctlpoints = 1;
	      c->ctlpoint[0][0] = min_x;
	      c->ctlpoint[0][1] = 0;
	      curve_editor_interpolate (c, width, height);
	      curve_editor_draw (c, width, height);
	    }
	  c->ctlpoint =
	    g_realloc (c->ctlpoint,
		       c->num_ctlpoints * sizeof (*c->ctlpoint));
	}
      new_type = GDK_FLEUR;
      c->grab_point = -1;
      retval = TRUE;
      g_signal_emit(G_OBJECT(c), curve_editor_signals[CHANGED_SIGNAL], 0);
      break;

    case GDK_MOTION_NOTIFY:
      mevent = (GdkEventMotion *) event;

      if (c->grab_point == -1)
	{
	  /* if no point is grabbed...  */
	  if (distance <= MIN_DISTANCE)
	    new_type = GDK_FLEUR;
	  else
	    new_type = GDK_TCROSS;
	}
      else
	{
	  /* drag the grabbed point  */
	  new_type = GDK_TCROSS;

	  leftbound = -MIN_DISTANCE;
	  if (c->grab_point > 0)
	    leftbound = project (c->ctlpoint[c->grab_point - 1][0],
				 min_x, 1, width);

	  rightbound = width + RADIUS * 2 + MIN_DISTANCE;
	  if (c->grab_point + 1 < c->num_ctlpoints)
	    rightbound = project (c->ctlpoint[c->grab_point + 1][0],
				  min_x, 1, width);

	  if (tx <= leftbound || tx >= rightbound
	      || ty > height + RADIUS * 2 + MIN_DISTANCE
	      || ty < -MIN_DISTANCE)
	    c->ctlpoint[c->grab_point][0] = min_x - 1.0;
	  else
	    {
	      rx = unproject (x, min_x, 1, width);
	      ry = unproject (height - y, 0, 1, height);
	      c->ctlpoint[c->grab_point][0] = rx;
	      c->ctlpoint[c->grab_point][1] = ry;
	    }
	  curve_editor_interpolate (c, width, height);
	  curve_editor_draw (c, width, height);
	}

      if (new_type != (GdkCursorType) c->cursor_type)
	{
	  GdkCursor *cursor;

	  c->cursor_type = new_type;

	  cursor = gdk_cursor_new_for_display (gtk_widget_get_display (w),
					      c->cursor_type);
	  gdk_window_set_cursor (w->window, cursor);
	  gdk_cursor_unref (cursor);
	}
      retval = TRUE;
      break;

    default:
      break;
    }

  return retval;
}

void
curve_editor_set_control_points(CurveEditor* self, CurveControlPoint *ctlpoints, gint num_points)
{
  gint i;

  if (self->ctlpoint)
    g_free (self->ctlpoint);

  self->num_ctlpoints = num_points;
  self->ctlpoint = g_malloc (num_points * sizeof (CurveControlPoint));
  memcpy(self->ctlpoint, ctlpoints, num_points * sizeof (CurveControlPoint));

  if (self->pixmap)
    {
      gint width, height;

      width = GTK_WIDGET (self)->allocation.width - RADIUS * 2;
      height = GTK_WIDGET (self)->allocation.height - RADIUS * 2;

      curve_editor_interpolate (self, width, height);
      curve_editor_draw (self, width, height);
    }
  g_signal_emit(G_OBJECT(self), curve_editor_signals[CHANGED_SIGNAL], 0);
}

CurveControlPoint*
curve_editor_get_control_points(CurveEditor* self, gint *num_points)
{
  *num_points = self->num_ctlpoints;
  return self->ctlpoint;
}

static void
curve_editor_get_vector (CurveEditor *c, int veclen, gfloat vector[])
{
  gfloat rx, ry, dx, dy, min_x, delta_x, *mem, *xv, *yv, *y2v, prev;
  gint dst, i, x, next, num_active_ctlpoints = 0, first_active = -1;

  min_x = 0;

  /* count active points: */
  prev = min_x - 1.0;
  for (i = num_active_ctlpoints = 0; i < c->num_ctlpoints; ++i)
    if (c->ctlpoint[i][0] > prev)
      {
	if (first_active < 0)
	  first_active = i;
	prev = c->ctlpoint[i][0];
	++num_active_ctlpoints;
      }

  /* handle degenerate case: */
  if (num_active_ctlpoints < 2)
    {
      if (num_active_ctlpoints > 0)
	ry = c->ctlpoint[first_active][1];
      else
	ry = 0;
      if (ry < 0) ry = 0;
      if (ry > 1) ry = 1;
      for (x = 0; x < veclen; ++x)
	vector[x] = ry;
      return;
    }

  mem = g_malloc (3 * num_active_ctlpoints * sizeof (gfloat));
  xv  = mem;
  yv  = mem + num_active_ctlpoints;
  y2v = mem + 2*num_active_ctlpoints;

  prev = min_x - 1.0;
  for (i = dst = 0; i < c->num_ctlpoints; ++i)
    if (c->ctlpoint[i][0] > prev)
      {
	prev    = c->ctlpoint[i][0];
	xv[dst] = c->ctlpoint[i][0];
	yv[dst] = c->ctlpoint[i][1];
	++dst;
      }

  spline_solve (num_active_ctlpoints, xv, yv, y2v);

  rx = min_x;
  dx = (1 - min_x) / (veclen - 1);
  for (x = 0; x < veclen; ++x, rx += dx)
    {
      ry = spline_eval (num_active_ctlpoints, xv, yv, y2v, rx);
      if (ry < 0) ry = 0;
      if (ry > 1) ry = 1;
      vector[x] = ry;
    }

  g_free (mem);
}

GtkWidget*
curve_editor_new (void)
{
  return g_object_new (TYPE_CURVE_EDITOR, NULL);
}

static void
curve_editor_finalize (GObject *object)
{
  CurveEditor *curve;

  g_return_if_fail (IS_CURVE_EDITOR (object));

  curve = CURVE_EDITOR(object);
  if (curve->pixmap)
    g_object_unref (curve->pixmap);
  if (curve->point)
    g_free (curve->point);
  if (curve->ctlpoint)
    g_free (curve->ctlpoint);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* The End */
