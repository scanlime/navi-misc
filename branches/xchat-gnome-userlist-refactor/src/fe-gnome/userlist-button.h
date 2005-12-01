/*
 * userlist-button.h - Widget encapsulating the userlist
 *
 * Copyright (C) 2005 David Trowbridge
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
#ifndef XCHAT_GNOME_USERLIST_BUTTON_H
#define XCHAT_GNOME_USERLIST_BUTTON_H

#include <gtk/gtktogglebutton.h>

G_BEGIN_DECLS

typedef struct _UserlistButton      UserlistButton;
typedef struct _UserlistButtonClass UserlistButtonClass;
typedef struct _UserlistButtonPriv  UserlistButtonPriv;

#define USERLIST_BUTTON_TYPE            (userlist_button_get_type ())
#define USERLIST_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), USERLIST_BUTTON_TYPE, UserlistButton))
#define USERLIST_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), USERLIST_BUTTON_TYPE, UserlistButton))
#define IS_USERLIST_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), USERLIST_BUTTON_TYPE))
#define IS_USERLIST_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), USERLIST_BUTTON_TYPE))

struct _UserlistButton
{
	GtkToggleButton     parent;

	UserlistButtonPriv *priv;
};

struct _UserlistButtonClass
{
	GtkToggleButtonClass parent_class;
};

GType      userlist_button_get_type (void) G_GNUC_CONST;
GtkWidget *userlist_button_new      (void);

G_END_DECLS

#endif
