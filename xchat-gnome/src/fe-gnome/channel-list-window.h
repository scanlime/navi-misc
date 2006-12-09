/*
 * channel-list-window.h - channel list
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

#include <glade/glade.h>
#include "../common/xchat.h"

#ifndef XCHAT_GNOME_CHANNEL_LIST_WINDOW_H
#define XCHAT_GNOME_CHANNEL_LIST_WINDOW_H

G_BEGIN_DECLS

typedef struct _ChannelListWindow      		ChannelListWindow;
typedef struct _ChannelListWindowClass		ChannelListWindowClass;
#define CHANNEL_LIST_WINDOW_TYPE           	(channel_list_window_get_type ())
#define CHANNEL_LIST_WINDOW(obj)            	(G_TYPE_CHECK_INSTANCE_CAST ((obj), CHANNEL_LIST_WINDOW_TYPE, ChannelListWindow))
#define CHANNEL_LIST_WINDOW_CLASS(klass)    	(G_TYPE_CHECK_CLASS_CAST ((klass), CHANNEL_LIST_WINDOW_TYPE, ChannelListWindowClass))
#define IS_CHANNEL_LIST_WINDOW(obj)            	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), CHANNEL_LIST_WINDOW_TYPE))
#define IS_CHANNEL_LIST_WINDOW_CLASS(klass)   	(G_TYPE_CHECK_CLASS_TYPE ((klass), CHANNEL_LIST_WINDOW_TYPE))

struct _ChannelListWindow	
{
	GObject parent;

	GtkListStore *store;
	GtkTreeModel *filter;
	GtkTreeModelSort *sort;
	GladeXML *xml;
	struct server *server;
	GtkWidget *window;
	GtkWidget *refresh_button;

	int minimum, maximum;
	char *text_filter;
	gboolean filter_topic, filter_name;
	guint refresh_timeout;
	guint refresh_calls;
	gboolean empty;
} channel_list_window;

struct _ChannelListWindowClass
{
	GObjectClass parent_class;
};

GType channel_list_window_get_type (void) G_GNUC_CONST;

ChannelListWindow* channel_list_window_new (session *sess, gboolean show_list);

G_END_DECLS

gboolean channel_list_exists (server *serv);
void create_channel_list_window (session *sess, gboolean show_list);
void channel_list_append (server *serv, char *channel, char *users, char *topic);
#endif
