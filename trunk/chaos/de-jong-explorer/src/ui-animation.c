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

static void store_keyframe(GtkTreeIter *iter);
static void get_current_keyframe(GtkTreeIter *iter);

void on_anim_play_toggled(GtkWidget *widget, gpointer user_data);
void on_keyframe_add(GtkWidget *widget, gpointer user_data);
void on_keyframe_delete(GtkWidget *widget, gpointer user_data);
void on_keyframe_view_cursor_changed(GtkWidget *widget, gpointer user_data);

enum {
  MODEL_THUMBNAIL,
  MODEL_PARAMS,
  MODEL_DURATION,
};


void animation_ui_init() {
  /* Create the GtkListStore holding our list of keyframe-transition pairs */
  gui.anim.keyframe_view = GTK_TREE_VIEW(glade_xml_get_widget(gui.xml, "keyframe_view"));
  gui.anim.keyframe_list = gtk_list_store_new(3,
					      GDK_TYPE_PIXBUF,
					      G_TYPE_STRING,
					      G_TYPE_FLOAT);
  gtk_tree_view_set_model(gui.anim.keyframe_view, GTK_TREE_MODEL(gui.anim.keyframe_list));

  /* Create columns in the keyframe view */
  gtk_tree_view_insert_column_with_attributes(gui.anim.keyframe_view, -1,
					      "Keyframe", gtk_cell_renderer_pixbuf_new(),
					      "pixbuf", MODEL_THUMBNAIL, NULL);
  gtk_tree_view_insert_column_with_attributes(gui.anim.keyframe_view, -1,
					      "Transition", gtk_cell_renderer_text_new(),
					      "text", MODEL_DURATION, NULL);
}

static void store_keyframe(GtkTreeIter *iter) {
  /* Store the current parameters into a keyframe at the given iterator */
  GdkPixbuf *thumbnail = gdk_pixbuf_scale_simple(render.pixbuf, 128, 128, GDK_INTERP_BILINEAR);
  gchar *params = save_parameters();

  gtk_list_store_set(gui.anim.keyframe_list, iter,
		     MODEL_THUMBNAIL, thumbnail,
		     MODEL_PARAMS,    params,
		     MODEL_DURATION,  1.0,
		     -1);

  g_free(params);
  gdk_pixbuf_unref(thumbnail);
}

static void get_current_keyframe(GtkTreeIter *iter) {
  /* Fill an iterator with one pointing to the currently selected keyframe */
  GtkTreePath *path;
  gtk_tree_view_get_cursor(gui.anim.keyframe_view, &path, NULL);
  gtk_tree_model_get_iter(GTK_TREE_MODEL(gui.anim.keyframe_list), iter, path);
  gtk_tree_path_free(path);
}

void on_anim_play_toggled(GtkWidget *widget, gpointer user_data) {
}

void on_keyframe_add(GtkWidget *widget, gpointer user_data) {
  GtkTreeIter iter;
  gtk_list_store_append(gui.anim.keyframe_list, &iter);
  store_keyframe(&iter);
}

void on_keyframe_replace(GtkWidget *widget, gpointer user_data) {
  GtkTreeIter iter;
  get_current_keyframe(&iter);
  store_keyframe(&iter);
}

void on_keyframe_delete(GtkWidget *widget, gpointer user_data) {
  /* Determine which row the cursor is on, delete it, and make the delete
   * button insensitive again until another row is selected.
   */
  GtkTreeIter iter;
  get_current_keyframe(&iter);

  gtk_widget_set_sensitive(glade_xml_get_widget(gui.xml, "keyframe_delete_button"), FALSE);
  gtk_widget_set_sensitive(glade_xml_get_widget(gui.xml, "keyframe_replace_button"), FALSE);

  gtk_list_store_remove(gui.anim.keyframe_list, &iter);
}

void on_keyframe_view_cursor_changed(GtkWidget *widget, gpointer user_data) {
  /* This is called when a new row in the keyframe view is selected.
   * enable the delete button, and load this row's parameters.
   */
  GtkTreePath *path;
  GtkTreeIter iter;
  GValue value;
  const char *params;

  get_current_keyframe(&iter);

  gtk_widget_set_sensitive(glade_xml_get_widget(gui.xml, "keyframe_delete_button"), TRUE);
  gtk_widget_set_sensitive(glade_xml_get_widget(gui.xml, "keyframe_replace_button"), TRUE);

  memset(&value, 0, sizeof(value));
  gtk_tree_model_get_value(GTK_TREE_MODEL(gui.anim.keyframe_list), &iter, MODEL_PARAMS, &value);
  params = g_value_get_string(&value);

  load_parameters(params);
  write_gui_params();
  restart_rendering();

  g_value_unset(&value);
}

/* The End */
