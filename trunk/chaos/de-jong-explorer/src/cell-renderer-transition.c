/*
 * cell-renderer-transition.c - A GtkCellRenderer for viewing and editing animation transitions
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

#include "cell-renderer-transition.h"
#include "cell-editable-table.h"
#include <gtk/gtk.h>

static void cell_renderer_transition_class_init(CellRendererTransitionClass *klass);
static void cell_renderer_transition_init(CellRendererTransition *self);

static void cell_renderer_transition_finalize      (GObject                  *object);

static void cell_renderer_transition_get_property  (GObject                  *object,
						    guint                     param_id,
						    GValue                   *value,
						    GParamSpec               *pspec);
static void cell_renderer_transition_set_property  (GObject                  *object,
						    guint                     param_id,
						    const GValue             *value,
						    GParamSpec               *pspec);
static void cell_renderer_transition_get_size      (GtkCellRenderer          *cell,
						    GtkWidget                *widget,
						    GdkRectangle             *cell_area,
						    gint                     *x_offset,
						    gint                     *y_offset,
						    gint                     *width,
						    gint                     *height);
static void cell_renderer_transition_render        (GtkCellRenderer          *cell,
						    GdkWindow                *window,
						    GtkWidget                *widget,
						    GdkRectangle             *background_area,
						    GdkRectangle             *cell_area,
						    GdkRectangle             *expose_area,
						    GtkCellRendererState      flags);

static GtkCellEditable *cell_renderer_transition_start_editing (GtkCellRenderer      *cell,
								GdkEvent             *event,
								GtkWidget            *widget,
								const gchar          *path,
								GdkRectangle         *background_area,
								GdkRectangle         *cell_area,
								GtkCellRendererState  flags);


GType cell_renderer_transition_get_type(void) {
  static GType cr_type = 0;

  if (!cr_type) {
    static const GTypeInfo cr_info = {
      sizeof(CellRendererTransitionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) cell_renderer_transition_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(CellRendererTransition),
      0,
      (GInstanceInitFunc) cell_renderer_transition_init,
    };

    cr_type = g_type_register_static(GTK_TYPE_CELL_RENDERER, "CellRendererTransition", &cr_info, 0);
  }

  return cr_type;
}


static void cell_renderer_transition_class_init(CellRendererTransitionClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

  object_class->finalize = cell_renderer_transition_finalize;

  object_class->get_property = cell_renderer_transition_get_property;
  object_class->set_property = cell_renderer_transition_set_property;

  cell_class->get_size = cell_renderer_transition_get_size;
  cell_class->render = cell_renderer_transition_render;
  cell_class->start_editing = cell_renderer_transition_start_editing;
}

static void cell_renderer_transition_init(CellRendererTransition *self) {
  GTK_CELL_RENDERER(self)->mode = GTK_CELL_RENDERER_MODE_EDITABLE;
}

GtkCellRenderer* cell_renderer_transition_new() {
  return GTK_CELL_RENDERER(g_object_new(cell_renderer_transition_get_type(), NULL));
}

static void cell_renderer_transition_finalize(GObject *object) {
}

static void cell_renderer_transition_get_property(GObject    *object,
						  guint       param_id,
						  GValue     *value,
						  GParamSpec *pspec) {
}

static void cell_renderer_transition_set_property(GObject       *object,
						  guint          param_id,
						  const GValue  *value,
						  GParamSpec    *pspec) {
}

static void cell_renderer_transition_get_size(GtkCellRenderer  *cell,
					      GtkWidget        *widget,
					      GdkRectangle     *cell_area,
					      gint             *x_offset,
					      gint             *y_offset,
					      gint             *width,
					      gint             *height) {
}

static void cell_renderer_transition_render(GtkCellRenderer      *cell,
					    GdkWindow            *window,
					    GtkWidget            *widget,
					    GdkRectangle         *background_area,
					    GdkRectangle         *cell_area,
					    GdkRectangle         *expose_area,
					    GtkCellRendererState  flags) {
}

static GtkCellEditable *cell_renderer_transition_start_editing(GtkCellRenderer      *cell,
							       GdkEvent             *event,
							       GtkWidget            *widget,
							       const gchar          *path,
							       GdkRectangle         *background_area,
							       GdkRectangle         *cell_area,
							       GtkCellRendererState  flags) {
  GtkWidget *table = cell_editable_table_new(2, 2, FALSE);

  GtkWidget *spin1 = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(1, 0, 10, 0.01, 0.1, 0.1)), 0.01, 2);
  GtkWidget *spin2 = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(1, 0, 10, 0.01, 0.1, 0.1)), 0.01, 2);
  GtkWidget *spin3 = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(1, 0, 10, 0.01, 0.1, 0.1)), 0.01, 2);
  GtkWidget *spin4 = gtk_spin_button_new(GTK_ADJUSTMENT(gtk_adjustment_new(1, 0, 10, 0.01, 0.1, 0.1)), 0.01, 2);

  gtk_table_attach(GTK_TABLE(table), spin1, 0,1, 0,1, 0,0, 4,4);
  gtk_table_attach(GTK_TABLE(table), spin2, 1,2, 0,1, 0,0, 4,4);
  gtk_table_attach(GTK_TABLE(table), spin3, 0,1, 1,2, 0,0, 4,4);
  gtk_table_attach(GTK_TABLE(table), spin4, 1,2, 1,2, 0,0, 4,4);

  gtk_widget_show_all(table);

  return GTK_CELL_EDITABLE(table);
}

/* The End */
