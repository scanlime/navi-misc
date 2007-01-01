/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * histogram-view.c - A DrawingArea subclass that displays the results of a HistogramImager
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

#include "histogram-view.h"
#include "image-fu.h"
#include <gtk/gtk.h>

static void histogram_view_class_init(HistogramViewClass *klass);
static void histogram_view_init(HistogramView *self);
static void histogram_view_finalize(GObject *object);

static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event);
static void on_resize_notify(HistogramImager *imager, GParamSpec *spec, HistogramView *self);

static void       histogram_view_draw_image_region(HistogramView *self, GdkRegion *region);
static void       histogram_view_draw_background_region(HistogramView *self, GdkRegion *region);
static GdkRegion* histogram_view_get_full_image_region(HistogramView *self);



/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType histogram_view_get_type(void) {
    static GType hv_type = 0;

    if (!hv_type) {
	static const GTypeInfo hv_info = {
	    sizeof(HistogramViewClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) histogram_view_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(HistogramView),
	    0,
	    (GInstanceInitFunc) histogram_view_init,
	};

	hv_type = g_type_register_static(GTK_TYPE_DRAWING_AREA, "HistogramView", &hv_info, 0);
    }

    return hv_type;
}

static void histogram_view_class_init(HistogramViewClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gobject_class->finalize = histogram_view_finalize;
    widget_class->expose_event = on_expose;
}

static void histogram_view_init(HistogramView *self) {
    /* Double buffering will just slow us down.
     * Turing this off also gives us the chance to paint our own background
     * in the expose handler. Painting it only behind the areas we don't have an
     * image in will make resizing much more speedy and usable.
     */
    gtk_widget_set_double_buffered(GTK_WIDGET(self), FALSE);
}

static void histogram_view_finalize(GObject *object) {
    HistogramView *self = HISTOGRAM_VIEW(object);

    if (self->imager) {
	g_object_unref(self->imager);
	self->imager = NULL;
    }
}

GtkWidget* histogram_view_new(HistogramImager *imager) {
    HistogramView *self = g_object_new(histogram_view_get_type(), NULL);
    histogram_view_set_imager(self, imager);
    return GTK_WIDGET(self);
}

void histogram_view_set_imager(HistogramView *self, HistogramImager *imager) {
    if (self->imager) {
	/* Remove the old imager */
	g_signal_handlers_disconnect_by_func(self->imager, G_CALLBACK(on_resize_notify), self);
	g_object_unref(self->imager);
    }

    self->imager = g_object_ref(imager);

    /* Do the first resize, and connect to notify signals for future resizes */
    gtk_widget_set_size_request(GTK_WIDGET(self), self->imager->width, self->imager->height);
    self->old_width = self->imager->width;
    self->old_height = self->imager->height;
    g_signal_connect(self->imager, "notify::width", G_CALLBACK(on_resize_notify), self);
    g_signal_connect(self->imager, "notify::height", G_CALLBACK(on_resize_notify), self);
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

static GdkRegion* histogram_view_get_full_image_region(HistogramView *self)
{
    /* Create a GdkRegion for the entire image */
    GdkRectangle image_rect;

    image_rect.x = 0;
    image_rect.y = 0;
    image_rect.width = self->imager->width;
    image_rect.height = self->imager->height;

    return gdk_region_rectangle(&image_rect);
}

static void on_resize_notify(HistogramImager *imager, GParamSpec *spec, HistogramView *self)
{
    GdkRegion *old, *new;
    GdkRectangle rect;

    gtk_widget_set_size_request(GTK_WIDGET(self), imager->width, imager->height);

    /* Since we're not letting gtk clear our widget for us for speed and unflickeriness,
     * we need to clear any areas that the previous size covered but the new size doesn't.
     */
    rect.x = rect.y = 0;
    rect.width = self->old_width;
    rect.height = self->old_height;
    old = gdk_region_rectangle(&rect);
    rect.width = self->imager->width;
    rect.height = self->imager->height;
    new = gdk_region_rectangle(&rect);

    gdk_region_subtract(old, new);
    if (!gdk_region_empty(old))
	histogram_view_draw_background_region(self, old);

    gdk_region_destroy(old);
    gdk_region_destroy(new);

    self->old_width = self->imager->width;
    self->old_height = self->imager->height;
}

void histogram_view_update(HistogramView *self) {
    GdkRegion *update_region;

    histogram_imager_update_image(self->imager);

    /* Draw the whole thing */
    update_region = histogram_view_get_full_image_region(self);
    histogram_view_draw_image_region(self, update_region);
    gdk_region_destroy(update_region);

    self->imager->render_dirty_flag = FALSE;
}

static void histogram_view_draw_image_region(HistogramView *self, GdkRegion *region) {
    GdkRectangle *rects;
    int n_rects, i;
    gdk_region_get_rectangles(region, &rects, &n_rects);

    if (self->imager->fgalpha < 0xFFFF || self->imager->bgalpha < 0xFFFF) {
	/* If we need to draw with alpha, composite our histogram imager's output
	 * onto a checkerboard. It's a little messy writing directly to the imager's
	 * pixbuf, but since we update the image before saving it anyway this shouldn't
	 * cause any problems. This should give us a speed boost compared to doing
	 * a separate compositing step like we used to.
	 */
	image_add_checkerboard(self->imager->image);
    }

    for (i=0; i<n_rects; i++) {
	/* Render a rectangle taken from our pixbuf.
	 * We use GdkRGB directly here to force ignoring the alpha channel.
	 */
	gdk_draw_rgb_32_image(GTK_WIDGET(self)->window, GTK_WIDGET(self)->style->fg_gc[GTK_STATE_NORMAL],
			      rects[i].x, rects[i].y,
			      rects[i].width, rects[i].height,
			      GDK_RGB_DITHER_NORMAL,
			      gdk_pixbuf_get_pixels(self->imager->image) +
			      rects[i].x * 4 +
			      rects[i].y * self->imager->width * 4,
			      self->imager->width * 4);
    }
    g_free(rects);
}

static void histogram_view_draw_background_region(HistogramView *self, GdkRegion *region) {
    GdkRectangle *rects;
    int n_rects, i;
    gdk_region_get_rectangles(region, &rects, &n_rects);

    for (i=0; i<n_rects; i++) {
	gdk_draw_rectangle(GTK_WIDGET(self)->window, GTK_WIDGET(self)->style->bg_gc[GTK_STATE_NORMAL],
			   TRUE,
			   rects[i].x, rects[i].y,
			   rects[i].width, rects[i].height);
    }
    g_free(rects);
}

static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event) {
    HistogramView *self = HISTOGRAM_VIEW(widget);
    GdkRegion *image_rect_region, *outside_image;

    if (self->imager->image && !self->imager->size_dirty_flag) {
	/* Separate the expose region into a region inside our image and a region outside
	 * our image. Draw the first with histogram_view_draw_image_region and the second
	 * with histogram_view_draw_background_region.
	 */
	image_rect_region = histogram_view_get_full_image_region(self);

	outside_image = gdk_region_copy(event->region);
	gdk_region_subtract(outside_image, image_rect_region);

	gdk_region_intersect(image_rect_region, event->region);

	histogram_view_draw_image_region(self, image_rect_region);
	histogram_view_draw_background_region(self, outside_image);

	gdk_region_destroy(image_rect_region);
	gdk_region_destroy(outside_image);
    }
    return FALSE;
}

/* The End */
