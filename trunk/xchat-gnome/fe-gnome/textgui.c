/*
 * textgui.c - helpers for the main text gui
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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
#include "topiclabel.h"
#include "../common/text.h"
#include "../common/xchatc.h"
#include <libgnome/gnome-url.h> /* gnome_url_show */
#include <gconf/gconf-client.h>

int check_word(GtkWidget *xtext, char *word);
void clicked_word(GtkWidget *xtext, char *word, GdkEventButton *even, gpointer data);
void font_changed(GConfClient *client, guint cnxn_id, const gchar* key, GConfValue* value, gboolean is_default, gpointer user_data);

void initialize_text_gui() {
	GtkWidget *frame, *scrollbar;
  /* For setting the font from gconf. */
  GConfClient *client;
  gchar *font;

	gui.xtext = GTK_XTEXT(gtk_xtext_new(colors, TRUE));
	frame = glade_xml_get_widget(gui.xml, "text area frame");
	gtk_container_add(GTK_CONTAINER(frame), gui.xtext);
	scrollbar = glade_xml_get_widget(gui.xml, "text area scrollbar");
	gtk_range_set_adjustment(GTK_RANGE(scrollbar), gui.xtext->adj);

	palette_alloc(GTK_WIDGET(gui.xtext));
	gtk_xtext_set_palette(gui.xtext, colors);
	gtk_xtext_set_max_lines(gui.xtext, 3000);
	gtk_xtext_set_show_separator(gui.xtext, TRUE);
	gtk_xtext_set_indent(gui.xtext, TRUE);
	gtk_xtext_set_max_indent(gui.xtext, 500);
	gtk_xtext_set_thin_separator(gui.xtext, TRUE);
	gtk_xtext_set_wordwrap(gui.xtext, TRUE);
	gtk_xtext_set_urlcheck_function(gui.xtext, check_word);
	g_signal_connect(G_OBJECT(gui.xtext), "word_click", G_CALLBACK(clicked_word), NULL);
 
  /* Set the font. */
  client = gconf_client_get_default ();
  gconf_client_add_dir(client, "/apps/xchat/main_window", GCONF_CLIENT_PRELOAD_NONE, NULL);
  gconf_client_notify_add(client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc)font_changed, NULL, NULL, NULL);
  gconf_client_notify_add(client, "/apps/xchat/main_window/font", (GConfClientNotifyFunc)font_changed, NULL, NULL, NULL);


  if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
    font = gconf_client_get_string (client, "/desktop/gnome/interface/font_name", NULL);
  else
    font = gconf_client_get_string(client, "/apps/xchat/main_window/font", NULL);

  gtk_xtext_set_font (GTK_XTEXT (gui.xtext), font);
  g_object_unref (client);
  g_free (font);

	gtk_widget_show_all(GTK_WIDGET(gui.xtext));
}

void text_gui_add_text_buffer(struct session *sess) {
	session_gui *tgui;

	tgui = malloc(sizeof(session_gui));
	tgui->buffer = gtk_xtext_buffer_new(gui.xtext);
	sess->gui = (struct session_gui *) tgui;

	gtk_xtext_buffer_show(gui.xtext, tgui->buffer, TRUE);
	if(preferences_show_timestamp())
		gtk_xtext_set_time_stamp(tgui->buffer, TRUE);
	gui.current_session = sess;

	if(sess->topic == NULL)
		tgui->topic = g_strdup("");
	else
		tgui->topic = g_strdup(sess->topic);
	tgui->entry = g_strdup("");
	tgui->lag_text = NULL;
	tgui->queue_text = NULL;
}

void text_gui_remove_text_buffer(struct session *sess) {
	session_gui *tgui;

	tgui = (session_gui *) sess->gui;
	gtk_xtext_buffer_free(tgui->buffer);
	g_free(tgui->topic);
	g_free(tgui->entry);
	if(tgui->lag_text)
		free(tgui->lag_text);
	if(tgui->queue_text)
		free(tgui->queue_text);
	free(tgui);
	sess->gui = NULL;
}

void text_gui_print_line(xtext_buffer *buf, unsigned char *text, int len, gboolean indent) {
	int leftlen;
	unsigned char *tab;
	if(len == 0)
		len = 1;

	/* FIXME: do timestamp */
	if(!indent) {
		gtk_xtext_append(buf, text, len);
		return;
	}

	tab = strchr(text, '\t');
	if(tab && tab < (text + len)) {
		leftlen = tab - text;
		gtk_xtext_append_indent(buf, text, leftlen, tab + 1, len - (leftlen + 1));
	} else {
		gtk_xtext_append_indent(buf, 0, 0, text, len);
	}
}

void text_gui_print(xtext_buffer *buf, unsigned char *text, gboolean indent) {
	char *last_text = text;
	int len = 0;

	/* split the text into separate lines */
	while(1) {
		switch(*text) {
		case '\0':
			text_gui_print_line(buf, last_text, len, indent);
			return;
		case '\n':
			text_gui_print_line(buf, last_text, len, indent);
			text++;
			if(*text == '\0')
				return;
			last_text = text;
			len = 0;
			break;
		case ATTR_BEEP:
			*text = ' ';
			gdk_beep();
		default:
			text++;
			len++;
		}
	}
}

void set_nickname(struct server *serv, char *newnick) {
	if(serv == gui.current_session->server) {
		GtkWidget *nick = glade_xml_get_widget(gui.xml, "nickname");
		if(newnick == NULL)
			gtk_label_set_text(GTK_LABEL(nick), serv->nick);
		else
			gtk_label_set_text(GTK_LABEL(nick), newnick);
	}
}

void set_gui_topic(session *sess, char *topic) {
	session_gui *tgui = (session_gui *) sess->gui;

	g_free(tgui->topic);
	if(topic == NULL)
		if(sess->topic == NULL)
			tgui->topic = g_strdup("");
		else
			tgui->topic = g_strdup(sess->topic);
	else
		tgui->topic = g_strdup(topic);
	if(sess == gui.current_session) {
		GtkWidget *topicbar;

		topicbar = glade_xml_get_widget(gui.xml, "topic label");
		topic_label_set_text (topicbar, tgui->topic);
	}
}

void clear_buffer(struct session *sess) {
	session_gui *sgui = (session_gui *) sess->gui;
	gtk_xtext_clear(sgui->buffer);
}

int check_word(GtkWidget *xtext, char *word) {
	current_sess = gui.current_session;
	return text_word_check(word);
}

void clicked_word(GtkWidget *xtext, char *word, GdkEventButton *event, gpointer data) {
	if(word == NULL)
		return;

	if(event->button == 1) {
		/* left click */

		switch (check_word (xtext, word))
		{
			case 0:
			  return;
			case WORD_URL:
			case WORD_HOST:
			{
			  /* we handle URLs here to be consistent with left-click behavior
				 * elsewhere in gnome */
		    GError *err = NULL;

				gnome_url_show (word, &err);
				if (err != NULL)
				{
					/* FIXME: should actually check the contents of the error quark, and
					 * should eventually output this error in a better way than stdout
					 */
					char *msg = g_strdup_printf (_("Unable to activate the URL '%s"), word);
					g_print (msg);
					g_free (msg);
					g_error_free (err);
				}
			}
		}
		return;
	}
	if(event->button == 2) {
		/* middle click */
		return;
	}
	switch(check_word(xtext, word)) {
	case 0:
		/* FIXME: show default context menu */
		return;
	case WORD_URL:
	case WORD_HOST:
		/* FIXME: show url context menu */
		g_print("its a url!\n");
		return;
	case WORD_NICK:
		/* FIXME: show nickname context menu */
		g_print("its a nickname!\n");
		return;
	case WORD_CHANNEL:
		/* FIXME: show nickname context menu */
		g_print("its a channel!\n");
		return;
	case WORD_EMAIL:
		/* FIXME: show nickname context menu */
		g_print("its a email address!\n");
		return;
	case WORD_DIALOG:
		/* FIXME: show nickname context menu */
		g_print("its a nickname dialog!\n");
		return;
	}
}
void
font_changed(GConfClient *client, guint cnxn_id, const gchar* key, GConfValue* value, gboolean is_default, gpointer user_data)
{
  gchar *font;

  if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
    font = gconf_client_get_string(client, "/desktop/gnome/interface/font_name", NULL);
  else
    font = gconf_client_get_string(client, "/apps/xchat/main_window/font", NULL);

  gtk_xtext_set_font(GTK_XTEXT(gui.xtext), font);
  gtk_xtext_refresh(GTK_XTEXT(gui.xtext), 0);

  gtk_adjustment_set_value(gui.xtext->adj, gui.xtext->adj->page_size);
  
  g_free(font);
}
