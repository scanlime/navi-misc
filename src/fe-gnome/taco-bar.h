/* side-bar.h
 *  this file is part of evince, a gnome
 * 
 * Copyright (C) 2004-2007 xchat-gnome team
 *
 * Evince is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evince is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef XCHAT_GNOME_SIDE_BAR_H
#define XCHAT_GNOME_SIDE_BAR_H

#include <gtk/gtkvbox.h>
#include <gtk/gtkpixmap.h>

G_BEGIN_DECLS

typedef struct _TacoBar TacoBar;
typedef struct _TacoBarClass TacoBarClass;
typedef struct _TacoBarPrivate TacoBarPrivate;

#define TYPE_TACO_BAR		     (taco_bar_get_type())
#define TACO_BAR(object)	     (G_TYPE_CHECK_INSTANCE_CAST((object), TYPE_TACO_BAR, TacoBar))
#define TACO_BAR_CLASS(klass)	     (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_TACO_BAR, TacoBarClass))
#define IS_TACO_BAR(object)	     (G_TYPE_CHECK_INSTANCE_TYPE((object), TYPE_TACO_BAR))
#define IS_TACO_BAR_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_TACO_BAR))
#define TACO_BAR_GET_CLASS(object)   (G_TYPE_INSTANCE_GET_CLASS((object), TYPE_TACO_BAR, TacoBarClass))

struct _TacoBar {
	GtkVBox base_instance;

	TacoBarPrivate *priv;
};

struct _TacoBarClass {
	GtkVBoxClass base_class;

	void (*closed) (TacoBar *sidebar);
};

GType      taco_bar_get_type     (void);
GtkWidget *taco_bar_new          (void);
void       taco_bar_add_entry	 (TacoBar *,
				  const gchar *page_id,
				  const gchar *title,
				  GtkWidget *icon,
				  GtkWidget *main_widget);
void       taco_bar_set_active   (TacoBar *,
				  const char *page_id);
const char *taco_bar_get_active  (TacoBar *);


G_END_DECLS

#endif
