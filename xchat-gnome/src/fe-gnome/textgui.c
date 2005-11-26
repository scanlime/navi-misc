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
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif
#include <libgnomevfs/gnome-vfs.h>

#include "textgui.h"
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

int check_word (GtkWidget *xtext, char *word, int len);
static void clicked_word (GtkWidget *xtext, char *word, GdkEventButton *even, gpointer data);
static void font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);
static void background_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data);
static void gconf_timestamps_changed (GConfClient *client, GConfEntry *entry, gpointer data);

static void open_url (GtkAction *action, gpointer data);
static void copy_text (GtkAction *action, gpointer data);
static void send_email (GtkAction *action, gpointer data);

static void on_drop_send_files_activated (GtkAction *action, gpointer data);
static void on_drop_paste_file_activated (GtkAction *action, gpointer data);
static void on_drop_paste_filename_activated (GtkAction *action, gpointer data);
static void on_drop_cancel_activated (GtkAction *action, gpointer data);

static void drag_data_received (GtkWidget *widget, GdkDragContext *context,
				gint x, gint y, GtkSelectionData *selection_data, guint info,
				guint time, gpointer data);


static GHashTable *notify_table;
static gchar *selected_word = NULL;
static GSList *dropped_files = NULL;

static GtkActionEntry action_entries[] = {
	/* URL Popup */
	{ "TextURLOpen", GTK_STOCK_OPEN, N_("_Open Link in Browser"), NULL, NULL, G_CALLBACK (open_url) },
	{ "TextURLCopy", GTK_STOCK_COPY, N_("_Copy Link Location"), NULL, NULL, G_CALLBACK (copy_text) },

	/* Email Popup */
	{ "TextEmailSend", GNOME_STOCK_MAIL, N_("Se_nd Message To..."), NULL, NULL, G_CALLBACK (send_email) },
	{ "TextEmailCopy", GTK_STOCK_COPY, N_("_Copy Address"), NULL, NULL, G_CALLBACK (copy_text) },

	/* Drag and Drop File Popup */
	{ "DropSendFiles", NULL, N_("_Send File"), NULL, NULL, G_CALLBACK (on_drop_send_files_activated) },
	{ "DropPasteFile", NULL, N_("Paste File _Contents"), NULL, NULL, G_CALLBACK (on_drop_paste_file_activated) },
	{ "DropPasteFileName", NULL, N_("Paste File_name"), NULL, NULL, G_CALLBACK (on_drop_paste_filename_activated) },
	{ "DropCancel", NULL, N_("_Cancel"), NULL, NULL, G_CALLBACK (on_drop_cancel_activated) },
};

enum
{
	TARGET_URI_LIST,
	TARGET_UTF8_STRING,
	TARGET_TEXT,
	TARGET_COMPOUND_TEXT,
	TARGET_STRING,
	TARGET_TEXT_PLAIN,
};

static GtkTargetEntry target_table[] = {
	{ "text/uri-list",  0, TARGET_URI_LIST },
	{ "UTF8_STRING",    0, TARGET_UTF8_STRING },
	{ "COMPOUND_TEXT",  0, TARGET_COMPOUND_TEXT },
	{ "TEXT",           0, TARGET_TEXT },
	{ "STRING",         0, TARGET_STRING },
	{ "text/plain",     0, TARGET_TEXT_PLAIN }
};


void
initialize_text_gui (void)
{
	GtkWidget *frame, *scrollbar;
	GConfClient *client;
	gchar *font;
	GtkActionGroup *action_group;
	gint background_type;

	client = gconf_client_get_default ();

	gui.xtext = GTK_XTEXT (gtk_xtext_new (colors, TRUE));
	frame = glade_xml_get_widget (gui.xml, "text area frame");
	gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (gui.xtext));
	scrollbar = glade_xml_get_widget (gui.xml, "text area scrollbar");
	gtk_range_set_adjustment (GTK_RANGE (scrollbar), gui.xtext->adj);

	notify_table = g_hash_table_new (g_direct_hash, g_direct_equal);

	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_set_max_lines (gui.xtext, 3000);
	gtk_xtext_set_show_separator (gui.xtext, prefs.show_separator);
	gtk_xtext_set_indent (gui.xtext, prefs.indent_nicks);
	gtk_xtext_set_max_indent (gui.xtext, prefs.max_auto_indent);
	gtk_xtext_set_thin_separator (gui.xtext, prefs.thin_separator);
	gtk_xtext_set_wordwrap (gui.xtext, prefs.wordwrap);
	gtk_xtext_set_urlcheck_function (gui.xtext, check_word);
	g_signal_connect (G_OBJECT (gui.xtext), "word_click", G_CALLBACK (clicked_word), NULL);

	/* Set menus */
	action_group = gtk_action_group_new ("TextPopups");
	gtk_action_group_add_actions (action_group, action_entries, G_N_ELEMENTS (action_entries), NULL);
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_ui_manager_insert_action_group (gui.manager, action_group, 0);
	g_object_unref (action_group);

	/* Set the font. */
	gconf_client_add_dir (client, "/apps/xchat/main_window", GCONF_CLIENT_PRELOAD_NONE, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) font_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/font",          (GConfClientNotifyFunc) font_changed, NULL, NULL, NULL);


	if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);

	if (font == NULL)
		font = g_strdup ("fixed 11");

	gtk_xtext_set_font (GTK_XTEXT (gui.xtext), font);
	g_free (font);

	/* Set the background */
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_type",         (GConfClientNotifyFunc) background_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_image",        (GConfClientNotifyFunc) background_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_transparency", (GConfClientNotifyFunc) background_changed, NULL, NULL, NULL);

	background_type = gconf_client_get_int (client, "/apps/xchat/main_window/background_type", NULL);
	if (background_type == 0) {
		gtk_xtext_set_tint (gui.xtext, 0, 0, 0);
		gtk_xtext_set_background (gui.xtext, NULL, FALSE);
	} else if (background_type == 1) {
		gchar *filename = gconf_client_get_string (client, "/apps/xchat/main_window/background_image", NULL);
		gtk_xtext_set_tint (gui.xtext, 0, 0, 0);
		gtk_xtext_set_background (gui.xtext, NULL, FALSE);
		if (filename) {
			GdkPixbuf *pixbuf;
			pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
			if (pixbuf) {
				gint width, height;
				GdkPixmap *image;

				width  = gdk_pixbuf_get_width  (pixbuf);
				height = gdk_pixbuf_get_height (pixbuf);

				image = gdk_pixmap_new (NULL, width, height, 24);
				gdk_draw_pixbuf (image, NULL, pixbuf, 0, 0, 0, 0, width, height, GDK_RGB_DITHER_NONE, 0, 0);

				gtk_xtext_set_background (gui.xtext, image, FALSE);
				g_object_unref (pixbuf);
				g_object_unref (image);
			}
		}
		g_free (filename);
	} else {
		float transparency = gconf_client_get_float (client, "/apps/xchat/main_window/background_transparency", NULL);
		int value = 255 - ((int) (transparency * 255));
		gtk_xtext_set_tint (gui.xtext, value, value, value);
		gtk_xtext_set_background (gui.xtext, NULL, TRUE);
	}

	/* Setup drag and drop */
	g_signal_connect (gui.xtext, "drag_data_received", G_CALLBACK (drag_data_received), NULL);
	gtk_drag_dest_set (GTK_WIDGET (gui.xtext), GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP, target_table, G_N_ELEMENTS (target_table), GDK_ACTION_COPY | GDK_ACTION_ASK);

	gtk_xtext_refresh (gui.xtext, TRUE);
	gtk_widget_show_all (GTK_WIDGET (gui.xtext));
	g_object_unref (client);
}

void
text_gui_add_text_buffer (struct session *sess)
{
	session_gui *tgui;
	GConfClient *client;
	gint notify;

	tgui = g_new0 (session_gui, 1);
	tgui->buffer = gtk_xtext_buffer_new (gui.xtext);
	sess->gui = (struct session_gui *) tgui;

	gtk_xtext_buffer_show (gui.xtext, tgui->buffer, TRUE);

	client = gconf_client_get_default ();
	gtk_xtext_set_time_stamp (tgui->buffer, gconf_client_get_bool (client, "/apps/xchat/irc/showtimestamps", NULL));
	notify = gconf_client_notify_add (client, "/apps/xchat/irc/showtimestamps", (GConfClientNotifyFunc) gconf_timestamps_changed, tgui->buffer, NULL, NULL);
	g_hash_table_insert (notify_table, tgui->buffer, GINT_TO_POINTER (notify));
	gui.current_session = sess;
	g_object_unref (client);

	if (sess->topic == NULL) {
		tgui->topic = g_strdup ("");
		tgui->url_topic = g_strdup ("");
	} else {
		tgui->topic = g_strdup (sess->topic);
		tgui->url_topic = g_strdup (sess->topic);
	}
	tgui->entry = g_strdup ("");
	tgui->lag_text = NULL;
	tgui->queue_text = NULL;
}

void
text_gui_remove_text_buffer (struct session *sess)
{
	session_gui *tgui;
	gint notify;
	GConfClient *client;

	tgui = (session_gui *) sess->gui;

	client = gconf_client_get_default ();
	notify = GPOINTER_TO_INT (g_hash_table_lookup (notify_table, tgui->buffer));
	g_hash_table_remove (notify_table, tgui->buffer);
	gconf_client_notify_remove (client, notify);
	g_object_unref (client);

	gtk_xtext_buffer_free (tgui->buffer);
	g_free (tgui->topic);
	g_free (tgui->url_topic);
	g_free (tgui->entry);
	if (tgui->lag_text)
		g_free (tgui->lag_text);
	if (tgui->queue_text)
		g_free (tgui->queue_text);
	g_free (tgui);
	sess->gui = NULL;
}

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

static gchar *
build_url_topic (char *topic)
{
	gchar **tokens;
	gchar *escaped, *result, *temp;
	int i;

	if (strlen (topic) == 0)
		return topic;

	/* escape out <>&"' so that pango markup doesn't get confused */
	escaped = g_markup_escape_text (topic, strlen (topic));

	/* surround urls with <a> markup so that sexy-url-label can link it */
	tokens = g_strsplit_set (escaped, " \t\n", 0);
	if (url_check_word (tokens[0], strlen (tokens[0])) == WORD_URL)
		result = g_strdup_printf ("<a href=\"%s\">%s</a>", tokens[0], tokens[0]);
	else
		result = g_strdup (tokens[0]);
	for (i = 1; tokens[i]; i++) {
		if (url_check_word (tokens[i], strlen (tokens[i])) == WORD_URL) {
			temp = g_strdup_printf ("%s <a href=\"%s\">%s</a>", result, tokens[i], tokens[i]);
			g_free (result);
			result = temp;
		} else {
			temp = g_strdup_printf ("%s %s", result, tokens[i]);
			g_free (result);
			result = temp;
		}
	}
	g_strfreev (tokens);

	g_free (escaped);

	return result;
}

void
set_gui_topic (session *sess, char *topic)
{
	session_gui *tgui = (session_gui *) sess->gui;

	gchar *escaped = build_url_topic (topic);

	g_free (tgui->topic);
	g_free (tgui->url_topic);
	if (topic == NULL) {
		if (sess->topic == NULL) {
			tgui->topic = g_strdup ("");
			tgui->url_topic = g_strdup ("");
		} else {
			tgui->topic = g_strdup (sess->topic);
			tgui->url_topic = g_strdup (sess->topic);
		}
	} else {
		tgui->topic = g_strdup (topic);
		tgui->url_topic = escaped;
	}
	if (sess == gui.current_session) {
#ifdef HAVE_LIBSEXY
		sexy_url_label_set_markup (SEXY_URL_LABEL (gui.topic_label), tgui->url_topic);
#else
		gtk_label_set_text (GTK_LABEL (gui.topic_label), tgui->topic);
#endif
	}
}

void
clear_buffer (struct session *sess)
{
	session_gui *sgui;

	if (sess == NULL)
		return;

	sgui = (session_gui *) sess->gui;
	gtk_xtext_clear (sgui->buffer);
	gtk_xtext_refresh (gui.xtext, FALSE);
}

int
check_word (GtkWidget *xtext, char *word, int len)
{
	int url;
	current_sess = gui.current_session;

	url = url_check_word (word, len);
	if (url == 0) {
		if (((word[0]=='@' || word[0]=='+') && userlist_find (current_sess, word+1)) || userlist_find (current_sess, word))
			return WORD_NICK;
		if (current_sess->type == SESS_DIALOG)
			return WORD_DIALOG;
	}
	return url;
}

static void
clicked_word (GtkWidget *xtext, char *word, GdkEventButton *event, gpointer data)
{
	if (word == NULL)
		return;

	if (event->button == 1) {
		/* left click */
		int type = check_word (xtext, word, strlen (word));

		switch (type) {
		case 0:
			return;
		case WORD_URL:
		case WORD_HOST:
			if (selected_word)
				g_free (selected_word);
			selected_word = g_strdup (word);
			open_url (NULL, NULL);
			break;
		}
		return;
	}
	if (event->button == 2) {
		/* middle click */
		return;
	}
	if (event->button == 3) {
		switch (check_word (xtext, word, strlen (word))) {
		case 0:
			/* FIXME: show default context menu */
			return;
		case WORD_URL:
		case WORD_HOST:
			{
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/TextURLPopup");
				g_return_if_fail (menu != NULL);
				if (selected_word)
					g_free (selected_word);
				selected_word = g_strdup (word);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
				return;
			}
		case WORD_NICK:
			{
				struct User *user;
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");
				g_return_if_fail (menu != NULL);
				if (selected_word)
					g_free (selected_word);
				selected_word = g_strdup (word);

				user = userlist_find (gui.current_session, word);
				if (user) {
					current_user = user;
					gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
				}
				return;
			}
		case WORD_CHANNEL:
			/* FIXME: show channel context menu */
			/* This will need to be a bit more complicated than the channel context
			 * menu shown in the navigation tree, since we'll need to check whether
			 * we've joined the channel and display different actions.  come to think
			 * of it, that context menu doesn't actually change based on context
			 * (joined/parted)
			 */
			return;
		case WORD_EMAIL:
			{
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/TextEmailPopup");
				g_return_if_fail (menu != NULL);
				if (selected_word)
					g_free (selected_word);
				selected_word = g_strdup (word);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
				return;
			}
		case WORD_DIALOG:
			/* FIXME: show dialog(?) context menu */
			/* See comment in channel case above */
			return;
		}
	}
}

static void
font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data)
{
	gchar *font;
	GtkAdjustment *adj;

	if (gconf_client_get_bool (client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);

	gtk_xtext_set_font (gui.xtext, font);
	adj = gui.xtext->adj;
	gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
	gtk_xtext_refresh (gui.xtext, FALSE);

	g_free (font);
}

static void
background_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer data)
{
	guint background_type = gconf_client_get_int (client, "/apps/xchat/main_window/background_type", NULL);
	if (background_type == 0) {
		gtk_xtext_set_tint (gui.xtext, 0, 0, 0);
		gtk_xtext_set_background (gui.xtext, NULL, FALSE);
	} else if (background_type == 1) {
		gchar *filename = gconf_client_get_string (client, "/apps/xchat/main_window/background_image", NULL);
		gtk_xtext_set_tint (gui.xtext, 0, 0, 0);
		gtk_xtext_set_background (gui.xtext, NULL, FALSE);
		if (filename) {
			GdkPixbuf *pixbuf;
			pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
			if (pixbuf) {
				gint width, height;
				GdkPixmap *image;

				width  = gdk_pixbuf_get_width  (pixbuf);
				height = gdk_pixbuf_get_height (pixbuf);

				image = gdk_pixmap_new (NULL, width, height, 24);
				gdk_draw_pixbuf (image, NULL, pixbuf, 0, 0, 0, 0, width, height, GDK_RGB_DITHER_NONE, 0, 0);

				gtk_xtext_set_background (gui.xtext, image, FALSE);
				g_object_unref (pixbuf);
				g_object_unref (image);
			}
		}
		g_free (filename);
	} else {
		float transparency = gconf_client_get_float (client, "/apps/xchat/main_window/background_transparency", NULL);
		int value = 255 - ((int) (transparency * 255));
		gtk_xtext_set_tint (gui.xtext, value, value, value);
		gtk_xtext_set_background (gui.xtext, NULL, TRUE);
	}
	gtk_xtext_refresh (gui.xtext, TRUE);
}

static void
gconf_timestamps_changed (GConfClient *client, GConfEntry *entry, gpointer data)
{
	gtk_xtext_set_time_stamp (data, gconf_client_get_bool (client, entry->key, NULL));
}

static void
open_url (GtkAction *action, gpointer data)
{
	fe_open_url (selected_word);
}

static void
copy_text (GtkAction *action, gpointer data)
{
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_NONE);
	gtk_clipboard_set_text (clipboard, selected_word, strlen (selected_word));
}

static void
send_email (GtkAction *action, gpointer data)
{
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
drag_data_received (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint info, guint time, gpointer data)
{
	switch (info) {
	case TARGET_TEXT:
	case TARGET_STRING:
	case TARGET_COMPOUND_TEXT:
	case TARGET_UTF8_STRING:
	case TARGET_TEXT_PLAIN:
		{
			gchar *txt;

			txt = gtk_selection_data_get_text (selection_data);
			if (gui.current_session != NULL)
				handle_multiline (gui.current_session, txt, TRUE, FALSE);

			g_free (txt);
			break;
		}

	case TARGET_URI_LIST:
		{
			gchar *uri_list, **uris;
			gint nb_uri;

			if ((gui.current_session->type != SESS_CHANNEL) && (gui.current_session->type != SESS_DIALOG))
				return;

			if (selection_data->format != 8 || selection_data->length == 0) {
				g_printerr (_("URI list dropped on xchat-gnome had wrong format (%d) or length (%d)\n"), selection_data->format, selection_data->length);
				return;
			}

			uri_list = g_strndup (selection_data->data, selection_data->length);
			uris = g_strsplit (uri_list, "\r\n", 0);
			g_free (uri_list);

			free_dropped_files ();

			for (nb_uri = 0; uris[nb_uri] && strlen (uris[nb_uri]) > 0; nb_uri++)
				dropped_files = g_slist_prepend (dropped_files, uris[nb_uri]);
			g_free (uris); /* String in uris will be freed in free_dropped_files */
			dropped_files = g_slist_reverse (dropped_files);

			if (context->action == GDK_ACTION_ASK) {
				/* Display the context menu */
				GtkWidget *menu, *entry;

				menu = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup");
				g_return_if_fail (menu != NULL);

				/* Enable/Disable paste file content */
				entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropPasteFile");
				g_return_if_fail (entry != NULL);
				if ( nb_uri > 1 || !uri_is_text (dropped_files->data) || !check_file_size (dropped_files->data))
					gtk_widget_set_sensitive (entry, FALSE);
				else
					gtk_widget_set_sensitive (entry, TRUE);

				/* Enable/Disable send files */
				entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropSendFiles");
				g_return_if_fail (entry != NULL);
				if (gui.current_session->type == SESS_CHANNEL)
					gtk_widget_set_sensitive (entry, FALSE);
				else
					gtk_widget_set_sensitive (entry, TRUE);

				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 2, gtk_get_current_event_time ());
			} else {
				/* Do the default action */
				if (gui.current_session->type == 2) {
					/* Droped in a channel */
					if (nb_uri == 1 && uri_is_text (dropped_files->data) && check_file_size (dropped_files->data))
						drop_paste_file ();
				} else {
					/* Droped in a query */
					if (nb_uri == 1 && uri_is_text (dropped_files->data) && check_file_size (dropped_files->data))
						drop_paste_file ();
					else
						drop_send_files ();
				}
			}
		}
	}
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
