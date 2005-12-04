/*
 * textgui.c - helpers for the main text gui
 *
 * Copyright (C) 2004-2005 xchat-gnome team
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
#include <libgnome/gnome-url.h> /* gnome_url_show */
#include <libgnomeui/gnome-stock-icons.h>
#include <gconf/gconf-client.h>
#include <libgnomevfs/gnome-vfs.h>

#include "status-bar.h"
#include "text-entry.h"
#include "textgui.h"
#include "topic-label.h"
#include "palette.h"
#include "preferences.h"
#include "userlist-gui.h"

#include "../common/xchat.h"
#include "../common/xchatc.h"
#include "../common/text.h"
#include "../common/userlist.h"
#include "../common/fe.h"
#include "../common/url.h"
#include "../common/outbound.h"

void
set_nickname (struct server *serv, char *newnick)
{
	if (serv == gui.current_session->server) {
		GtkWidget *nick = glade_xml_get_widget (gui.xml, "nickname");
		if (newnick == NULL)
			gtk_button_set_label (GTK_BUTTON (nick), serv->nick);
		else
			gtk_button_set_label (GTK_BUTTON (nick), newnick);
	}
}
