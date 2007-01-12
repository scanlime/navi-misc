/*
 * connect-dialog.h - utilities for displaying the connect dialog
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


#include "gui.h"

#ifndef XCHAT_GNOME_CONNECT_DIALOG_H
#define XCHAT_GNOME_CONNECT_DIALOG_H

G_BEGIN_DECLS

typedef struct _ConnectDialog      ConnectDialog;
typedef struct _ConnectDialogClass ConnectDialogClass;

#define CONNECT_DIALOG_TYPE            (connect_dialog_get_type ());
#define CONNECT_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CONNECT_DIALOG_TYPE, ConnectDialog))
#define CONNECT_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CONNECT_DIALOG_TYPE, ConnectDialogClass))
#define IS_CONNECT_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CONNECT_DIALOG_TYPE))
#define IS_CONNECT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CONNECT_DIALOG_TYPE))

struct _ConnectDialog
{
	GtkDialog parent;

	GtkWidget *toplevel;
	GtkWidget *server_list;
	GtkListStore *server_store;
};

struct _ConnectDialogClass
{
	GtkDialogClass parent_class;
};

GType          connect_dialog_get_type (void) G_GNUC_CONST;
ConnectDialog *connect_dialog_new (void);

G_END_DECLS

#endif
