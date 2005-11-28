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
#include <gconf/gconf-client.h>
#include <libgnomevfs/gnome-vfs.h>

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

#define DROP_FILE_PASTE_MAX_SIZE 1024

static void gconf_timestamps_changed (GConfClient *client, GConfEntry *entry, gpointer data);

static void on_drop_send_files_activated (GtkAction *action, gpointer data);
static void on_drop_paste_file_activated (GtkAction *action, gpointer data);
static void on_drop_paste_filename_activated (GtkAction *action, gpointer data);
static void on_drop_cancel_activated (GtkAction *action, gpointer data);

static GSList *dropped_files = NULL;

static void
text_gui_print_line (xtext_buffer *buf, unsigned char *text, int len, gboolean indent)
{
	int leftlen;
	unsigned char *tab;
	if (len == 0)
		len = 1;

	if (!indent) {
		int stamp_size;
		char *stamp;
		unsigned char *new_text;

		stamp_size = get_stamp_str (prefs.stamp_format, time(NULL), &stamp);
		new_text = g_malloc (len + stamp_size + 1);
		memcpy (new_text, stamp, stamp_size);
		g_free (stamp);
		memcpy (new_text + stamp_size, text, len);
		gtk_xtext_append (buf, new_text, len + stamp_size);
		g_free (new_text);
		return;
	}

	tab = strchr (text, '\t');
	if (tab && tab < (text + len)) {
		leftlen = tab - text;
		gtk_xtext_append_indent (buf, text, leftlen, tab + 1, len - (leftlen + 1));
	} else {
		gtk_xtext_append_indent (buf, 0, 0, text, len);
	}
}

void
text_gui_print (xtext_buffer *buf, unsigned char *text, gboolean indent)
{
	char *last_text = text;
	int len = 0;

	/* split the text into separate lines */
	while (1) {
		switch (*text) {
		case '\0':
			text_gui_print_line (buf, last_text, len, indent);
			return;
		case '\n':
			text_gui_print_line (buf, last_text, len, indent);
			text++;
			if (*text == '\0')
				return;
			last_text = text;
			len = 0;
			break;
		case ATTR_BEEP:
			*text = ' ';
			gdk_beep ();
		default:
			text++;
			len++;
		}
	}
}

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

void
set_gui_topic (session *sess, char *topic)
{
	session_gui *tgui = (session_gui *) sess->gui;

	g_free (tgui->topic);
	if (topic == NULL) {
		if (sess->topic == NULL)
			tgui->topic = g_strdup ("");
		else
			tgui->topic = g_strdup (sess->topic);
	} else {
		tgui->topic = topic_label_get_topic_string (topic);
	}
	if (sess == gui.current_session)
		topic_label_set_topic (TOPIC_LABEL (gui.topic_label), tgui->topic);
}

void
clear_buffer (struct session *sess)
{
	session_gui *sgui;

	if (sess == NULL)
		return;

	sgui = (session_gui *) sess->gui;
	gtk_xtext_clear (sgui->buffer);
	//gtk_xtext_refresh (gui.xtext, FALSE);
}

static void
gconf_timestamps_changed (GConfClient *client, GConfEntry *entry, gpointer data)
{
	gtk_xtext_set_time_stamp (data, gconf_client_get_bool (client, entry->key, NULL));
}

static gboolean
uri_is_text (gchar *uri)
{
	gchar *mime;
	gboolean is_text = FALSE;

	mime = gnome_vfs_get_mime_type (uri);
	if (mime) {
		is_text = g_str_has_prefix (mime, "text/");
	       	g_free (mime);
	}

	return is_text;
}

static gboolean
check_file_size (gchar *uri)
{
	GnomeVFSResult result;
	GnomeVFSFileInfo *info;
	gboolean file_size_ok = FALSE;

	info = gnome_vfs_file_info_new ();
	result = gnome_vfs_get_file_info (uri, info, GNOME_VFS_FILE_INFO_DEFAULT);

	if (result == GNOME_VFS_OK) {
		if (info->size <= DROP_FILE_PASTE_MAX_SIZE)
			file_size_ok = TRUE;
	} else {
		g_printerr (_("Error retrieving file information for \"%s\": %s\n"), uri, gnome_vfs_result_to_string (result));
	}

	gnome_vfs_file_info_unref (info);
	return file_size_ok;
}

static void
free_dropped_files (void)
{
	if (dropped_files) {
		g_slist_foreach (dropped_files, (GFunc) g_free, NULL);
		g_slist_free (dropped_files);
		dropped_files = NULL;
	}
}

static void
send_file (gpointer file, gpointer user_data)
{
	gchar *path;
	GError *err = NULL;

	path = g_filename_from_uri ((char*) file, NULL, &err);

	if (err) {
		g_printerr (_("Error converting URI \"%s\" into filename: %s\n"), (char*) file, err->message);
		g_error_free (err);
	} else {
		dcc_send (gui.current_session, gui.current_session->channel, path, 0, FALSE);
		g_free (path);
	}
}

static void
drop_send_files (void)
{
	g_return_if_fail (dropped_files != NULL);
	g_slist_foreach (dropped_files, send_file, NULL);
	free_dropped_files ();
}

static void
drop_paste_file (void)
{
	GnomeVFSResult result;
	gchar *contents, *uri;

	g_return_if_fail (g_slist_length (dropped_files) == 1);

	uri = dropped_files->data;
	result = gnome_vfs_read_entire_file (uri, NULL, &contents);
	if (result == GNOME_VFS_OK) {
		if (gui.current_session != NULL)
			handle_multiline (gui.current_session, (char *) contents, TRUE, FALSE);
		g_free (contents);
	} else {
		g_printerr (_("Error reading file \"%s\": %s\n"), uri, gnome_vfs_result_to_string (result));
	}

	free_dropped_files ();
}

static void
on_drop_send_files_activated (GtkAction *action, gpointer data)
{
	/* Should be dropped in a query */
	g_return_if_fail (gui.current_session->type == 3);

	drop_send_files ();
}

static void
on_drop_paste_file_activated (GtkAction *action, gpointer data)
{
	drop_paste_file ();
}

static void
on_drop_paste_filename_activated (GtkAction *action, gpointer data)
{
	gchar *txt = NULL, *path, *tmp;
	GSList *l;

	g_return_if_fail (dropped_files != NULL);

	for (l = dropped_files; l != NULL; l = g_slist_next (l)) {
		path = g_filename_from_uri (l->data, NULL, NULL);
		if (path == NULL) path = g_strdup (l->data);

		if (txt == NULL) {
			txt = g_strdup (path);
		} else {
			tmp = txt;
			txt = g_strdup_printf ("%s %s", tmp, path);

			g_free (tmp);
		}

		g_free (path);
	}

	if (gui.current_session != NULL)
		handle_multiline (gui.current_session, txt, TRUE, FALSE);

	g_free (txt);
	free_dropped_files ();
}

static void
on_drop_cancel_activated (GtkAction *action, gpointer data)
{
	free_dropped_files ();
}
