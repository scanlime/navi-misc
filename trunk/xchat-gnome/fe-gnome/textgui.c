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

#include "textgui.h"
#include "palette.h"
#include "preferences.h"
#include "../common/text.h"
#include "../common/xchatc.h"
#include "../common/fe.h"
#include "../common/url.h"
#include <libgnome/gnome-url.h> /* gnome_url_show */
#include <gconf/gconf-client.h>

#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif

int check_word (GtkWidget *xtext, char *word, int len);
void clicked_word (GtkWidget *xtext, char *word, GdkEventButton *even, gpointer data);
void font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);
static void gconf_timestamps_changed (GConfClient *client, GConfEntry *entry, gpointer data);

static void open_url (GtkAction *action, gpointer data);
static void copy_text (GtkAction *action, gpointer data);
static void send_email (GtkAction *action, gpointer data);

static GHashTable *notify_table;
static gchar *selected_word = NULL;

static GtkActionEntry action_entries[] = {
	/* URL Popup */
	{ "TextURLOpen", GTK_STOCK_OPEN, _("_Open Link in Browser"), NULL, NULL, G_CALLBACK (open_url) },
	{ "TextURLCopy", GTK_STOCK_COPY, _("_Copy Link Location"), NULL, NULL, G_CALLBACK (copy_text) },

	/* Email Popup */
	{ "TextEmailSend", GNOME_STOCK_MAIL, _("Se_nd Message To..."), NULL, NULL, G_CALLBACK (send_email) },
	{ "TextEmailCopy", GTK_STOCK_COPY, _("_Copy Address"), NULL, NULL, G_CALLBACK (copy_text) },
};

void
initialize_text_gui ()
{
	GtkWidget *frame, *scrollbar;
	/* For setting the font from gconf. */
	GConfClient *client;
	gchar *font;
	GtkActionGroup *action_group;

	gui.xtext = GTK_XTEXT (gtk_xtext_new (colors, TRUE));
	frame = glade_xml_get_widget (gui.xml, "text area frame");
	gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (gui.xtext));
	scrollbar = glade_xml_get_widget (gui.xml, "text area scrollbar");
	gtk_range_set_adjustment (GTK_RANGE (scrollbar), gui.xtext->adj);

	notify_table = g_hash_table_new (g_direct_hash, g_direct_equal);

	palette_alloc (GTK_WIDGET (gui.xtext));
	gtk_xtext_set_palette (gui.xtext, colors);
	gtk_xtext_set_background (gui.xtext, NULL, FALSE, FALSE);
	gtk_xtext_set_max_lines (gui.xtext, 3000);
	gtk_xtext_set_show_separator (gui.xtext, TRUE);
	gtk_xtext_set_indent (gui.xtext, TRUE);
	gtk_xtext_set_max_indent (gui.xtext, 500);
	gtk_xtext_set_thin_separator (gui.xtext, TRUE);
	gtk_xtext_set_wordwrap (gui.xtext, TRUE);
	gtk_xtext_set_urlcheck_function (gui.xtext, check_word);
	g_signal_connect (G_OBJECT (gui.xtext), "word_click", G_CALLBACK (clicked_word), NULL);
	gtk_xtext_refresh (gui.xtext, FALSE);

	/* Set menus */
	action_group = gtk_action_group_new ("TextPopups");
	gtk_action_group_add_actions (action_group, action_entries, G_N_ELEMENTS (action_entries), NULL);
	gtk_ui_manager_insert_action_group (gui.manager, action_group, 0);
	g_object_unref (action_group);

	/* Set the font. */
	client = gconf_client_get_default ();
	gconf_client_add_dir (client, "/apps/xchat/main_window", GCONF_CLIENT_PRELOAD_NONE, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc)font_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/font", (GConfClientNotifyFunc)font_changed, NULL, NULL, NULL);


	if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);

	if (font == NULL)
		font = g_strdup ("fixed 11");

	gtk_xtext_set_font (GTK_XTEXT (gui.xtext), font);
	g_object_unref (client);
	g_free (font);

	gtk_widget_show_all (GTK_WIDGET (gui.xtext));
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

void
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
			gtk_label_set_text (GTK_LABEL (nick), serv->nick);
		else
			gtk_label_set_text (GTK_LABEL (nick), newnick);
	}
}

static gchar *
build_url_topic (char *topic)
{
	gchar **tokens;
	gchar *escaped, *result, *temp;
	int i;

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
	if (sess == NULL)
		return;
	session_gui *sgui = (session_gui *) sess->gui;
	gtk_xtext_clear (sgui->buffer);
}

int
check_word (GtkWidget *xtext, char *word, int len)
{
	current_sess = gui.current_session;
	return url_check_word (word, len);
}

void
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
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");
				g_return_if_fail (menu != NULL);
				if (selected_word)
					g_free (selected_word);
				selected_word = g_strdup (word);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
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

void
font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data)
{
	gchar *font;
	GtkAdjustment *adj;

	if (gconf_client_get_bool (client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);

	gtk_xtext_set_font (GTK_XTEXT (gui.xtext), font);
	adj = GTK_XTEXT (gui.xtext)->adj;
	gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
	gtk_xtext_refresh (GTK_XTEXT (gui.xtext), 0);

	g_free (font);
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
