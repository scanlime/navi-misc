/*
 * conversation-panel.c - Widget encapsulating the conversation panel
 *
 * Copyright (C) 2005 xchat-gnome team
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
#include <string.h>
#include <gtk/gtk.h>
#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "userlist-gui.h"
#include "xtext.h"
#include "../common/fe.h"
#include "../common/url.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"

static void conversation_panel_class_init   (ConversationPanelClass *klass);
static void conversation_panel_init         (ConversationPanel      *panel);
static void conversation_panel_finalize     (GObject                *object);
static int  conversation_panel_check_word   (GtkWidget              *xtext,
                                             char                   *word,
                                             int                     len);
static void conversation_panel_clicked_word (GtkWidget              *xtext,
                                             char                   *word,
                                             GdkEventButton         *event,
                                             ConversationPanel      *panel);
static void conversation_panel_open_url     (GtkAction              *action,
                                             gchar                  *url);
static void conversation_panel_copy_text    (GtkAction              *action,
                                             gchar                  *text);
static void conversation_panel_send_email   (GtkAction              *action,
                                             gchar                  *text);
static void drop_send_files_activated       (GtkAction              *action,
                                             gpointer                data);
static void drop_paste_file_activated       (GtkAction              *action,
                                             gpointer                data);
static void drop_paste_filename_activated   (GtkAction              *action,
                                             gpointer                data);
static void drop_cancel_activated           (GtkAction              *action,
                                             gpointer                data);

static GtkHBoxClass *parent_class;

G_DEFINE_TYPE (ConversationPanel, conversation_panel, GTK_TYPE_HBOX);

struct _ConversationPanelPriv
{
	GtkWidget *xtext;
	GtkWidget *frame;
	GtkWidget *scrollbar;
};

static GtkActionEntry url_popup_entries[] = {
	{ "TextURLOpen", GTK_STOCK_OPEN, N_("_Open Link in Browser"), NULL, NULL, G_CALLBACK (conversation_panel_open_url)  },
	{ "TextURLCopy", GTK_STOCK_COPY, N_("_Copy Link Location"),   NULL, NULL, G_CALLBACK (conversation_panel_copy_text) },
};

static GtkActionEntry email_popup_entries[] = {
	{ "TextEmailSend", GNOME_STOCK_MAIL, N_("Se_nd Message To..."), NULL, NULL, G_CALLBACK (conversation_panel_send_email) },
	{ "TextEmailCopy", GTK_STOCK_COPY,   N_("_Copy Address"),       NULL, NULL, G_CALLBACK (conversation_panel_copy_text)  },
};

static GtkActionEntry dnd_popup_entries[] = {
	{ "DropSendFiles",     NULL, N_("_Send File"),           NULL, NULL, G_CALLBACK (drop_send_files_activated)     },
	{ "DropPasteFile",     NULL, N_("Paste File _Contents"), NULL, NULL, G_CALLBACK (drop_paste_file_activated)     },
	{ "DropPasteFileName", NULL, N_("Paste File_name"),      NULL, NULL, G_CALLBACK (drop_paste_filename_activated) },
	{ "DropCancel",        NULL, N_("_Cancel"),              NULL, NULL, G_CALLBACK (drop_cancel_activated)         },
};

static GtkActionGroup *text_popups = NULL;

static void
conversation_panel_class_init (ConversationPanelClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = conversation_panel_finalize;
}

static void
conversation_panel_init (ConversationPanel *panel)
{
	panel->priv = g_new0 (ConversationPanelPriv, 1);
	panel->priv->xtext     = gtk_xtext_new (colors, TRUE);
	panel->priv->frame     = gtk_frame_new (NULL);
	panel->priv->scrollbar = gtk_vscrollbar_new (GTK_XTEXT (panel->priv->xtext)->adj);

	gtk_widget_show (panel->priv->xtext);
	gtk_widget_show (panel->priv->frame);
	gtk_widget_show (panel->priv->scrollbar);

	gtk_container_add (GTK_CONTAINER (panel->priv->frame), panel->priv->xtext);
	gtk_box_pack_start (GTK_BOX (panel), panel->priv->frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (panel), panel->priv->scrollbar, TRUE, FALSE, 0);
	gtk_box_set_spacing (GTK_BOX (panel), 3);

	palette_alloc (panel->priv->xtext);
	gtk_xtext_set_palette           (GTK_XTEXT (panel->priv->xtext), colors);
	gtk_xtext_set_max_lines         (GTK_XTEXT (panel->priv->xtext), 3000);
	gtk_xtext_set_show_separator    (GTK_XTEXT (panel->priv->xtext), prefs.show_separator);
	gtk_xtext_set_indent            (GTK_XTEXT (panel->priv->xtext), prefs.indent_nicks);
	gtk_xtext_set_max_indent        (GTK_XTEXT (panel->priv->xtext), prefs.max_auto_indent);
	gtk_xtext_set_thin_separator    (GTK_XTEXT (panel->priv->xtext), prefs.thin_separator);
	gtk_xtext_set_wordwrap          (GTK_XTEXT (panel->priv->xtext), prefs.wordwrap);
	gtk_xtext_set_urlcheck_function (GTK_XTEXT (panel->priv->xtext), conversation_panel_check_word);

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click", G_CALLBACK (conversation_panel_clicked_word), NULL);

	if (text_popups == NULL) {
		text_popups = gtk_action_group_new ("TextPopups");
		gtk_action_group_set_translation_domain (text_popups, GETTEXT_PACKAGE);
		gtk_action_group_add_actions (text_popups, url_popup_entries,   G_N_ELEMENTS (url_popup_entries),   NULL);
		gtk_action_group_add_actions (text_popups, email_popup_entries, G_N_ELEMENTS (email_popup_entries), NULL);
		gtk_action_group_add_actions (text_popups, dnd_popup_entries,   G_N_ELEMENTS (dnd_popup_entries),   NULL);
		gtk_ui_manager_insert_action_group (gui.manager, text_popups, 0);
	}
}

static void
conversation_panel_finalize (GObject *object)
{
	ConversationPanel *panel;

	panel = CONVERSATION_PANEL (object);

	if (panel->priv)
		g_free (panel->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static int
conversation_panel_check_word (GtkWidget *xtext, char *word, int len)
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
conversation_panel_clicked_word (GtkWidget *xtext, char *word, GdkEventButton *event, ConversationPanel *panel)
{
	if (word == NULL)
		return;

	if (event->button == 1) {
		int type = conversation_panel_check_word (xtext, word, strlen (word));

		if (type == WORD_URL || type == WORD_HOST) {
			fe_open_url (word);
		}
		return;
	}

	if (event->button == 3) {
		GtkWidget *menu;

		switch (conversation_panel_check_word (xtext, word, strlen (word))) {
		case WORD_URL:
		case WORD_HOST:
			menu = gtk_ui_manager_get_widget (gui.manager, "/TextURLPopup");

			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, g_strdup (word), 3, gtk_get_current_event_time ());

			return;
		case WORD_NICK:
		{
			struct User *user;

			menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");

			user = userlist_find (gui.current_session, word);
			if (user) {
				current_user = user;
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, g_strdup (word), 3, gtk_get_current_event_time ());
			}
			return;
		}
		case WORD_CHANNEL:
			 /* FIXME: show channel context menu */
			return;
		case WORD_EMAIL:
			menu = gtk_ui_manager_get_widget (gui.manager, "/TextEmailPopup");

			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, g_strdup (word), 3, gtk_get_current_event_time ());

			return;
		case WORD_DIALOG:
			/* FIXME: show dialog(?) context menu */
			return;
		}
	}
}

static void
conversation_panel_open_url (GtkAction *action, gchar *url)
{
	fe_open_url (url);
	g_free (url);
}

static void
conversation_panel_copy_text (GtkAction *action, gchar *text)
{
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text (clipboard, text, strlen (text));
	g_object_unref (clipboard);
	g_free (text);
}

static void
conversation_panel_send_email (GtkAction *action, gchar *text)
{
	g_free (text);
}

static void
drop_send_files_activated (GtkAction *action, gpointer data)
{
}

static void
drop_paste_file_activated (GtkAction *action, gpointer data)
{
}

static void
drop_paste_filename_activated (GtkAction *action, gpointer data)
{
}

static void
drop_cancel_activated (GtkAction *action, gpointer data)
{
}
