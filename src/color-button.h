/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * color-button.h - Interactive color selection button. This is a composite
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

#ifndef __COLOR_BUTTON_H__
#define __COLOR_BUTTON_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkbutton.h>

G_BEGIN_DECLS

#define COLOR_BUTTON_TYPE            (color_button_get_type ())
#define COLOR_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), COLOR_BUTTON_TYPE, ColorButton))
#define COLOR_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), COLOR_BUTTON_TYPE, ColorButtonClass))
#define IS_COLOR_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), COLOR_BUTTON_TYPE))
#define IS_COLOR_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), COLOR_BUTTON_TYPE))

typedef struct _ColorButton      ColorButton;
typedef struct _ColorButtonClass ColorButtonClass;

struct _ColorButton {
    GtkButton button;

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

struct _ColorButtonClass {
    GtkButtonClass parent_class;

    void (* color_button) (ColorButton *cb);
};

GType      color_button_get_type(void);
GtkWidget* color_button_new_with_defaults(const char *title, GdkColor *default_color, guint16 default_alpha);
GtkWidget* color_button_new(const char *title);

void       color_button_set_color(ColorButton *cb, GdkColor *c);
void       color_button_get_color(ColorButton *cb, GdkColor *c);
void       color_button_set_alpha(ColorButton *cb, guint16 alpha);
guint16    color_button_get_alpha(ColorButton *cb);
void       color_button_set_color_and_alpha(ColorButton *cb, GdkColor *c, guint16 alpha);

G_END_DECLS

#endif /* __COLOR_BUTTON_H__ */

/* The End */
