/*
 * userlist-window.h - The userlist pop-up window
 *
 * Copyright (C) 2005 xchat-gnome team
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
#ifndef XCHAT_GNOME_USERLIST_WINDOW_H
#define XCHAT_GNOME_USERLIST_WINDOW_H

#include <gtk/gtkwindow.h>
#include "../common/xchat.h"

G_BEGIN_DECLS

typedef struct _UserlistWindow      UserlistWindow;
typedef struct _UserlistWindowClass UserlistWindowClass;
typedef struct _UserlistWindowPriv  UserlistWindowPriv;

#define USERLIST_WINDOW_TYPE            (userlist_window_get_type ())
#define USERLIST_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), USERLIST_WINDOW_TYPE, UserlistWindow))
#define USERLIST_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), USERLIST_WINDOW_TYPE, UserlistWindowClass))
#define IS_USERLIST_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), USERLIST_WINDOW_TYPE))
#define IS_USERLIST_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), USERLIST_WINDOW_TYPE))

struct _UserlistWindow
{
	GtkWindow parent;

	UserlistWindowPriv *priv;
};

struct _UserlistWindowClass
{
	GtkWindowClass parent_class;
};

GType      userlist_window_get_type       (void) G_GNUC_CONST;
GtkWidget *userlist_window_new            (void);
void       userlist_window_show           (UserlistWindow *window);
void       userlist_window_hide           (UserlistWindow *window);
void       userlist_window_insert_user    (UserlistWindow *window, struct session *sess, struct User *user, int row, gboolean selected);
void       userlist_window_remove_user    (UserlistWindow *window, struct session *sess, struct User *user);
void       userlist_window_set_current    (UserlistWindow *window, struct session *sess);
void       userlist_window_remove_session (UserlistWindow *window, struct session *sess);

G_END_DECLS

#endif
