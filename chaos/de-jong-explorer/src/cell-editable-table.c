/*
 * cell-editable-table.c - A GtkTable subclass that implements GtkCellEditable
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

#include "cell-editable-table.h"
#include <gtk/gtk.h>

static void cell_editable_table_class_init(CellEditableTableClass *klass);
static void cell_editable_table_init(CellEditableTable *self);
static void cell_editable_table_cell_editable_init(GtkCellEditableIface *iface);

static void cell_editable_table_start_editing(GtkCellEditable *cell_editable,
					      GdkEvent        *event);


GType cell_editable_table_get_type(void) {
  static GType ce_type = 0;

  if (!ce_type) {
    static const GTypeInfo cb_info = {
      sizeof(CellEditableTableClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) cell_editable_table_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(CellEditableTable),
      0,
      (GInstanceInitFunc) cell_editable_table_init,
    };

    static const GInterfaceInfo cell_editable_info = {
      (GInterfaceInitFunc) cell_editable_table_cell_editable_init,    /* interface_init */
      NULL,                                                           /* interface_finalize */
      NULL                                                            /* interface_data */
    };

    ce_type = g_type_register_static(GTK_TYPE_TABLE, "CellEditableTable", &cb_info, 0);

    g_type_add_interface_static (ce_type,
				 GTK_TYPE_CELL_EDITABLE,
				 &cell_editable_info);
  }

  return ce_type;
}


static void cell_editable_table_class_init(CellEditableTableClass *klass) {
}

static void cell_editable_table_init(CellEditableTable *self) {
}

GtkWidget* cell_editable_table_new(guint rows, guint columns, gboolean homogeneous) {
  CellEditableTable *self = CELL_EDITABLE_TABLE(g_object_new(cell_editable_table_get_type(), NULL));
  GtkTable *table = GTK_TABLE(self);

  if (rows == 0)
    rows = 1;
  if (columns == 0)
    columns = 1;

  table->homogeneous = (homogeneous ? TRUE : FALSE);
  gtk_table_resize (table, rows, columns);

  return GTK_WIDGET(self);
}

static void cell_editable_table_cell_editable_init(GtkCellEditableIface *iface) {
  iface->start_editing = cell_editable_table_start_editing;
}

static void cell_editable_table_start_editing(GtkCellEditable *cell_editable,
					      GdkEvent        *event) {
}

/* The End */
