/*
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

#include <stdlib.h>
#include "cell-renderer-toggle-image.h"

#include <bonobo/bonobo-i18n.h>
#include <libgnome/gnome-macros.h>

static void bze_cell_renderer_toggle_image_instance_init	(BzeCellRendererToggleImage		*cell);
static void bze_cell_renderer_toggle_image_class_init		(BzeCellRendererToggleImageClass	*class);

enum {
	PROP_0,
	PROP_TOGGLE,
	PROP_PIXBUF
};

GNOME_CLASS_BOILERPLATE (BzeCellRendererToggleImage,
			 bze_cell_renderer_toggle_image,
			 GtkCellRenderer, GTK_TYPE_CELL_RENDERER);

static void
bze_cell_renderer_toggle_image_set_property (GObject *object,
					     guint param_id,
					     const GValue *value,
					     GParamSpec *pspec)
{
	BzeCellRendererToggleImage *cell = BZE_CELL_RENDERER_TOGGLE_IMAGE (object);
	switch(param_id) {
	case PROP_TOGGLE:
		g_object_set_property (G_OBJECT (cell->toggle), "active", value);
		break;
	case PROP_PIXBUF:
		g_object_set_property (G_OBJECT (cell->image), "pixbuf", value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

static void
bze_cell_renderer_toggle_image_get_property (GObject *object,
					     guint param_id,
					     GValue *value,
					     GParamSpec *pspec)
{
	BzeCellRendererToggleImage *cell = BZE_CELL_RENDERER_TOGGLE_IMAGE (object);
	switch(param_id) {
	case PROP_TOGGLE:
		g_object_get_property (G_OBJECT (cell->toggle), "active", value);
		break;
	case PROP_PIXBUF:
		g_object_get_property (G_OBJECT (cell->image), "pixbuf", value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	}
}

GtkCellRenderer *
bze_cell_renderer_toggle_image_new (void)
{
	return GTK_CELL_RENDERER (g_object_new (bze_cell_renderer_toggle_image_get_type (), NULL));
}

static void
bze_cell_renderer_toggle_image_get_size (GtkCellRenderer *gtk_cell,
					 GtkWidget *widget,
					 GdkRectangle *cell_area,
					 int *x_offset,
					 int *y_offset,
					 int *width,
					 int *height)
{
}

static void
bze_cell_renderer_toggle_image_render (GtkCellRenderer *gtk_cell,
				       GdkWindow *window,
				       GtkWidget *widget,
				       GdkRectangle *background_area,
				       GdkRectangle *cell_area,
				       GdkRectangle *expose_area,
				       guint flags)
{
	BzeCellRendererToggleImage *cell = BZE_CELL_RENDERER_TOGGLE_IMAGE (gtk_cell);
//	GdkRectangle toggle_area;
	GdkRectangle pixbuf_area;
	int width, height;

	gtk_cell_renderer_get_size (cell->image, widget, cell_area, NULL, NULL, &width, &height);

	pixbuf_area.y = cell_area->y;
	pixbuf_area.x = cell_area->x;
	pixbuf_area.height = cell_area->height;
	pixbuf_area.width = cell_area->width;

//	gtk_cell_renderer_get_size (cell->toggle, widget, cell_area, NULL, NULL, &width, &height);

//	toggle_area.y = cell_area->y;
//	toggle_area.x = cell_area->x;
//	toggle_area.height = cell_area->height;
//	toggle_area.width = cell_area->width;

	gtk_cell_renderer_render (cell->image, window, widget,
				  background_area, &pixbuf_area,
				  expose_area, flags);
//	gtk_cell_renderer_render (cell->toggle, window, widget,
//				  background_area, &toggle_area,
//				  expose_area, flags);
}

static void
bze_cell_renderer_toggle_image_dispose (GObject *obj)
{
	BzeCellRendererToggleImage *cell = BZE_CELL_RENDERER_TOGGLE_IMAGE (obj);
	g_object_unref (cell->image);
	g_object_unref (cell->toggle);
}

static void
bze_cell_renderer_toggle_image_instance_init (BzeCellRendererToggleImage *cell)
{
	cell->image = gtk_cell_renderer_pixbuf_new ();
	cell->toggle = gtk_cell_renderer_toggle_new ();
}

static void
bze_cell_renderer_toggle_image_class_init (BzeCellRendererToggleImageClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);

	parent_class = g_type_class_peek_parent (class);

	object_class->dispose = bze_cell_renderer_toggle_image_dispose;
	object_class->get_property = bze_cell_renderer_toggle_image_get_property;
	object_class->set_property = bze_cell_renderer_toggle_image_set_property;

	cell_class->get_size = bze_cell_renderer_toggle_image_get_size;
	cell_class->render = bze_cell_renderer_toggle_image_render;

	g_object_class_install_property (object_class,
					 PROP_TOGGLE,
					 g_param_spec_object ("toggle",
					 		      _("Toggle"),
							      _("Toggle state"),
							      G_TYPE_BOOLEAN,
							      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
					 PROP_PIXBUF,
					 g_param_spec_object ("pixbuf",
					 		      _("Pixbuf Object"),
							      _("The pixbuf to render"),
							      GDK_TYPE_PIXBUF,
							      G_PARAM_READWRITE));
}
