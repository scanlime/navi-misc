/*
 * irc-network.c - an object which interfaces around the core 'ircnet'
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

#include <config.h>
#include <glib/gi18n.h>
#include <string.h>
#include "irc-network.h"
#include "navigation-tree.h"
#include "gui.h"
#include "../common/server.h"

const char *encodings[] =
{
	N_("UTF-8 (Unicode)"),
	N_("ISO-8859-15 (Western Europe)"),
	N_("ISO-8859-2 (Central Europe)"),
	N_("ISO-8859-7 (Greek)"),
	N_("ISO-8859-8 (Hebrew)"),
	N_("ISO-8859-9 (Turkish)"),
	N_("ISO-2022-JP (Japanese)"),
	N_("SJIS (Japanese)"),
	N_("CP949 (Korean)"),
	N_("KOI8-R (Cyrillic)"),
	N_("CP1251 (Cyrillic)"),
	N_("CP1256 (Arabic)"),
	N_("CP1257 (Baltic)"),
	N_("GB18030 (Chinese)"),
	N_("TIS-620 (Thai)"),
	NULL
};

static GHashTable *enctoindex = NULL;
static GObjectClass *parent_class;

static void
irc_network_finalize (GObject *object)
{
	IrcNetwork *network = IRC_NETWORK (object);
	GSList *s;

	g_free (network->name);
	g_free (network->nickserv_password);
	g_free (network->password);
	g_free (network->nick);
	g_free (network->real);

	for (s = network->servers; s; s = g_slist_next (s)) {
		g_free (((ircserver *) s->data)->hostname);
	}
	g_slist_foreach (network->servers, (GFunc) g_free, NULL);
	g_slist_free (network->servers);

	parent_class->finalize (object);
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
	if (!enctoindex) {
		gchar **enc = (gchar **) encodings;
		gint index = 0;

		enctoindex = g_hash_table_new (g_str_hash, g_str_equal);
		do {
			g_hash_table_insert (enctoindex, *enc, GUINT_TO_POINTER (index));
			enc++; index++;
		} while (*enc);
	}
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

		parent_class = g_type_class_ref (G_TYPE_OBJECT);
	}

	return irc_network_type;
}

IrcNetwork *
irc_network_new (ircnet *net)
{
	GSList *s1, *s2 = NULL;
	IrcNetwork *n = IRC_NETWORK (g_object_new (irc_network_get_type (), 0));

	if (net == NULL) {
		n->net = NULL;
		n->servers = NULL;
		return n;
	}

	n->name        = g_strdup (net->name);
	n->autoconnect = net->flags & FLAG_AUTO_CONNECT;
	n->use_ssl     = net->flags & FLAG_USE_SSL;
	n->allow_invalid = net->flags & FLAG_ALLOW_INVALID;
	n->cycle       = net->flags & FLAG_CYCLE;

	for (s1 = net->servlist; s1; s1 = g_slist_next (s1)) {
		ircserver *s = g_new0(ircserver, 1);
		s->hostname = g_strdup(((ircserver *) (s1->data))->hostname);
		s2 = g_slist_prepend (s2, s);
	}
	n->servers = s2;

	n->nickserv_password = g_strdup (net->nickserv);
	n->password    = g_strdup(net->pass);
	if (net->encoding)
		n->encoding = GPOINTER_TO_UINT (g_hash_table_lookup (enctoindex, net->encoding));
	else
		n->encoding = 0;
	n->use_global  = net->flags & FLAG_USE_GLOBAL;
	n->nick        = g_strdup (net->nick);
	n->real        = g_strdup (net->real);
	n->autojoin    = g_strdup (net->autojoin);

	n->net = net;

	return n;
}

void
irc_network_save (IrcNetwork *network)
{
	ircnet *net = network->net;
	guint32 flags = 0;
	GSList *s;
	struct server *serv;

	if (net == NULL) {
		net = servlist_net_add (network->name, "", TRUE);
	}

	if (net->name)     g_free (net->name);
	if (net->pass)     g_free (net->pass);
	if (net->nick)     g_free (net->nick);
	if (net->real)     g_free (net->real);
	if (net->nickserv) g_free (net->nickserv);
	if (net->autojoin) g_free (net->autojoin);
	if (net->encoding) g_free (net->encoding);

	net->name     = g_strdup (network->name);
	if (network->password && strlen (network->password) != 0)
		net->pass = g_strdup (network->password);
	else
		net->pass = NULL;
	net->nick     = g_strdup (network->nick);
	net->real     = g_strdup (network->real);
	if (network->nickserv_password && strlen (network->nickserv_password) != 0)
		net->nickserv = g_strdup (network->nickserv_password);
	else
		net->nickserv = NULL;
	net->autojoin = g_strdup (network->autojoin);
	net->encoding = g_strdup (encodings[network->encoding]);

	serv = navigation_model_get_server (gui.tree_model, net);
	if (serv)
		server_set_encoding (serv, net->encoding);

	if (network->autoconnect) flags |= FLAG_AUTO_CONNECT;
	if (network->use_ssl)     flags |= FLAG_USE_SSL;
	if (network->allow_invalid) flags |= FLAG_ALLOW_INVALID;
	if (network->cycle)       flags |= FLAG_CYCLE;
	if (network->use_global)  flags |= FLAG_USE_GLOBAL;
	net->flags = flags;

	while (net->servlist) {
		ircserver *is = net->servlist->data;
		if (is && is->hostname)
			servlist_server_remove (net, is);
	}
	for (s = network->servers; s; s = g_slist_next (s)) {
		ircserver *is = s->data;
		if (!(is == NULL || is->hostname == NULL))
			servlist_server_add (net, is->hostname);
	}

	servlist_save ();
}
