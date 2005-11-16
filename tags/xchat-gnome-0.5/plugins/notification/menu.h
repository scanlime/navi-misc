/*
 * menu.h - Context menu for the notification plugin.
 *
 * Copyright (C) 2005 W. Evan Sheehan
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

#include <gnome.h>
#include <glib.h>

#ifndef __XCHAT_GNOME_CHANNEL_MENU_H__
#define __XCHAT_GNOME_CHANNEL_MENU_H__

G_BEGIN_DECLS

typedef struct _ChannelMenu          ChannelMenu;
typedef struct _ChannelMenuClass     ChannelMenuClass;

#define CHANNELMENU_TYPE             (channel_menu_get_type ())
#define CHANNELMENU (obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHANNELMENU_TYPE, ChannelMenu))
#define CHANNELMENU_CLASS (klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MENUCHANNEL_TYPE, ChannelMenuClass))
#define IS_CHANNELMENU (obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHANNELMENU_TYPE))
#define IS_CHANNLEMENU_CLASS (klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHANNLEMENU_TYPE))

struct _ChannelMenu
{
	GtkMenu parent;
	GHashTable* channels;
};

struct _ChannelMenuClass
{
	GtkMenuClass parent;
	GHashTable channels;
};

GType channel_menu_get_type (void) G_GNUC_CONST;
ChannelMenu* channel_menu_new (void);
void channel_menu_add (gchar* name);
void channel_menu_remove (gchar* name);
void channel_menu_update (gchar* name, gint status);


typedef struct _ChannelMenuItem          ChannelMenuItem;
typedef struct _ChannelMenuItemClass     ChannelMenuItemClass;

#define CHANNELMENUITEM_TYPE             (channel_menu_item_get_type ())
#define CHANNELMENUITEM (obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CHANNELMENUITEM_TYPE, ChannelMenuItem))
#define CHANNELMENUITEM_CLASS (klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CHANNELMENUITEM_TYPE, ChannelMenuItemClass))
#define IS_CHANNELMENUITEM (obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHANNELMENUITEM_TYPE))
#define IS_CHANNELMENUITEM_CLASS (klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CHANNELMENUITEM_TYPE))

struct _ChannelMenuItem
{
	GtkImageMenuItem parent;
	gint status;
};

struct _ChannelMenuItemClass
{
	GtkImageMenuItemClass parent;
};

GType channel_menu_get_type (void) G_GNUC_CONST;
ChannelMenuItem* channel_menu_item_new (void);
void channel_menu_item_set_status (gint status);