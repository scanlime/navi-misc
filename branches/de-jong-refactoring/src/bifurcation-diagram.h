/*
 * bifurcation-diagram.h - A HistogramImager that renders a bifurcation diagram
 *                         in which one axis interpolates across de Jong parameters
 *                         and the other axis shows a 1 dimensional projection of the
 *                         image at those parameters.
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

#ifndef __BIFURCATION_DIAGRAM_H__
#define __BIFURCATION_DIAGRAM_H__

#include <gtk/gtk.h>
#include "histogram-imager.h"
#include "de-jong.h"

G_BEGIN_DECLS

#define BIFURCATION_DIAGRAM_TYPE            (bifurcation_diagram_get_type ())
#define BIFURCATION_DIAGRAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIFURCATION_DIAGRAM_TYPE, BifurcationDiagram))
#define BIFURCATION_DIAGRAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BIFURCATION_DIAGRAM_TYPE, BifurcationDiagramClass))
#define IS_BIFURCATION_DIAGRAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIFURCATION_DIAGRAM_TYPE))
#define IS_BIFURCATION_DIAGRAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BIFURCATION_DIAGRAM_TYPE))

typedef struct _BifurcationDiagram          BifurcationDiagram;
typedef struct _BifurcationDiagramClass     BifurcationDiagramClass;


typedef struct {
  guint ix;
  gboolean initialized;
  gdouble point_x, point_y;
  struct {
    gboolean initialized;
    double a,b,c,d;
  } interpolated[1];
} BifurcationColumn;


struct _BifurcationDiagram {
  HistogramImager parent;

  DeJong *a, *b;
  gboolean calc_dirty_flag;
  BifurcationColumn *columns;
  int current_column;
};

struct _BifurcationDiagramClass {
  HistogramImagerClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType                bifurcation_diagram_get_type();
BifurcationDiagram*  bifurcation_diagram_new();

void                 bifurcation_diagram_calculate(BifurcationDiagram     *self,
						   ParameterInterpolator  *interp,
						   gpointer                interp_data,
						   guint                   iterations);

G_END_DECLS

#endif /* __BIFURCATION_DIAGRAM_H__ */

/* The End */
