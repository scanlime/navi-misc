/*
 * histogram-view.c - A DrawingArea subclass that displays the results of a HistogramImager
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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
#include <gtk/gtk.h>

static void histogram_view_class_init(HistogramViewClass *klass);
static void histogram_view_init(HistogramView *self);
static void histogram_view_finalize(GObject *object);

static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event);
static void on_resize_notify(HistogramImager *imager, GParamSpec *spec, HistogramView *self);


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
}

static void histogram_view_finalize(GObject *object) {
  HistogramView *self = HISTOGRAM_VIEW(object);

  if (self->imager) {
    g_object_unref(self->imager);
    self->imager = NULL;
  }

  if (self->viewable_image) {
    gdk_pixbuf_unref(self->viewable_image);
    self->viewable_image = NULL;
  }
}

GtkWidget* histogram_view_new(HistogramImager *imager) {
  HistogramView *self = g_object_new(histogram_view_get_type(), NULL);
  self->imager = g_object_ref(imager);

  /* Do the first resize, and connect to notify signals for future resizes */
  gtk_widget_set_size_request(GTK_WIDGET(self), self->imager->width, self->imager->height);
  g_signal_connect(self->imager, "notify::width", G_CALLBACK(on_resize_notify), self);
  g_signal_connect(self->imager, "notify::height", G_CALLBACK(on_resize_notify), self);

  return GTK_WIDGET(self);
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

static void on_resize_notify(HistogramImager *imager, GParamSpec *spec, HistogramView *self) {
  gtk_widget_set_size_request(GTK_WIDGET(self), imager->width, imager->height);
}

void histogram_view_update(HistogramView *self) {
  histogram_imager_update_image(self->imager);

  /* We're about to get a new viewable_image, free the old one */
  if (self->viewable_image)
    gdk_pixbuf_unref(self->viewable_image);

  if (self->imager->fgalpha < 0xFFFF ||
      self->imager->bgalpha < 0xFFFF) {
    /* If we need to draw with alpha, composite our histogram imager's output on top of a checkerboard */
    self->viewable_image = gdk_pixbuf_composite_color_simple(self->imager->image,
							     self->imager->width, self->imager->height,
							     GDK_INTERP_TILES, 255,
							     16, 0xaaaaaa, 0x555555);
  }
  else {
    /* If we don't need alpha, just render the histogram imager's pixbuf directly */
    self->viewable_image = gdk_pixbuf_ref(self->imager->image);
  }

  /* Update our entire drawing area.
   * We use GdkRGB directly here to force ignoring the alpha channel.
   */
  gdk_draw_rgb_32_image(GTK_WIDGET(self)->window, GTK_WIDGET(self)->style->fg_gc[GTK_STATE_NORMAL],
			0, 0, self->imager->width, self->imager->height,
			GDK_RGB_DITHER_NORMAL, gdk_pixbuf_get_pixels(self->viewable_image),
			self->imager->width * 4);
}


static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event) {
  HistogramView *self = HISTOGRAM_VIEW(widget);
  GdkRectangle *rects;
  int n_rects, i;

  if (self->viewable_image && !self->imager->size_dirty_flag) {

    gdk_region_get_rectangles(event->region, &rects, &n_rects);

    for (i=0; i<n_rects; i++) {
      /* Clip this rectangle to the image size, since reading outside our buffer is a bad thing */
      if (rects[i].x + rects[i].width > self->imager->width)
	rects[i].width = self->imager->width - rects[i].x;
      if (rects[i].y + rects[i].height > self->imager->height)
	rects[i].height = self->imager->height - rects[i].y;
      if (rects[i].width <= 0 || rects[i].height <= 0)
	continue;

      /* Render a rectangle taken from our pixbuf.
       * We use GdkRGB directly here to force ignoring the alpha channel.
       */
      gdk_draw_rgb_32_image(GTK_WIDGET(self)->window, GTK_WIDGET(self)->style->fg_gc[GTK_STATE_NORMAL],
			    rects[i].x, rects[i].y,
			    rects[i].width, rects[i].height,
			    GDK_RGB_DITHER_NORMAL,
			    gdk_pixbuf_get_pixels(self->viewable_image) +
			    rects[i].x * 4 +
			    rects[i].y * self->imager->width * 4,
			    self->imager->width * 4);
    }

    g_free(rects);
  }
  return FALSE;
}

/* The End */
