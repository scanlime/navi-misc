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

#include "irc-network.h"
#include <gtk/gtkdialog.h>
#include <gtk/gtkliststore.h>
#include <glade/glade.h>
#include <gconf/gconf-client.h>

#ifndef __XCHAT_GNOME_IRC_NETWORK_EDITOR_H__
#define __XCHAT_GNOME_IRC_NETWORK_EDITOR_H__

G_BEGIN_DECLS

typedef struct _IrcNetworkEditor      IrcNetworkEditor;
typedef struct _IrcNetworkEditorClass IrcNetworkEditorClass;
#define IRC_NETWORK_EDITOR_TYPE            (irc_network_editor_get_type ());
#define IRC_NETWORK_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), IRC_NETWORK_EDITOR_TYPE, IrcNetworkEditor))
#define IRC_NETWORK_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), IRC_NETWORK_EDITOR_TYPE, IrcNetworkEditorClass))
#define IS_IRC_NETWORK_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IRC_NETWORK_EDITOR_TYPE))
#define IS_IRC_NETWORK_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), IRC_NETWORK_EDITOR_TYPE))

struct _IrcNetworkEditor
{
	GtkDialog parent;

	IrcNetwork *network;
	GConfClient *gconf;
	GladeXML *xml;

	GtkWidget *network_name;

	GtkWidget *autoconnect;
	GtkWidget *use_ssl;
	GtkWidget *cycle;
	GtkWidget *autoreconnect;
	GtkWidget *giveup_reconnect;

	GtkWidget *password;
	GtkWidget *encoding_hbox;
	GtkWidget *encoding;

	GtkWidget *servers;
	GtkWidget *add_server;
	GtkWidget *edit_server;
	GtkWidget *remove_server;

	GtkWidget *use_globals;
	GtkWidget *use_custom;
	GtkWidget *nickname;
	GtkWidget *realname;

	GtkWidget *autojoin_channels;
	GtkWidget *add_autojoin;
	GtkWidget *edit_autojoin;
	GtkWidget *remove_autojoin;

	GtkWidget *toplevel;

	GtkListStore *store;
};

struct _IrcNetworkEditorClass
{
	GtkDialogClass parent_class;
};

GType             irc_network_editor_get_type (void) G_GNUC_CONST;
IrcNetworkEditor *irc_network_editor_new (IrcNetwork *network);
void              irc_network_editor_run (IrcNetworkEditor *editor);

G_END_DECLS

#endif
