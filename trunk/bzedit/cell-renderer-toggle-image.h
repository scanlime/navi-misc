/* cell-renderer-toggle-image.h
 *
 * Copyright (C) 2002 David Trowbridge
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CELL_RENDERER_TOGGLE_IMAGE_H
#define CELL_RENDERER_TOGGLE_IMAGE_H

#include <gtk/gtkcellrenderer.h>
#include <gtk/gtkcellrendererpixbuf.h>
#include <gtk/gtkcellrenderertoggle.h>

G_BEGIN_DECLS

#define BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE			(bze_cell_renderer_toggle_image_get_type ())
#define BZE_CELL_RENDERER_TOGGLE_IMAGE(obj)			(GTK_CHECK_CAST ((obj), BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE, BzeCellRendererToggleImage))
#define BZE_CELL_RENDERER_TOGGLE_IMAGE_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE, BzeCellRendererToggleImageClass))
#define BZE_IS_CELL_RENDERER_TOGGLE_IMAGE(obj)			(GTK_CHECK_TYPE ((obj), BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE))
#define BZE_IS_CELL_RENDERER_TOGGLE_IMAGE_CLASS(klass)		(GTK_CHECK_CLASS_TYPE ((klass), BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE))
#define BZE_CELL_RENDERER_TOGGLE_IMAGE_GET_CLASS(obj)		(GTK_CHECK_GET_CLASS ((obj), BZE_TYPE_CELL_RENDERER_TOGGLE_IMAGE, BzeCellRendererToggleImageClass))

typedef struct _BzeCellRendererToggleImage	BzeCellRendererToggleImage;
typedef struct _BzeCellRendererToggleImageClass	BzeCellRendererToggleImageClass;

struct _BzeCellRendererToggleImage {
  GtkCellRenderer parent;

  GtkCellRenderer *image;
  GtkCellRenderer *toggle;
};

struct _BzeCellRendererToggleImageClass {
  GtkCellRendererClass parent_class;
};

GType		 bze_cell_renderer_toggle_image_get_type(void);
GtkCellRenderer	*bze_cell_renderer_toggle_image_new	(void);

G_END_DECLS

#endif
