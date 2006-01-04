/*
 * status-bar.h - Widget encapsulating the status bar
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
#ifndef XCHAT_GNOME_STATUS_BAR_H
#define XCHAT_GNOME_STATUS_BAR_H

#include <gtk/gtkstatusbar.h>
#include "../common/xchat.h"

G_BEGIN_DECLS

typedef struct _StatusBar      StatusBar;
typedef struct _StatusBarClass StatusBarClass;
typedef struct _StatusBarPriv  StatusBarPriv;

#define STATUS_BAR_TYPE            (status_bar_get_type ())
#define STATUS_BAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), STATUS_BAR_TYPE, StatusBar))
#define STATUS_BAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), STATUS_BAR_TYPE, StatusBarClass))
#define IS_STATUS_BAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), STATUS_BAR_TYPE))
#define IS_STATUS_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), STATUS_BAR_TYPE))

struct _StatusBar
{
	GtkStatusbar   parent;

	StatusBarPriv *priv;
};

struct _StatusBarClass
{
	GtkStatusbarClass parent_class;
};

GType      status_bar_get_type      (void) G_GNUC_CONST;
GtkWidget *status_bar_new           (void);
void       status_bar_set_lag       (StatusBar *bar, struct server *sess, float seconds, gboolean sent);
void       status_bar_set_queue     (StatusBar *bar, struct server *sess, int bytes);
void       status_bar_set_current   (StatusBar *bar, struct server *sess);
void       status_bar_remove_server (StatusBar *bar, struct server *sess);

G_END_DECLS

#endif
