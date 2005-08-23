/*
 * menu.c - Context menu for the notification plugin.
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

GType
channel_menu_get_type (void)
{
	static GType channel_menu_type = 0;

	if (!channel_menu_type) {
		static const GTypeInfo channel_menu_info = {
			sizeof (ChannelMenuClass),
			NULL,
			NULL,
			(GClassInitFunc) channel_menu_class_init,
			NULL,
			NULL,
			sizeof (ChannelMenu),
			0,
			(GInstanceInitFunc) channel_menu_init,
		};

		channel_menu_type = g_type_register_static (GTK_TYPE_MENU, "ChannelMenu", &channel_menu_info, 0);
	}
}

void
channel_menu_item_add (gchar* name)
{
}

void
channel_menu_remove (gchar* name)
{
}

void
channel_menu_update (gchar* name, gint status)
{
}


void
channel_menu_item_set_status (gint status)
{
}
