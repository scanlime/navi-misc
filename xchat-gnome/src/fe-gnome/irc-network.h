/*
 * irc-network.h - an object which interfaces around the core 'ircnet'
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

#include <glib.h>
#include <glib-object.h>
#include "../common/xchat.h"
#include "../common/servlist.h"

#ifndef __XCHAT_GNOME_IRC_NETWORK_H__
#define __XCHAT_GNOME_IRC_NETWORK_H__

G_BEGIN_DECLS

typedef struct _IrcNetwork      IrcNetwork;
typedef struct _IrcNetworkClass IrcNetworkClass;
#define IRC_NETWORK_TYPE            (irc_network_get_type ())
#define IRC_NETWORK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IRC_NETWORK_TYPE, IrcNetwork))
#define IRC_NETWORK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IRC_NETWORK_TYPE, IrcNetworkClass))
#define IS_IRC_NETWORK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IRC_NETWORK_TYPE))
#define IS_IRC_NETWORK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IRC_NETWORK_TYPE))

struct _IrcNetwork
{
	GObject parent;

	gchar *name;
	gboolean autoconnect;
	gboolean use_ssl;
	gboolean allow_invalid;
	gboolean cycle;
	gboolean reconnect;
	gboolean nogiveup_reconnect;

	gchar *nickserv_password;
	gchar *password;
	gint encoding;

	GSList *servers;

	gboolean use_global;
	gchar *nick;
	gchar *real;
	gchar *autojoin;

	ircnet *net;
};

struct _IrcNetworkClass
{
	GObjectClass parent_class;
};

GType       irc_network_get_type (void) G_GNUC_CONST;
IrcNetwork *irc_network_new (ircnet *net);
void        irc_network_save (IrcNetwork *net);

extern const char *encodings[];

G_END_DECLS

#endif
