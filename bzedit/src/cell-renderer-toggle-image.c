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

#include "cell-renderer-toggle-image.h"

static void cell_renderer_toggle_image_class_init   (CellRendererToggleImageClass *klass);
static void cell_renderer_toggle_image_init         (CellRendererToggleImage *crte);
static void cell_renderer_toggle_image_set_property (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);
static void cell_renderer_toggle_image_get_property (GObject *object, guint param_id, GValue *value, GParamSpec *pspec);
static void cell_renderer_toggle_image_dispose      (GObject *object);
static void cell_renderer_toggle_image_get_size     (GtkCellRenderer *cell, GtkWidget *widget, GdkRectangle *cell_area,
                                                     int *x_offset, int *y_offset, int *width, int *height);
static void cell_renderer_toggle_image_render       (GtkCellRenderer *cell, GdkWindow *window, GtkWidget *widget,
                                                     GdkRectangle *bg, GdkRectangle *area, GdkRectangle *expose, guint flags);

enum
{
  PROP_0,
  PROP_TOGGLE,
  PROP_PIXBUF,
};

static gpointer parent_class = NULL;

GType
cell_renderer_toggle_image_get_type (void)
{
  static GType crte_type = 0;
  if (!crte_type)
  {
    static GTypeInfo crte_info =
    {
      sizeof (CellRendererToggleImageClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    cell_renderer_toggle_image_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (CellRendererToggleImage),
      0,                  /* n preallocs */
      (GInstanceInitFunc) cell_renderer_toggle_image_init,
    };

    crte_type = g_type_register_static (GTK_TYPE_CELL_RENDERER_TOGGLE, "CellRendererToggleImage", &crte_info, 0);
  }

  return crte_type;
}

static void
cell_renderer_toggle_image_class_init (CellRendererToggleImageClass *klass)
{
  GObjectClass *object_class = (GObjectClass*) klass;
  GtkCellRendererClass *cell_class = (GtkCellRendererClass*) klass;
  GParamSpec *pspec;

  parent_class = g_type_class_peek_parent (klass);

  object_class->get_property = cell_renderer_toggle_image_get_property;
  object_class->set_property = cell_renderer_toggle_image_set_property;
  object_class->dispose = cell_renderer_toggle_image_dispose;

  cell_class->get_size = cell_renderer_toggle_image_get_size;
  cell_class->render = cell_renderer_toggle_image_render;

  pspec = g_param_spec_object ("pixbuf", "Pixbuf Object", "The pixbuf to render", GDK_TYPE_PIXBUF, G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_PIXBUF, pspec);
}

static void
cell_renderer_toggle_image_init (CellRendererToggleImage *crte)
{
  crte->image = gtk_cell_renderer_pixbuf_new ();
}

GtkCellRenderer*
cell_renderer_toggle_image_new (void)
{
  return GTK_CELL_RENDERER (g_object_new (cell_renderer_toggle_image_get_type (), NULL));
}

static void
cell_renderer_toggle_image_set_property (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec)
{
  CellRendererToggleImage *cell = CELL_RENDERER_TOGGLE_IMAGE (object);
  switch (param_id)
  {
    case PROP_PIXBUF:
      g_object_set_property (G_OBJECT (cell->image), "pixbuf", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  };
}

static void
cell_renderer_toggle_image_get_property (GObject *object, guint param_id, GValue *value, GParamSpec *pspec)
{
  CellRendererToggleImage *cell = CELL_RENDERER_TOGGLE_IMAGE (object);
  switch (param_id)
  {
    case PROP_PIXBUF:
      g_object_get_property (G_OBJECT (cell->image), "pixbuf", value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  };
}

static void
cell_renderer_toggle_image_dispose (GObject *object)
{
  CellRendererToggleImage *crte = CELL_RENDERER_TOGGLE_IMAGE (object);
  if (crte->image)
  {
    g_object_unref (crte->image);
    crte->image = NULL;
  }
}

static void
cell_renderer_toggle_image_get_size (GtkCellRenderer *cell, GtkWidget *widget, GdkRectangle *cell_area,
                                     int *x_offset, int *y_offset, int *width, int *height)
{
  CellRendererToggleImage *crte = CELL_RENDERER_TOGGLE_IMAGE (cell);
  gtk_cell_renderer_get_size (crte->image, widget, cell_area, x_offset, y_offset, width, height);
  width  = width  + cell->xpad * 2 + widget->style->xthickness * 2;
  height = height + cell->xpad * 2 + widget->style->ythickness * 2;
}

static void
cell_renderer_toggle_image_render (GtkCellRenderer *cell, GdkWindow *window, GtkWidget *widget,
                                   GdkRectangle *bg, GdkRectangle *area, GdkRectangle *expose, guint flags)
{
  CellRendererToggleImage *crte = CELL_RENDERER_TOGGLE_IMAGE (cell);
  GdkRectangle pixbuf_area;
  GdkRectangle draw_area;
  GtkStateType state;
  gint width, height;
  gboolean active;

  gtk_cell_renderer_get_size (crte->image, widget, area, NULL, NULL, &width, &height);

  pixbuf_area.y = area->y;
  pixbuf_area.x = area->x;
  pixbuf_area.height = area->height;
  pixbuf_area.width = area->width;

  if ((flags & GTK_CELL_RENDERER_SELECTED) == GTK_CELL_RENDERER_SELECTED)
  {
    if (GTK_WIDGET_HAS_FOCUS (widget))
      state = GTK_STATE_SELECTED;
    else
      state = GTK_STATE_ACTIVE;
  }
  else
    state = GTK_STATE_NORMAL;

  active = gtk_cell_renderer_toggle_get_active (GTK_CELL_RENDERER_TOGGLE (cell));

  if (gdk_rectangle_intersect (expose, area, &pixbuf_area) && (flags & GTK_CELL_RENDERER_PRELIT))
    gtk_paint_shadow (widget->style, window, state, active ? GTK_SHADOW_IN : GTK_SHADOW_OUT, &draw_area,
                      widget, NULL, pixbuf_area.x, pixbuf_area.y, pixbuf_area.width, pixbuf_area.height);

  if (!active)
    return;

  pixbuf_area.x      += widget->style->xthickness;
  pixbuf_area.y      += widget->style->ythickness;
  pixbuf_area.width  -= widget->style->xthickness * 2;
  pixbuf_area.height -= widget->style->ythickness * 2;
  gtk_cell_renderer_render (crte->image, window, widget, bg, &pixbuf_area, expose, flags);
}
