/*
 * cell-editable-table.h - A GtkTable subclass that implements GtkCellEditable
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

#ifndef __CELL_EDITABLE_TABLE_H__
#define __CELL_EDITABLE_TABLE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtktable.h>

G_BEGIN_DECLS

#define CELL_EDITABLE_TABLE_TYPE            (cell_editable_table_get_type ())
#define CELL_EDITABLE_TABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CELL_EDITABLE_TABLE_TYPE, CellEditableTable))
#define CELL_EDITABLE_TABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CELL_EDITABLE_TABLE_TYPE, CellEditableTableClass))
#define IS_CELL_EDITABLE_TABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CELL_EDITABLE_TABLE_TYPE))
#define IS_CELL_EDITABLE_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CELL_EDITABLE_TABLE_TYPE))

typedef struct _CellEditableTable      CellEditableTable;
typedef struct _CellEditableTableClass CellEditableTableClass;

struct _CellEditableTable {
  GtkTable button;

  GtkWidget *frame;
  GtkWidget *drawing_area;
  GtkWidget *dialog;

  gchar *title;
  GdkColor previous_color;
  GdkColor color;
  guint16 previous_alpha;
  guint16 alpha;

  gboolean ignore_changes;
};

struct _CellEditableTableClass {
  GtkTableClass parent_class;

  void (* cell_editable_table) (CellEditableTable *cb);
};

GType      cell_editable_table_get_type();
GtkWidget* cell_editable_table_new();

G_END_DECLS

#endif /* __CELL_EDITABLE_TABLE_H__ */

/* The End */
