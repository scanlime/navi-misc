/*
 * channel-list.h - channel list
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

#ifndef XCHAT_GNOME_CHANNEL_LIST_H
#define XCHAT_GNOME_CHANNEL_LIST_H

typedef struct
{
	GtkListStore *store;
	GtkTreeModel *filter;
	GtkTreeModelSort *sort;
	GladeXML *xml;
	struct server *server;

	int minimum, maximum;
	char *text_filter;
	gboolean filter_topic, filter_name;
	guint refresh_timeout;
	guint refresh_calls;
	gboolean empty;
} channel_list_window;

gboolean channel_list_exists (server *serv);
void create_channel_list (session *sess);
void channel_list_append (server *serv, char *channel, char *users, char *topic);
void repopulate_channel_list (channel_list_window *win);

#endif
