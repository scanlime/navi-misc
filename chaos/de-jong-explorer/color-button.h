/*
 * Interactive color selection button. This is a composite widget
 * that shows a color sample inside a button. When the button is clicked,
 * a color picker with auto-apply modifies the color sample and sends the
 * 'changed' signal.
 *
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
};

struct _ColorButtonClass {
  GtkButtonClass parent_class;

  void (* color_button) (ColorButton *cb);
};

GType      color_button_get_type(void);
GtkWidget* color_button_new(const char *title, GdkColor *defaultColor);
void       color_button_set_color(ColorButton *cb, GdkColor *c);
void       color_button_get_color(ColorButton *cb, GdkColor *c);

G_END_DECLS

#endif /* __COLOR_BUTTON_H */

/* The End */
