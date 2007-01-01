/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * color-button.c - Interactive color selection button. This is a composite
 *                  widget that shows a color sample inside a button. When
 *                  the button is clicked, a color picker with auto-apply
 *                  modifies the color sample and sends the 'changed' signal.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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
#include "image-fu.h"
#include <gtk/gtk.h>


enum {
    CHANGED_SIGNAL,
    LAST_SIGNAL
};

static void color_button_class_init(ColorButtonClass *klass);
static void color_button_init(ColorButton *self);
static void color_button_activate(GtkWidget *widget, ColorButton *self);
static void color_changed(GtkWidget *widget, ColorButton *self);
static void color_response(GtkWidget *widget, gint response, ColorButton *self);

static void on_realize(GtkWidget *widget, ColorButton *self);
static void update_color_sample(ColorButton *self);

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

static void color_button_init(ColorButton *self) {
    self->ignore_changes = FALSE;

    g_signal_connect(G_OBJECT(self), "clicked", G_CALLBACK(color_button_activate), (gpointer) self);

    self->frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(self->frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(self->frame));

    self->drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(self->drawing_area), 16, 16);
    gtk_container_add(GTK_CONTAINER(self->frame), GTK_WIDGET(self->drawing_area));
    g_signal_connect(self->drawing_area, "realize", G_CALLBACK(on_realize), self);
}

static void on_realize(GtkWidget *widget, ColorButton *self) {
    update_color_sample(self);
}

static void update_color_sample(ColorButton *self) {
    /* Composite together a pixbuf showing a sample of this color, with alpha,
     * and set it as the drawing area's background pixmap.
     */
    GdkPixbuf *color;
    GdkPixmap *pixmap;
    const int width = CHECKERBOARD_TILE_SIZE * 2;
    const int height = CHECKERBOARD_TILE_SIZE * 2;

    /* First make a 1x1 pixbuf of our color */
    color = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    gdk_pixbuf_fill(color,
		    ((self->color.red   >> 8) << 24) |
		    ((self->color.green >> 8) << 16) |
		    ((self->color.blue  >> 8) <<  8) |
		    ((self->alpha       >> 8)      ));

    /* Blend this on top of a checkerboard pattern */
    image_add_checkerboard(color);

    /* Make a pixbuf out of it */
    pixmap = gdk_pixmap_new(self->drawing_area->window, width, height, -1);
    gdk_pixbuf_render_to_drawable(color, pixmap, self->drawing_area->style->fg_gc[GTK_STATE_NORMAL],
				  0, 0, 0, 0, width, height, GDK_RGB_DITHER_NORMAL, 0, 0);

    /* Set it as a backing pixmap */
    gdk_window_set_back_pixmap(self->drawing_area->window, pixmap, FALSE);
    gdk_window_clear(self->drawing_area->window);

    gdk_pixbuf_unref(color);
    gdk_pixmap_unref(pixmap);
}

GtkWidget* color_button_new_with_defaults(const char *title, GdkColor *default_color, guint16 default_alpha) {
    ColorButton *self = g_object_new(color_button_get_type(), NULL);
    self->title = g_strdup(title);
    self->color = *default_color;
    self->alpha = default_alpha;
    return GTK_WIDGET(self);
}

GtkWidget* color_button_new(const char *title) {
    GdkColor black = {0,0,0,0};
    return color_button_new_with_defaults(title, &black, 0xFFFF);
}

static void color_button_activate(GtkWidget *widget, ColorButton *self) {
    GtkWidget *colorsel;

    /* Only allow one dialog open at a time */
    if (self->dialog)
	return;

    self->dialog = gtk_color_selection_dialog_new(self->title);
    colorsel = GTK_COLOR_SELECTION_DIALOG(self->dialog)->colorsel;
    gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(colorsel), TRUE);

    g_signal_connect(G_OBJECT(self->dialog), "response", G_CALLBACK(color_response), (gpointer) self);

    gtk_color_selection_set_update_policy(GTK_COLOR_SELECTION(colorsel), GTK_UPDATE_CONTINUOUS);
    g_signal_connect(G_OBJECT(colorsel), "color-changed", G_CALLBACK(color_changed), (gpointer) self);

    /* Set the current and previous colors in the dialog */
    self->previous_color = self->color;
    self->previous_alpha = self->alpha;
    self->ignore_changes = TRUE;
    gtk_color_selection_set_current_alpha(GTK_COLOR_SELECTION(colorsel), self->alpha);
    gtk_color_selection_set_previous_alpha(GTK_COLOR_SELECTION(colorsel), self->previous_alpha);
    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(colorsel), &self->color);
    gtk_color_selection_set_previous_color(GTK_COLOR_SELECTION(colorsel), &self->previous_color);
    self->ignore_changes = FALSE;

    /* Is there a better way to hide the help button? */
    gtk_widget_show_all(self->dialog);
    gtk_widget_hide(GTK_COLOR_SELECTION_DIALOG(self->dialog)->help_button);
}

static void color_response(GtkWidget *widget, gint response, ColorButton *self) {
    if (response == GTK_RESPONSE_CANCEL) {
	color_button_set_color(self, &self->previous_color);
	color_button_set_alpha(self, self->previous_alpha);
    }
    gtk_widget_destroy(self->dialog);
    self->dialog = NULL;
}

static void color_changed(GtkWidget *widget, ColorButton *self) {
    GdkColor gcolor;
    if (!self->ignore_changes) {
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(widget), &gcolor);
	self->alpha = gtk_color_selection_get_current_alpha(GTK_COLOR_SELECTION(widget));
	color_button_set_color(self, &gcolor);
    }
}

void color_button_get_color(ColorButton *self, GdkColor *c) {
    *c = self->color;
}

guint16 color_button_get_alpha(ColorButton *self) {
    return self->alpha;
}

void color_button_set_color(ColorButton *self, GdkColor *c) {
    self->color = *c;
    update_color_sample(self);
    g_signal_emit(G_OBJECT(self), color_button_signals[CHANGED_SIGNAL], 0);
}

void color_button_set_alpha(ColorButton *self, guint16 alpha) {
    self->alpha = alpha;
    update_color_sample(self);
    g_signal_emit(G_OBJECT(self), color_button_signals[CHANGED_SIGNAL], 0);
}

void color_button_set_color_and_alpha(ColorButton *self, GdkColor *c, guint16 alpha) {
    self->alpha = alpha;
    self->color = *c;
    update_color_sample(self);
    g_signal_emit(G_OBJECT(self), color_button_signals[CHANGED_SIGNAL], 0);
}

/* The End */
