/*
 * color-button.c - Interactive color selection button. This is a composite
 *                  widget that shows a color sample inside a button. When
 *                  the button is clicked, a color picker with auto-apply
 *                  modifies the color sample and sends the 'changed' signal.
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

#include "color-button.h"
#include <gtk/gtk.h>


enum {
  CHANGED_SIGNAL,
  LAST_SIGNAL
};

static void color_button_class_init(ColorButtonClass *klass);
static void color_button_init(ColorButton *cb);
static void color_button_activate(GtkWidget *widget, ColorButton *cb);
static void set_sample_color(ColorButton *cb, GdkColor *c);
static void color_changed(GtkWidget *widget, ColorButton *cb);
static void color_response(GtkWidget *widget, gint response, ColorButton *cb);

static guint color_button_signals[LAST_SIGNAL] = { 0 };


GType color_button_get_type(void) {
  static GType cb_type = 0;

  if (!cb_type) {
    static const GTypeInfo cb_info = {
      sizeof(ColorButtonClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) color_button_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(ColorButton),
      0,
      (GInstanceInitFunc) color_button_init,
    };

    cb_type = g_type_register_static(GTK_TYPE_BUTTON, "ColorButton", &cb_info, 0);
  }

  return cb_type;
}


static void color_button_class_init(ColorButtonClass *klass) {
  color_button_signals[CHANGED_SIGNAL] = g_signal_new("changed",
						      G_TYPE_FROM_CLASS(klass),
						      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
						      G_STRUCT_OFFSET(ColorButtonClass, color_button),
						      NULL,
						      NULL,
						      g_cclosure_marshal_VOID__VOID,
						      G_TYPE_NONE, 0);
}

static void color_button_init(ColorButton *cb) {
  cb->ignore_changes = FALSE;

  g_signal_connect(G_OBJECT(cb), "clicked", G_CALLBACK(color_button_activate), (gpointer) cb);

  cb->frame = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(cb->frame), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(cb), GTK_WIDGET(cb->frame));

  cb->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA(cb->drawing_area), 16, 16);
  gtk_container_add(GTK_CONTAINER(cb->frame), GTK_WIDGET(cb->drawing_area));
}

static void set_sample_color(ColorButton *cb, GdkColor *c) {
  GtkRcStyle *rc_style;

  rc_style = gtk_rc_style_new();
  rc_style->bg[GTK_STATE_NORMAL] = *c;
  rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BG;
  gtk_widget_modify_style(GTK_WIDGET(cb->drawing_area), rc_style);
  gtk_rc_style_unref(rc_style);
}

GtkWidget* color_button_new(const char *title, GdkColor *default_color, guint16 default_alpha) {
  ColorButton *cb = g_object_new(color_button_get_type(), NULL);
  cb->title = g_strdup(title);
  cb->color = *default_color;
  cb->alpha = default_alpha;
  set_sample_color(cb, default_color);
  return GTK_WIDGET(cb);
}

static void color_button_activate(GtkWidget *widget, ColorButton *cb) {
  GtkWidget *colorsel;

  /* Only allow one dialog open at a time */
  if (cb->dialog)
    return;

  cb->dialog = gtk_color_selection_dialog_new(cb->title);
  colorsel = GTK_COLOR_SELECTION_DIALOG(cb->dialog)->colorsel;
  gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(colorsel), TRUE);

  g_signal_connect(G_OBJECT(cb->dialog), "response", G_CALLBACK(color_response), (gpointer) cb);

  gtk_color_selection_set_update_policy(GTK_COLOR_SELECTION(colorsel), GTK_UPDATE_CONTINUOUS);
  g_signal_connect(G_OBJECT(colorsel), "color-changed", G_CALLBACK(color_changed), (gpointer) cb);

  /* Set the current and previous colors in the dialog */
  cb->previous_color = cb->color;
  cb->previous_alpha = cb->alpha;
  cb->ignore_changes = TRUE;
  gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(colorsel), cb->alpha);
  gtk_color_selection_set_previous_alpha(GTK_COLOR_SELECTION(colorsel), cb->previous_alpha);
  gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(colorsel), &cb->color);
  gtk_color_selection_set_previous_color(GTK_COLOR_SELECTION(colorsel), &cb->previous_color);
  cb->ignore_changes = FALSE;

  /* Is there a better way to hide the help button? */
  gtk_widget_show_all(cb->dialog);
  gtk_widget_hide(GTK_COLOR_SELECTION_DIALOG(cb->dialog)->help_button);
}

static void color_response(GtkWidget *widget, gint response, ColorButton *cb) {
  if (response == GTK_RESPONSE_CANCEL) {
    color_button_set_color(cb, &cb->previous_color);
    color_button_set_alpha(cb, cb->previous_alpha);
  }
  gtk_widget_destroy(cb->dialog);
  cb->dialog = NULL;
}

static void color_changed(GtkWidget *widget, ColorButton *cb) {
  GdkColor gcolor;
  if (!cb->ignore_changes) {
    gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(widget), &gcolor);
    cb->alpha = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(widget));
    color_button_set_color(cb, &gcolor);
  }
}

void color_button_set_color(ColorButton *cb, GdkColor *c) {
  cb->color = *c;
  set_sample_color(cb, &cb->color);
  g_signal_emit(G_OBJECT(cb), color_button_signals[CHANGED_SIGNAL], 0);
}

void color_button_get_color(ColorButton *cb, GdkColor *c) {
  *c = cb->color;
}

void color_button_set_alpha(ColorButton *cb, guint16 alpha) {
  cb->alpha = alpha;
}

guint16 color_button_get_alpha(ColorButton *cb) {
  return cb->alpha;
}

/* The End */
