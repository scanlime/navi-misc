/*
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
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
#include "notification-area-icon.h"

int  e_plugin_lib_enable (EPluginLib *ep, int enable);
void new_mail (EPlugin *ep, EMEventTargetFolder *target);
void reading_message (EPlugin *ep, EMEventTargetMessage *message);

int
e_plugin_lib_enable (EPluginLib *ep, int enable)
{
	gboolean init = ni_init ();
	ni_set_status (NI_STATUS_NORMAL);
	return (!init);
}

void
new_mail (EPlugin *ep, EMEventTargetFolder *target)
{
	g_print ("new mail\n");
	ni_set_status (NI_STATUS_NEW_MAIL);
}

void
reading_message (EPlugin *ep, EMEventTargetMessage *message)
{
	g_print ("reading message...\n");
	ni_set_status (NI_STATUS_NORMAL);
}
