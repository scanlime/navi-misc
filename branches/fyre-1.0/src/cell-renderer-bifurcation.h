/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * cell-renderer-bifurcation.h - A GtkCellRenderer for viewing a bifurcation
 *                               diagram over the range of a keyframe's transition.
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

#ifndef __CELL_RENDERER_BIFURCATION_H__
#define __CELL_RENDERER_BIFURCATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "de-jong.h"
#include "animation.h"

G_BEGIN_DECLS

#define CELL_RENDERER_BIFURCATION_TYPE            (cell_renderer_bifurcation_get_type ())
#define CELL_RENDERER_BIFURCATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CELL_RENDERER_BIFURCATION_TYPE, CellRendererBifurcation))
#define CELL_RENDERER_BIFURCATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CELL_RENDERER_BIFURCATION_TYPE, CellRendererBifurcationClass))
#define IS_CELL_RENDERER_BIFURCATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CELL_RENDERER_BIFURCATION_TYPE))
#define IS_CELL_RENDERER_BIFURCATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CELL_RENDERER_BIFURCATION_TYPE))

typedef struct _CellRendererBifurcation      CellRendererBifurcation;
typedef struct _CellRendererBifurcationClass CellRendererBifurcationClass;

struct _CellRendererBifurcation {
    GtkCellRenderer parent;

    Animation *animation;
    gulong row_id;

    /* Our parent - yes, I know this makes baby jesus cry. */
    GtkTreeView *tree;
};

struct _CellRendererBifurcationClass {
    GtkCellRendererClass parent_class;

    void (* cell_renderer_bifurcation) (CellRendererBifurcation *cb);
};

GType            cell_renderer_bifurcation_get_type();
GtkCellRenderer* cell_renderer_bifurcation_new();

G_END_DECLS

#endif /* __CELL_RENDERER_BIFURCATION_H__ */

/* The End */
