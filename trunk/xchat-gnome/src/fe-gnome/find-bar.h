/*
 * find-bar.h - Widget encapsulating the find bar
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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
#ifndef XCHAT_GNOME_FIND_BAR_H
#define XCHAT_GNOME_FIND_BAR_H

#include <gtk/gtkhbox.h>

G_BEGIN_DECLS

typedef struct _FindBar      FindBar;
typedef struct _FindBarClass FindBarClass;
typedef struct _FindBarPriv  FindBarPriv;

#define FIND_BAR_TYPE            (find_bar_get_type ())
#define FIND_BAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FIND_BAR_TYPE, FindBar))
#define FIND_BAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FIND_BAR_TYPE, FindBarClass))
#define IS_FIND_BAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FIND_BAR_TYPE))
#define IS_FIND_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FIND_BAR_TYPE))

struct _FindBar
{
	GtkHBox      parent;
	FindBarPriv *priv;
};

struct _FindBarClass
{
	GtkHBoxClass parent_class;
};

GType      find_bar_get_type (void) G_GNUC_CONST;
GtkWidget *find_bar_new      (void);
void       find_bar_close    (FindBar *bar);
void       find_bar_open     (FindBar *bar);

G_END_DECLS

#endif
