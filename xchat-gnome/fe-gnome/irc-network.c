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

const char *encodings[] =
{
	"UTF-8",
	"ISO-8859-15 (Western Europe)",
	"ISO-8859-2 (Central Europe)",
	"ISO-8859-7 (Greek)",
	"ISO-8859-8 (Hebrew)",
	"ISO-8859-9 (Turkish)",
	"ISO-2022-JP (Japanese)",
	"SJIS (Japanese)",
	"CP949 (Korean)",
	"CP1251 (Cyrillic)",
	"CP1256 (Arabic)",
	"GB18030 (Chinese)",
	NULL
};

static void
irc_network_finalize (GObject *object)
{
	IrcNetwork *network = IRC_NETWORK (object);

	g_free (network->name);
	g_free (network->password);
	g_free (network->nick);
	g_free (network->real);

	g_slist_foreach (network->servers, g_free, NULL);
	g_slist_free (network->servers);

	g_slist_foreach (network->autojoin, g_free, NULL);
	g_slist_free (network->autojoin);
}

static void
irc_network_class_init (IrcNetworkClass *klass)
{
	GObjectClass *object_class = (GObjectClass *) klass;

	object_class->finalize = irc_network_finalize;
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
	IrcNetwork *n = IRC_NETWORK (g_object_new (irc_network_get_type (), 0));

	n->name        = g_strdup (net->name);
	n->autoconnect = net->flags & FLAG_AUTO_CONNECT;
	n->use_ssl     = net->flags & FLAG_USE_SSL;
	n->cycle       = net->flags & FLAG_CYCLE;
	/* FIXME */
	/*
	n->reconnect   = net->flags & FLAG_;
	n->novegiveup  = net->flags & FLAG_;
	*/

	n->password    = g_strdup(net->pass);
	/* FIXME */
	n->encoding    = 0;

	n->use_global  = net->flags & FLAG_USE_GLOBAL;
	n->nick        = g_strdup (net->nick);
	n->real        = g_strdup (net->real);
	/* FIXME */
	n->autojoin    = NULL;

	n->net = net;

	return n;
}

void
irc_network_save (IrcNetwork *network)
{
	ircnet *net = network->net;
	guint32 flags = 0;

	if (net->name) g_free (net->name);
	if (net->pass) g_free (net->pass);
	if (net->nick) g_free (net->nick);
	if (net->real) g_free (net->real);

	net->name = g_strdup (network->name);
	net->pass = g_strdup (network->password);
	net->nick = g_strdup (network->nick);
	net->real = g_strdup (network->real);

	if (network->autoconnect) flags |= FLAG_AUTO_CONNECT;
	if (network->use_ssl)     flags |= FLAG_USE_SSL;
	if (network->cycle)       flags |= FLAG_CYCLE;
	if (network->use_global)  flags |= FLAG_USE_GLOBAL;
	net->flags = flags;

	/* FIXME - autojoin */
	/* FIXME - encoding */
	/* FIXME - reconnect */
	/* FIXME - nogiveup */
}
