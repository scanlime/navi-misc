/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * cell-renderer-transition.c - A GtkCellRenderer for viewing a keyframe's transition,
 *                              including the transition curve and duration.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#include "cell-renderer-transition.h"
#include <gtk/gtk.h>

static void cell_renderer_transition_class_init(CellRendererTransitionClass *klass);
static void cell_renderer_transition_init(CellRendererTransition *self);

static void cell_renderer_transition_finalize      (GObject                  *object);

static void cell_renderer_transition_get_property  (GObject                  *object,
						    guint                     param_id,
						    GValue                   *value,
						    GParamSpec               *pspec);
static void cell_renderer_transition_set_property  (GObject                  *object,
						    guint                     param_id,
						    const GValue             *value,
						    GParamSpec               *pspec);
static void cell_renderer_transition_get_size      (GtkCellRenderer          *cell,
						    GtkWidget                *widget,
						    GdkRectangle             *cell_area,
						    gint                     *x_offset,
						    gint                     *y_offset,
						    gint                     *width,
						    gint                     *height);
static void cell_renderer_transition_render        (GtkCellRenderer          *cell,
						    GdkWindow                *window,
						    GtkWidget                *widget,
						    GdkRectangle             *background_area,
						    GdkRectangle             *cell_area,
						    GdkRectangle             *expose_area,
						    GtkCellRendererState      flags);

static PangoLayout* cell_renderer_transition_get_text_layout (CellRendererTransition *self,
							      GtkWidget              *widget);

static void cell_renderer_transition_render_spline (CellRendererTransition   *self,
						    GdkWindow                *window,
						    GtkWidget                *widget,
						    GtkStateType             state,
						    GdkRectangle             *area);

enum {
    PROP_0,
    PROP_SPLINE,
    PROP_DURATION,
    PROP_SPLINE_SIZE,
};


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType cell_renderer_transition_get_type(void) {
    static GType cr_type = 0;

    if (!cr_type) {
	static const GTypeInfo cr_info = {
	    sizeof(CellRendererTransitionClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) cell_renderer_transition_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(CellRendererTransition),
	    0,
	    (GInstanceInitFunc) cell_renderer_transition_init,
	};

	cr_type = g_type_register_static(GTK_TYPE_CELL_RENDERER, "CellRendererTransition", &cr_info, 0);
    }

    return cr_type;
}


static void cell_renderer_transition_class_init(CellRendererTransitionClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

    object_class->finalize = cell_renderer_transition_finalize;

    object_class->get_property = cell_renderer_transition_get_property;
    object_class->set_property = cell_renderer_transition_set_property;

    cell_class->get_size = cell_renderer_transition_get_size;
    cell_class->render = cell_renderer_transition_render;

    g_object_class_install_property(object_class,
				    PROP_DURATION,
				    g_param_spec_double("duration",
							"Duration",
							"Duration of this keyframe's transition",
							0, G_MAXDOUBLE, 0,
							G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
				    PROP_SPLINE,
				    g_param_spec_boxed("spline",
						       "Spline",
						       "The spline object to draw this transition's curve with",
						       SPLINE_TYPE,
						       G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
				    PROP_SPLINE_SIZE,
				    g_param_spec_uint("spline-size",
						      "Spline size",
						      "The width and height to draw splines at",
						      0, 1000, 96,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void cell_renderer_transition_init(CellRendererTransition *self) {
    /* Set a default spline */
    self->spline = spline_copy(&spline_template_linear);
}

GtkCellRenderer* cell_renderer_transition_new() {
    return GTK_CELL_RENDERER(g_object_new(cell_renderer_transition_get_type(), NULL));
}

static void cell_renderer_transition_finalize(GObject *object) {
    CellRendererTransition *self = CELL_RENDERER_TRANSITION(object);

    if (self->spline) {
	spline_free(self->spline);
	self->spline = NULL;
    }
}



/************************************************************************************/
/*********************************************************************** Properties */
/************************************************************************************/

static void cell_renderer_transition_get_property(GObject    *object,
						  guint       prop_id,
						  GValue     *value,
						  GParamSpec *pspec) {
    CellRendererTransition *self = CELL_RENDERER_TRANSITION(object);

    switch (prop_id) {

    case PROP_SPLINE:
	g_value_set_boxed(value, self->spline);
	break;

    case PROP_DURATION:
	g_value_set_double(value, self->duration);
	break;

    case PROP_SPLINE_SIZE:
	g_value_set_uint(value, self->spline_size);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void cell_renderer_transition_set_property(GObject       *object,
						  guint          prop_id,
						  const GValue  *value,
						  GParamSpec    *pspec) {

    CellRendererTransition *self = CELL_RENDERER_TRANSITION(object);

    switch (prop_id) {

    case PROP_SPLINE:
	if (self->spline)
	    spline_free(self->spline);
	self->spline = spline_copy(g_value_get_boxed(value));
	break;

    case PROP_DURATION:
	self->duration = g_value_get_double(value);
	break;

    case PROP_SPLINE_SIZE:
	self->spline_size = g_value_get_uint(value);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}


/************************************************************************************/
/********************************************************** GtkCellRenderer Methods */
/************************************************************************************/

static void cell_renderer_transition_get_size(GtkCellRenderer  *cell,
					      GtkWidget        *widget,
					      GdkRectangle     *cell_area,
					      gint             *x_offset,
					      gint             *y_offset,
					      gint             *width,
					      gint             *height) {
    CellRendererTransition *self = CELL_RENDERER_TRANSITION(cell);
    PangoLayout *layout = cell_renderer_transition_get_text_layout(self, widget);
    PangoRectangle text_rect;

    pango_layout_get_pixel_extents(layout, NULL, &text_rect);

    if (width)
	*width = GTK_CELL_RENDERER(self)->xpad * 2 + MAX(text_rect.width, self->spline_size);
    if (height)
	*height = GTK_CELL_RENDERER(self)->ypad * 2 + text_rect.height + self->spline_size;

    g_object_unref(layout);
}

static void cell_renderer_transition_render(GtkCellRenderer      *cell,
					    GdkWindow            *window,
					    GtkWidget            *widget,
					    GdkRectangle         *background_area,
					    GdkRectangle         *cell_area,
					    GdkRectangle         *expose_area,
					    GtkCellRendererState  flags) {
    CellRendererTransition *self = CELL_RENDERER_TRANSITION(cell);
    PangoLayout *layout = cell_renderer_transition_get_text_layout(self, widget);
    GtkStateType state;
    PangoRectangle text_rect;
    GdkRectangle spline_rect;

    /* Determine the correct state to render our text in, based on
     * the cell's selectedness and the widget's current state.
     * This was copied from GtkCellRendererText.
     */
    if ((flags & GTK_CELL_RENDERER_SELECTED) == GTK_CELL_RENDERER_SELECTED) {
	if (GTK_WIDGET_HAS_FOCUS (widget))
	    state = GTK_STATE_SELECTED;
	else
	    state = GTK_STATE_ACTIVE;
    }
    else {
	if (GTK_WIDGET_STATE (widget) == GTK_STATE_INSENSITIVE)
	    state = GTK_STATE_INSENSITIVE;
	else
	    state = GTK_STATE_NORMAL;
    }

    /* Calculate width and height for the spline and text */
    pango_layout_get_pixel_extents(layout, NULL, &text_rect);
    spline_rect.width = self->spline_size;
    spline_rect.height = self->spline_size;

    /* Center the spline and text, with the text just above the spline */
    text_rect.x = spline_rect.x = cell_area->x + (cell_area->width - spline_rect.width)/2;
    text_rect.y = cell_area->y + (cell_area->height - spline_rect.height - text_rect.height)/2;
    spline_rect.y = text_rect.y + text_rect.height;

    gtk_paint_layout(widget->style, window, state, TRUE, cell_area, widget,
		     "cellrenderertransition", text_rect.x, text_rect.y, layout);

    gdk_draw_rectangle(window, GTK_WIDGET(widget)->style->dark_gc[state], FALSE,
		       spline_rect.x, spline_rect.y, spline_rect.width, spline_rect.height);
    cell_renderer_transition_render_spline(self, window, widget, state, &spline_rect);

    g_object_unref(layout);
}


/************************************************************************************/
/***************************************************************** Internal Methods */
/************************************************************************************/

static PangoLayout* cell_renderer_transition_get_text_layout (CellRendererTransition *self,
							      GtkWidget              *widget) {
    /* Create and return a PangoLayout with the cell's text
     */
    PangoLayout *layout;
    gchar *text;

    text = g_strdup_printf("%.02f s", (gfloat) self->duration);

    layout = gtk_widget_create_pango_layout(widget, text);
    g_free(text);

    pango_layout_set_width (layout, -1);

    return layout;
}

static void cell_renderer_transition_render_spline (CellRendererTransition  *self,
						    GdkWindow               *window,
						    GtkWidget               *widget,
						    GtkStateType             state,
						    GdkRectangle            *area) {
    GtkStyle *style = GTK_WIDGET(widget)->style;
    gfloat *interpolated;
    GdkPoint *points;
    int i;

    /* Get enough points to have one per column of pixels */
    interpolated = g_malloc(sizeof(gfloat) * area->width);
    spline_solve_and_eval_all(self->spline, area->width, interpolated);

    /* Map those into our area rectangle */
    points = g_malloc(sizeof(GdkPoint) * area->width);
    for (i=0; i<area->width; i++) {
	points[i].x = area->x + area->width * i / area->width;
	points[i].y = area->y + area->height * (1 - interpolated[i]);
    }

    gdk_draw_lines(window, style->fg_gc[state], points, area->width);

    g_free(interpolated);
    g_free(points);
}


/* The End */
