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
#include <mail/em-config.h>
#include <gtk/gtk.h>
#include <libgnome/gnome-i18n.h>
#include "notification-area-icon.h"

int        e_plugin_lib_enable (EPluginLib *ep, int enable);
void       new_mail (EPlugin *ep, EMEventTargetFolder *target);
void       reading_message (EPlugin *ep, EMEventTargetMessage *message);
GtkWidget *mnotify_notification_area (EPlugin *epl, struct _EConfigHookItemFactoryData *data);
GtkWidget *mnotify_beep (EPlugin *epl, struct _EConfigHookItemFactoryData *data);
GtkWidget *mnotify_sound (EPlugin *epl, struct _EConfigHookItemFactoryData *data);
GtkWidget *mnotify_sound_prefs (EPlugin *epl, struct _EConfigHookItemFactoryData *data);

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

GtkWidget *
mnotify_notification_area (EPlugin *epl, struct _EConfigHookItemFactoryData *data)
{
	GtkWidget *check;

	if (data->old)
		return data->old;

	check = gtk_check_button_new_with_mnemonic (_("_Show an icon in the notification area"));

	gtk_box_pack_start (GTK_BOX (data->parent), check, FALSE, FALSE, 0);
	gtk_widget_show (check);

	return check;
}

GtkWidget *
mnotify_beep (EPlugin *epl, struct _EConfigHookItemFactoryData *data)
{
	GtkWidget *check;

	if (data->old)
		return data->old;

	check = gtk_check_button_new_with_mnemonic (_("Beep w_hen new mail arrives"));

	gtk_box_pack_start (GTK_BOX (data->parent), check, FALSE, FALSE, 0);
	gtk_widget_show (check);

	return check;
}

GtkWidget *
mnotify_sound (EPlugin *epl, struct _EConfigHookItemFactoryData *data)
{
	GtkWidget *check;

	if (data->old)
		return data->old;

	check = gtk_check_button_new_with_mnemonic (_("Play a sound file when new mail _arrives"));

	gtk_box_pack_start (GTK_BOX (data->parent), check, FALSE, FALSE, 0);
	gtk_widget_show (check);

	return check;
}

GtkWidget *
mnotify_sound_prefs (EPlugin *epl, struct _EConfigHookItemFactoryData *data)
{
	GtkWidget *hbox, *label, *entry, *browse, *play;

	if (data->old)
		return data->old;

	hbox = gtk_hbox_new (FALSE, 6);
	label = gtk_label_new_with_mnemonic (_("Specify _filename:"));
	entry = gtk_entry_new ();
	browse = gtk_button_new_with_mnemonic (_("_Browse..."));
	play = gtk_button_new_with_mnemonic (_("_Play"));
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), browse, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), play, FALSE, TRUE, 0);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);

	gtk_box_pack_start (GTK_BOX (data->parent), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all (hbox);

	return hbox;
}
