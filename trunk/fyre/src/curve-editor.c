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
 * It has been modified in many ways, see curve-editor.h for more information.
 */

#include <stdlib.h>
#include <string.h>

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

  curve->spline.num_points = 0;
  curve->spline.points = NULL;

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

static void
curve_editor_interpolate (CurveEditor *c, gint width, gint height)
{
  gfloat *vector;
  int i;
  Spline *active = spline_find_active_points(&c->spline);

  vector = g_malloc (width * sizeof (vector[0]));

  spline_solve_and_eval_all(active, width, vector);
  g_free(active);

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
  if (!GTK_WIDGET_IS_SENSITIVE (GTK_WIDGET (c))) {
    state = GTK_STATE_INSENSITIVE;

    /* Release any grabbed point if we're insensitive.
     * This is important if we become insensitive while the
     * user is dragging.
     */
    c->grab_point = -1;
  }

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

  for (i = 0; i < c->spline.num_points; ++i)
    {
      gint x, y;

      if (c->spline.points[i][0] < 0)
	continue;

      x = project (c->spline.points[i][0], 0, 1,
		   width);
      y = height -
	project (c->spline.points[i][1], 0, 1,
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
  for (i = 0; i < c->spline.num_points; ++i)
    {
      cx = project (c->spline.points[i][0], min_x, 1, width);
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
	  if (c->spline.num_points > 0)
	    {
	      cx = project (c->spline.points[closest_point][0], min_x,
			    1, width);
	      if (x > cx)
		++closest_point;
	    }
	  ++c->spline.num_points;
	  c->spline.points =
	    g_realloc (c->spline.points,
		       c->spline.num_points * sizeof (*c->spline.points));
	  for (i = c->spline.num_points - 1; i > closest_point; --i)
	    memcpy (c->spline.points + i, c->spline.points + i - 1,
		    sizeof (*c->spline.points));
	}
      c->grab_point = closest_point;
      c->spline.points[c->grab_point][0] =
	unproject (x, min_x, 1, width);
      c->spline.points[c->grab_point][1] =
	unproject (height - y, 0, 1, height);

      curve_editor_interpolate (c, width, height);

      curve_editor_draw (c, width, height);
      g_signal_emit(G_OBJECT(c), curve_editor_signals[CHANGED_SIGNAL], 0);
      retval = TRUE;
      break;

    case GDK_BUTTON_RELEASE:
      gtk_grab_remove (widget);

      /* delete inactive points: */
      for (src = dst = 0; src < c->spline.num_points; ++src)
	{
	  if (c->spline.points[src][0] >= min_x)
	    {
	      memcpy (c->spline.points + dst, c->spline.points + src,
		      sizeof (*c->spline.points));
	      ++dst;
	    }
	}
      if (dst < src)
	{
	  c->spline.num_points -= (src - dst);
	  if (c->spline.num_points <= 0)
	    {
	      c->spline.num_points = 1;
	      c->spline.points[0][0] = min_x;
	      c->spline.points[0][1] = 0;
	      curve_editor_interpolate (c, width, height);
	      curve_editor_draw (c, width, height);
	    }
	  c->spline.points =
	    g_realloc (c->spline.points,
		       c->spline.num_points * sizeof (*c->spline.points));
	}
      new_type = GDK_FLEUR;
      c->grab_point = -1;
      retval = TRUE;
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
	    leftbound = project (c->spline.points[c->grab_point - 1][0],
				 min_x, 1, width);

	  rightbound = width + RADIUS * 2 + MIN_DISTANCE;
	  if (c->grab_point + 1 < c->spline.num_points)
	    rightbound = project (c->spline.points[c->grab_point + 1][0],
				  min_x, 1, width);

	  if (tx <= leftbound || tx >= rightbound
	      || ty > height + RADIUS * 2 + MIN_DISTANCE
	      || ty < -MIN_DISTANCE)
	    c->spline.points[c->grab_point][0] = min_x - 1.0;
	  else
	    {
	      rx = unproject (x, min_x, 1, width);
	      ry = unproject (height - y, 0, 1, height);
	      c->spline.points[c->grab_point][0] = rx;
	      c->spline.points[c->grab_point][1] = ry;
	    }
	  curve_editor_interpolate (c, width, height);
	  curve_editor_draw (c, width, height);
	  g_signal_emit(G_OBJECT(c), curve_editor_signals[CHANGED_SIGNAL], 0);
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
curve_editor_set_spline(CurveEditor* self, const Spline *spline)
{
  if (self->spline.points)
    g_free (self->spline.points);

  self->spline.num_points = spline->num_points;
  self->spline.points = g_malloc (spline->num_points * sizeof (SplineControlPoint));
  memcpy(self->spline.points, spline->points, spline->num_points * sizeof (SplineControlPoint));

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

Spline*
curve_editor_get_spline(CurveEditor* self) {
  return spline_find_active_points(&self->spline);
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
  if (curve->spline.points)
    g_free (curve->spline.points);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* The End */
