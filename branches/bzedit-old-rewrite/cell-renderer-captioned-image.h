/* prefs-cell-renderer.h
 *
 * Copyright (C) 2002 Dave Camp
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

#ifndef PREFS_CELL_RENDERER_H
#define PREFS_CELL_RENDERER_H

#include <gtk/gtkcellrenderer.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkcellrendererpixbuf.h>

G_BEGIN_DECLS

#define ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE		(anjuta_cell_renderer_captioned_image_get_type ())
#define ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_CAST ((obj), ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, AnjutaCellRendererCaptionedImage))
#define ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_CAST ((klass), ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, AnjutaCellRendererCaptionedImageClass))
#define ANJUTA_IS_CELL_RENDERER_CAPTIONED_IMAGE(obj)		(GTK_CHECK_TYPE ((obj), ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define ANJUTA_IS_CELL_RENDERER_CAPTIONED_IMAGE_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((klass), ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE))
#define ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE_GET_CLASS(obj)   (GTK_CHECK_GET_CLASS ((obj), ANJUTA_TYPE_CELL_RENDERER_CAPTIONED_IMAGE, AnjutaCellRendererCaptionedImageClass))

typedef struct _AnjutaCellRendererCaptionedImage        AnjutaCellRendererCaptionedImage;
typedef struct _AnjutaCellRendererCaptionedImageClass   AnjutaCellRendererCaptionedImageClass;

struct _AnjutaCellRendererCaptionedImage {
	GtkCellRenderer parent;

	GtkCellRenderer *image;
	GtkCellRenderer *caption;
};

struct _AnjutaCellRendererCaptionedImageClass {
	GtkCellRendererClass parent_class;
};

GType            anjuta_cell_renderer_captioned_image_get_type (void);
GtkCellRenderer *anjuta_cell_renderer_captioned_image_new      (void);

G_END_DECLS

#endif
