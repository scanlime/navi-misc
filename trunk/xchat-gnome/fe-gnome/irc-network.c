/*
 * irc-network.c - an object which interfaces around the core 'ircnet'
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

#include "irc-network.h"

static void
irc_network_dispose (GObject *object)
{
}

static void
irc_network_class_init (IrcNetworkClass *klass)
{
	GObjectClass *object_class;

	object_class->dispose = irc_network_dispose;
}

static void
irc_network_init (IrcNetwork *obj)
{
}

GType
irc_network_get_type (void)
{
	static GType irc_network_type = 0;
	if (!irc_network_type) {
		static const GTypeInfo irc_network_info = {
			sizeof (IrcNetworkClass),
			NULL, NULL,
			(GClassInitFunc) irc_network_class_init,
			NULL, NULL,
			sizeof (IrcNetwork),
			0,
			(GInstanceInitFunc) irc_network_init,
		};

		irc_network_type = g_type_register_static (G_TYPE_OBJECT, "IrcNetwork", &irc_network_info, 0);
	}

	return irc_network_type;
}

IrcNetwork *
irc_network_new (ircnet *net)
{
}

void
irc_network_save (IrcNetwork *net)
{
}
