/*
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

#include <stdlib.h>
#include "cell-renderer-captioned-image.h"

#include <bonobo/bonobo-i18n.h>
#include <libgnome/gnome-macros.h>

static void anjuta_cell_renderer_captioned_image_instance_init (AnjutaCellRendererCaptionedImage      *cell);
static void anjuta_cell_renderer_captioned_image_class_init    (AnjutaCellRendererCaptionedImageClass *class);

enum {
	PROP_0,
	
	PROP_TEXT,
	PROP_PIXBUF
};

#define PAD 3
#define SPACE 5

GNOME_CLASS_BOILERPLATE (AnjutaCellRendererCaptionedImage,
			 anjuta_cell_renderer_captioned_image,
			 GtkCellRenderer, GTK_TYPE_CELL_RENDERER);

static void
anjuta_cell_renderer_captioned_image_get_property (GObject *object,
						   guint param_id,
						   GValue *value,
						   GParamSpec *pspec)
{
	AnjutaCellRendererCaptionedImage *cell = ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE (object);
	
	switch (param_id) {
	case PROP_TEXT:
		g_object_get_property (G_OBJECT (cell->caption), 
				       "text", value);
		break;
	case PROP_PIXBUF:
		g_object_get_property (G_OBJECT (cell->image), 
				       "pixbuf", value);
		break;
	default :
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

static void
anjuta_cell_renderer_captioned_image_set_property (GObject *object,
						   guint param_id,
						   const GValue *value,
						   GParamSpec *pspec)
{
	AnjutaCellRendererCaptionedImage *cell = ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE (object);
	
	switch (param_id) {
	case PROP_TEXT:
		g_object_set_property (G_OBJECT (cell->caption), "text", value);
		break;
		
	case PROP_PIXBUF:
		g_object_set_property (G_OBJECT (cell->image), "pixbuf", 
				       value);
		break;
		
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

GtkCellRenderer *
anjuta_cell_renderer_captioned_image_new (void)
{
	return GTK_CELL_RENDERER (g_object_new (anjuta_cell_renderer_captioned_image_get_type (), NULL));
}

static void
anjuta_cell_renderer_captioned_image_get_size (GtkCellRenderer *gtk_cell,
					       GtkWidget *widget,
					       GdkRectangle *cell_area,
					       int *x_offset,
					       int *y_offset,
					       int *width,
					       int *height)
{
	int text_x_offset;
	int text_y_offset;
	int text_width;
	int text_height;
	
	AnjutaCellRendererCaptionedImage *cell = ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE (gtk_cell);
	
	gtk_cell_renderer_get_size (cell->image, widget, cell_area, 
				    NULL, NULL, width, height);
	
	gtk_cell_renderer_get_size (cell->caption, widget, cell_area,
				    &text_x_offset, 
				    &text_y_offset,
				    &text_width,
				    &text_height);


	if (height) {
		*height = *height + text_height + PAD;
	}
	
	if (width) {
		*width = MAX (*width, text_width);
		*width += SPACE * 2;
	}
}

static void
anjuta_cell_renderer_captioned_image_render (GtkCellRenderer *gtk_cell,
					     GdkWindow *window,
					     GtkWidget *widget,
					     GdkRectangle *background_area,
					     GdkRectangle *cell_area,
					     GdkRectangle *expose_area,
					     guint flags)

{
	AnjutaCellRendererCaptionedImage *cell = ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE (gtk_cell);
	GdkRectangle text_area;
	GdkRectangle pixbuf_area;
	int width, height;
	
	gtk_cell_renderer_get_size (cell->image, widget, cell_area, 
				    NULL, NULL, &width, &height);
	
	pixbuf_area.y = cell_area->y;
	pixbuf_area.x = cell_area->x;
	pixbuf_area.height = height;
	pixbuf_area.width = cell_area->width;

	gtk_cell_renderer_get_size (cell->caption, widget, cell_area, 
				    NULL, NULL, &width, &height);

	text_area.x = cell_area->x + (cell_area->width - width) / 2;
	text_area.y = cell_area->y + (pixbuf_area.height + PAD);
	text_area.height = height;
	text_area.width = width;

	gtk_cell_renderer_render (cell->image, window, widget, 
				  background_area, &pixbuf_area,
				  expose_area, flags);

	gtk_cell_renderer_render (cell->caption, window, widget,
				  background_area, &text_area,
				  expose_area, flags);
}

static void
anjuta_cell_renderer_captioned_image_dispose (GObject *obj)
{
	AnjutaCellRendererCaptionedImage *cell = ANJUTA_CELL_RENDERER_CAPTIONED_IMAGE (obj);
	
	g_object_unref (cell->image);
	g_object_unref (cell->caption);
}

static void
anjuta_cell_renderer_captioned_image_instance_init (AnjutaCellRendererCaptionedImage *cell)
{
	cell->image = gtk_cell_renderer_pixbuf_new ();
	cell->caption = gtk_cell_renderer_text_new ();
}

static void
anjuta_cell_renderer_captioned_image_class_init (AnjutaCellRendererCaptionedImageClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);
	
	parent_class = g_type_class_peek_parent (class);
	
	object_class->dispose = anjuta_cell_renderer_captioned_image_dispose;
	
	object_class->get_property = anjuta_cell_renderer_captioned_image_get_property;
	object_class->set_property = anjuta_cell_renderer_captioned_image_set_property;
	
	cell_class->get_size = anjuta_cell_renderer_captioned_image_get_size;
	cell_class->render = anjuta_cell_renderer_captioned_image_render;

	g_object_class_install_property (object_class,
					 PROP_TEXT,
					 g_param_spec_string ("text",
							      _("Text"),
							      _("Text to render"),
							      NULL,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PIXBUF,
					 g_param_spec_object ("pixbuf",
							      _("Pixbuf Object"),
							      _("The pixbuf to render."),
							      GDK_TYPE_PIXBUF,
							      G_PARAM_READWRITE));
}
