/*
 * cell-renderer-toggle-image.h - A cell renderer that toggles an image on and
 *                                off as if it were a check box
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
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

#ifndef __CELL_RENDERER_TOGGLE_IMAGE_H__
#define __CELL_RENDERER_TOGGLE_IMAGE_H__

#include <gtk/gtkcellrenderer.h>
#include <gtk/gtkcellrendererpixbuf.h>
#include <gtk/gtkcellrenderertoggle.h>

G_BEGIN_DECLS

#define CELL_RENDERER_TOGGLE_IMAGE_TYPE            (cell_renderer_toggle_image_get_type ())
#define CELL_RENDERER_TOGGLE_IMAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CELL_RENDERER_TOGGLE_IMAGE_TYPE, CellRendererToggleImage))
#define CELL_RENDERER_TOGGLE_IMAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CELL_RENDERER_TOGGLE_IMAGE_TYPE, CellRendererToggleImageClass))
#define IS_CELL_RENDERER_TOGGLE_IMAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CELL_RENDERER_TOGGLE_IMAGE_TYPE))
#define IS_CELL_RENDERER_TOGGLE_IMAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CELL_RENDERER_TOGGLE_IMAGE_TYPE))

typedef struct _CellRendererToggleImage      CellRendererToggleImage;
typedef struct _CellRendererToggleImageClass CellRendererToggleImageClass;

struct _CellRendererToggleImage
{
  GtkCellRendererToggle parent;

  GtkCellRenderer *image;
};

struct _CellRendererToggleImageClass
{
  GtkCellRendererToggleClass parent_class;
};

GType            cell_renderer_toggle_image_get_type (void) G_GNUC_CONST;
GtkCellRenderer* cell_renderer_toggle_image_new      (void);

G_END_DECLS

#endif
