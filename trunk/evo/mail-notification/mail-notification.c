/*
 * 
 *
 * Copyright (C) 2004 David Trowbridge
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

#include <mail/em-event.h>
#include <libgnomeui/gnome-stock-icons.h>
#include "eggstatusicon.h"

static EggStatusIcon *icon = NULL;

int
e_plugin_lib_enable (EPluginLib *ep, int enable)
{
	icon = egg_status_icon_new_from_stock (GNOME_STOCK_MAIL);
	return (icon == NULL);
}

void
new_mail (EPlugin *ep, EMEventTargetFolder *target)
{
	g_print ("new mail\n");
	egg_status_icon_set_from_stock (icon, GNOME_STOCK_MAIL_NEW);
	egg_status_icon_set_is_blinking (icon, TRUE);
}

void
reading_message (EPlugin *ep, EMEventTargetMessage *message)
{
	g_print ("reading message...\n");
}
