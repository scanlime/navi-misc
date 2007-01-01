/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * histogram-view.h - A DrawingArea subclass that displays the results of a HistogramImager
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

#ifndef __HISTOGRAM_VIEW_H__
#define __HISTOGRAM_VIEW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkdrawingarea.h>
#include "histogram-imager.h"

G_BEGIN_DECLS

#define HISTOGRAM_VIEW_TYPE            (histogram_view_get_type ())
#define HISTOGRAM_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HISTOGRAM_VIEW_TYPE, HistogramView))
#define HISTOGRAM_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), HISTOGRAM_VIEW_TYPE, HistogramViewClass))
#define IS_HISTOGRAM_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HISTOGRAM_VIEW_TYPE))
#define IS_HISTOGRAM_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), HISTOGRAM_VIEW_TYPE))

typedef struct _HistogramView      HistogramView;
typedef struct _HistogramViewClass HistogramViewClass;

struct _HistogramView {
    GtkDrawingArea parent;

    HistogramImager *imager;
    int old_width, old_height;
};

struct _HistogramViewClass {
    GtkDrawingAreaClass parent_class;

    void (* histogram_view) (HistogramView *cb);
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType      histogram_view_get_type   (void);
GtkWidget* histogram_view_new        (HistogramImager *imager);
void       histogram_view_update     (HistogramView   *self);
void       histogram_view_set_imager (HistogramView   *self,
				      HistogramImager *imager);

G_END_DECLS

#endif /* __HISTOGRAM_VIEW_H__ */

/* The End */
