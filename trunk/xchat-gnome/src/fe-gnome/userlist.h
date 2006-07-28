/*
 * userlist.h - the userlist object
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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

#include <glib-object.h>
#include <gtk/gtkliststore.h>
#include "../common/xchat.h"
#include "../common/userlist.h"

#ifndef __XCHAT_GNOME_USERLIST_H__
#define __XCHAT_GNOME_USERLIST_H__

G_BEGIN_DECLS

#define USERLIST_TYPE            (userlist_get_type ())
#define USERLIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), USERLIST_TYPE, Userlist))
#define USERLIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), USERLIST_TYPE, UserlistClass))
#define IS_USERLIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), USERLIST_TYPE))
#define IS_USERLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), USERLIST_TYPE))

typedef struct _Userlist      Userlist;
typedef struct _UserlistClass UserlistClass;

struct _Userlist
{
	GObject parent;

	GHashTable *stores;
};

struct _UserlistClass
{
	GObjectClass parent_class;
};

GType         userlist_get_type        (void) G_GNUC_CONST;
Userlist*     userlist_new             (void);
void          userlist_insert          (Userlist *userlist, session *sess, struct User *newuser, int row, gboolean selected);
gboolean      userlist_remove_user     (Userlist *userlist, session *sess, struct User *user);
void          userlist_update          (Userlist *userlist, session *sess, struct User *user);
void          userlist_move            (Userlist *userlist, session *sess, struct User *user, int new_row);
void          userlist_clear_all       (Userlist *userlist, session *sess);
void          userlist_erase           (Userlist *userlist, session *sess);
GtkListStore* userlist_get_store       (Userlist *userlist, session *sess);
GCompletion*  userlist_get_completion  (Userlist *userlist, session *sess);
void	      userlist_set_user_button (Userlist *userlist, session *sess);

G_END_DECLS

#endif
