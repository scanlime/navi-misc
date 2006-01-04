/*
 * dcc-window.h - GtkWindow subclass for managing file transfers
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

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "../common/xchat.h"
#include "../common/dcc.h"
#include "../common/userlist.h"

#ifndef __XCHAT_GNOME_DCC_WINDOW_H__
#define __XCHAT_GNOME_DCC_WINDOW_H__

G_BEGIN_DECLS

typedef struct _DccWindow      DccWindow;
typedef struct _DccWindowClass DccWindowClass;
#define DCC_WINDOW_TYPE            (dcc_window_get_type ())
#define DCC_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DCC_WINDOW_TYPE, DccWindow))
#define DCC_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DCC_WINDOW_TYPE, DccWindowClass))
#define IS_DCC_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DCC_WINDOW_TYPE))
#define IS_DCC_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DCC_WINDOW_TYPE))

struct _DccWindow
{
	GtkWindow parent;

	GtkWidget *transfer_list;
	GtkWidget *stop_button;
	GtkWidget *toplevel;

	GtkListStore *transfer_store;

	GtkCellRenderer *progress_cell;
	GtkCellRenderer *icon_cell;
	GtkCellRenderer *info_cell;
	GtkCellRenderer *remaining_cell;

	GtkTreeViewColumn *progress_column;
	GtkTreeViewColumn *info_column;
	GtkTreeViewColumn *remaining_column;

	GdkPixbuf *up_icon;
	GdkPixbuf *down_icon;
};

struct _DccWindowClass
{
	GtkWindowClass parent_class;
};

GType      dcc_window_get_type (void) G_GNUC_CONST;
DccWindow *dcc_window_new (void);
void       dcc_window_add (DccWindow *window, struct DCC *dcc);
void       dcc_window_update (DccWindow *window, struct DCC *dcc);
void       dcc_window_remove (DccWindow *window, struct DCC *dcc);
void       dcc_send_file (struct User *user);

G_END_DECLS

#endif
