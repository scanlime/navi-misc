/*
 * irc-network-editor.h - GtkDialog subclass for editing an IrcNetwork
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

#include "irc-network-editor.h"

static void
irc_network_editor_class_init (IrcNetworkEditorClass *klass)
{
}

static void
irc_network_editor_init (IrcNetworkEditor *obj)
{
}

GType
irc_network_editor_get_type (void)
{
	static GType irc_network_editor_type = 0;
	if (!irc_network_editor_type) {
		static const GTypeInfo irc_network_editor_info = {
			sizeof (IrcNetworkEditorClass),
			NULL, NULL,
			(GClassInitFunc) irc_network_editor_class_init,
			NULL, NULL,
			sizeof (IrcNetworkEditor),
			0,
			(GInstanceInitFunc) irc_network_editor_init,
		};

		irc_network_editor_type = g_type_register_static (GTK_TYPE_DIALOG, "IrcNetworkEditor", &irc_network_editor_info, 0);
	}

	return irc_network_editor_type;
}

IrcNetworkEditor *
irc_network_editor_new (IrcNetwork *network)
{
	//IrcNetworkEditor *e = IRC_NETWORK_EDITOR (g_object_new (irc_network_editor_get_type (), 0));
	IrcNetworkEditor *e = g_object_new (irc_network_editor_get_type (), 0);

	e->network = network;
}

void
irc_network_editor_run (IrcNetworkEditor *editor)
{
}
