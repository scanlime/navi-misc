/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * cell-renderer-bifurcation.h - A GtkCellRenderer for viewing a bifurcation
 *                               diagram over the range of a keyframe's transition.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include "cell-renderer-bifurcation.h"
#include "bifurcation-diagram.h"
#include <gtk/gtk.h>

static void cell_renderer_bifurcation_class_init    (CellRendererBifurcationClass *klass);
static void cell_renderer_bifurcation_init          (CellRendererBifurcation  *self);

static void cell_renderer_bifurcation_finalize      (GObject                  *object);

static void cell_renderer_bifurcation_get_property  (GObject                  *object,
						     guint                     param_id,
						     GValue                   *value,
						     GParamSpec               *pspec);
static void cell_renderer_bifurcation_set_property  (GObject                  *object,
						     guint                     param_id,
						     const GValue             *value,
						     GParamSpec               *pspec);
static void cell_renderer_bifurcation_get_size      (GtkCellRenderer          *cell,
						     GtkWidget                *widget,
						     GdkRectangle             *cell_area,
						     gint                     *x_offset,
						     gint                     *y_offset,
						     gint                     *width,
						     gint                     *height);
static void cell_renderer_bifurcation_render        (GtkCellRenderer          *cell,
						     GdkWindow                *window,
						     GtkWidget                *widget,
						     GdkRectangle             *background_area,
						     GdkRectangle             *cell_area,
						     GdkRectangle             *expose_area,
						     GtkCellRendererState      flags);

static BifurcationDiagram* get_bifurcation_diagram  (CellRendererBifurcation  *self);

enum {
    PROP_0,
    PROP_ROW_ID,
    PROP_ANIMATION,
};

typedef struct {
    GtkTreeView *view;
    GdkRectangle rect;
} RedrawInfo;


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType cell_renderer_bifurcation_get_type(void) {
    static GType cr_type = 0;

    if (!cr_type) {
	static const GTypeInfo cr_info = {
	    sizeof(CellRendererBifurcationClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) cell_renderer_bifurcation_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(CellRendererBifurcation),
	    0,
	    (GInstanceInitFunc) cell_renderer_bifurcation_init,
	};

	cr_type = g_type_register_static(GTK_TYPE_CELL_RENDERER, "CellRendererBifurcation", &cr_info, 0);
    }

    return cr_type;
}


static void cell_renderer_bifurcation_class_init(CellRendererBifurcationClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

    object_class->finalize = cell_renderer_bifurcation_finalize;

    object_class->get_property = cell_renderer_bifurcation_get_property;
    object_class->set_property = cell_renderer_bifurcation_set_property;

    cell_class->get_size = cell_renderer_bifurcation_get_size;
    cell_class->render = cell_renderer_bifurcation_render;

    g_object_class_install_property(object_class,
				    PROP_ROW_ID,
				    g_param_spec_ulong("row-id",
						       "Row Id",
						       "A row ID pointing to the keyframe this diagram starts at",
						       0, (gulong)-1, 1,
						       G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
				    PROP_ANIMATION,
				    g_param_spec_object("animation",
							"Animation",
							"The animation this bifurcation diagram gets its keyframes from",
							G_TYPE_OBJECT,
							G_PARAM_READWRITE));
}

static void cell_renderer_bifurcation_init(CellRendererBifurcation *self) {
}

GtkCellRenderer* cell_renderer_bifurcation_new() {
    return GTK_CELL_RENDERER(g_object_new(cell_renderer_bifurcation_get_type(), NULL));
}

static void cell_renderer_bifurcation_finalize(GObject *object) {
    CellRendererBifurcation *self = CELL_RENDERER_BIFURCATION(object);

    if (self->animation) {
	g_object_unref(self->animation);
	self->animation = NULL;
    }
}


/************************************************************************************/
/*********************************************************************** Properties */
/************************************************************************************/

static void cell_renderer_bifurcation_get_property(GObject    *object,
						   guint       prop_id,
						   GValue     *value,
						   GParamSpec *pspec) {
    CellRendererBifurcation *self = CELL_RENDERER_BIFURCATION(object);

    switch (prop_id) {

    case PROP_ROW_ID:
	g_value_set_ulong(value, self->row_id);
	break;

    case PROP_ANIMATION:
	g_value_set_object(value, &self->animation);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void cell_renderer_bifurcation_set_property(GObject       *object,
						   guint          prop_id,
						   const GValue  *value,
						   GParamSpec    *pspec) {
    CellRendererBifurcation *self = CELL_RENDERER_BIFURCATION(object);

    switch (prop_id) {

    case PROP_ROW_ID:
	self->row_id = g_value_get_ulong(value);
	break;

    case PROP_ANIMATION:
	if (self->animation)
	    g_object_unref(self->animation);
	self->animation = ANIMATION(g_object_ref(g_value_get_object(value)));
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static BifurcationDiagram* get_bifurcation_diagram  (CellRendererBifurcation  *self) {
    /* Using the current iterator and animation, find the corresponding
     * bifurcation diagram object, creating and/or updating it if necessary.
     */
    GObject *obj;
    BifurcationDiagram *bd;
    GtkTreeIter keyframe, next_keyframe;
    DeJong *a, *b;

    /* Look up the first keyframe from a row ID */
    if (!animation_keyframe_find_by_id(self->animation, self->row_id, &keyframe))
	return NULL;

    /* Iterate to the next keyframe */
    next_keyframe = keyframe;
    if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(self->animation->model), &next_keyframe)) {
	/* We're at the last keyframe, no bifurcation diagram for us */
	return NULL;
    }

    /* Try to extract an existing bifurcation diagram */
    gtk_tree_model_get(GTK_TREE_MODEL(self->animation->model), &keyframe,
		       ANIMATION_MODEL_BIFURCATION, &obj,
		       -1);
    if (obj) {
	/* We have an existing object, yay */
	bd = BIFURCATION_DIAGRAM(obj);
    }
    else {
	/* Nope, create a new object and store it in the model */
	bd = bifurcation_diagram_new();
	gtk_list_store_set(self->animation->model, &keyframe,
			   ANIMATION_MODEL_BIFURCATION, bd,
			   -1);
    }

    /* Load parameters from both keyframes */
    a = de_jong_new();
    b = de_jong_new();
    animation_keyframe_load(self->animation, &keyframe, PARAMETER_HOLDER(a));
    animation_keyframe_load(self->animation, &next_keyframe,  PARAMETER_HOLDER(b));

    /* Set up this bifurcation diagram for linear interpolation between the two */
    bifurcation_diagram_set_linear_endpoints(bd, a, b);

    g_object_unref(a);
    g_object_unref(b);
    return bd;
}


/************************************************************************************/
/********************************************************** GtkCellRenderer Methods */
/************************************************************************************/

static void cell_renderer_bifurcation_get_size(GtkCellRenderer  *cell,
					       GtkWidget        *widget,
					       GdkRectangle     *cell_area,
					       gint             *x_offset,
					       gint             *y_offset,
					       gint             *width,
					       gint             *height) {
    /* We don't bother suggesting a size yet, just use whatever's available */
}

static gboolean cell_queue_redraw (RedrawInfo *info) {
    gtk_widget_queue_draw_area (GTK_WIDGET (info->view),
		                info->rect.x, info->rect.y,
				info->rect.width, info->rect.height);
    return FALSE;
}

static void cell_renderer_bifurcation_render(GtkCellRenderer      *cell,
					     GdkWindow            *window,
					     GtkWidget            *widget,
					     GdkRectangle         *background_area,
					     GdkRectangle         *cell_area,
					     GdkRectangle         *expose_area,
					     GtkCellRendererState  flags) {
    CellRendererBifurcation *self = CELL_RENDERER_BIFURCATION(cell);
    BifurcationDiagram *bd = get_bifurcation_diagram(self);
    GtkStateType state;
    RedrawInfo *nr;

    if (!bd)
	return;

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

    /* Set the bifurcation diagram renderer's parameters appropriately for this
     * cell renderer. It will automatically figure out what if anything it has
     * to change internally. We size it to fit this cell exactly, and set
     * the colors appropriately for our state and theme.
     */
    g_object_set(bd,
		 "width",  cell_area->width,
		 "height", cell_area->height,
		 "fgcolor-gdk", &GTK_WIDGET(widget)->style->fg[state],
		 "bgcolor-gdk", &GTK_WIDGET(widget)->style->base[state],
		 NULL);

    /* Assume that 5*(cell area) is good enough) */
    if(HISTOGRAM_IMAGER(bd)->total_points_plotted > 5 * cell_area->width * cell_area->height) {
        gdk_draw_pixbuf(window, GTK_WIDGET(widget)->style->fg_gc[state],
		        HISTOGRAM_IMAGER(bd)->image,
		        0, 0, cell_area->x, cell_area->y, cell_area->width, cell_area->height,
		        GDK_RGB_DITHER_NONE, 0, 0);

        return;
    }

    /* Render it a bit and update the image */
    bifurcation_diagram_calculate(bd, 10000, 100);
    histogram_imager_update_image(HISTOGRAM_IMAGER(bd));

    gdk_draw_pixbuf(window, GTK_WIDGET(widget)->style->fg_gc[state],
		    HISTOGRAM_IMAGER(bd)->image,
		    0, 0, cell_area->x, cell_area->y, cell_area->width, cell_area->height,
		    GDK_RGB_DITHER_NONE, 0, 0);

    nr = g_new (RedrawInfo, 1);
    nr->view = self->tree;
    nr->rect.x = cell_area->x;
    nr->rect.y = cell_area->y;
    nr->rect.width = cell_area->width;
    nr->rect.height = cell_area->height;

    g_timeout_add (20, (GSourceFunc) cell_queue_redraw, nr);
}

/* The End */
