/*
 * ui-animation.c - Implements the GTK+ user interface components used for animation
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

#include "de-jong.h"

void on_anim_play_toggled(GtkWidget *widget, gpointer user_data);
void on_keyframe_copy(GtkWidget *widget, gpointer user_data);
void on_keyframe_delete(GtkWidget *widget, gpointer user_data);


void animation_ui_init() {
  /* Create the keyframe list */
  gui.anim.keyframe_view = GTK_TREE_VIEW(glade_xml_get_widget(gui.xml, "keyframe_view"));
  gui.anim.keyframe_list = gtk_list_store_new(1, GDK_TYPE_PIXBUF);
  gtk_tree_view_set_model(gui.anim.keyframe_view, GTK_TREE_MODEL(gui.anim.keyframe_list));

  /* Create columns in the keyframe view */
  gtk_tree_view_insert_column_with_attributes(gui.anim.keyframe_view, -1,
					      "Thumbnail", gtk_cell_renderer_pixbuf_new(),
					      "pixbuf", 0, NULL);

  /* Add the initial keyframe */
  gtk_list_store_insert(gui.anim.keyframe_list, &gui.anim.current_keyframe, 0);
  gtk_list_store_set(gui.anim.keyframe_list, &gui.anim.current_keyframe,
		     0, render.pixbuf,
		     -1);
}

void on_anim_play_toggled(GtkWidget *widget, gpointer user_data) {
}

void on_keyframe_copy(GtkWidget *widget, gpointer user_data) {
}

void on_keyframe_delete(GtkWidget *widget, gpointer user_data) {
}

/* The End */
