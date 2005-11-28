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
#include <gconf/gconf-client.h>
#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "userlist-gui.h"
#include "xtext.h"
#include "../common/fe.h"
#include "../common/outbound.h"
#include "../common/url.h"
#include "../common/xchat.h"
#include "../common/xchatc.h"

static void conversation_panel_class_init     (ConversationPanelClass *klass);
static void conversation_panel_init           (ConversationPanel      *panel);
static void conversation_panel_finalize       (GObject                *object);
static void conversation_panel_realize        (GtkWidget              *widget);
static int  conversation_panel_check_word     (GtkWidget              *xtext,
                                               char                   *word,
                                               int                     len);
static void conversation_panel_clicked_word   (GtkWidget              *xtext,
                                               char                   *word,
                                               GdkEventButton         *event,
                                               ConversationPanel      *panel);
static void conversation_panel_open_url       (GtkAction              *action,
                                               gchar                  *url);
static void conversation_panel_copy_text      (GtkAction              *action,
                                               gchar                  *text);
static void conversation_panel_send_email     (GtkAction              *action,
                                               gchar                  *text);
static void conversation_panel_set_font       (ConversationPanel      *panel);
static void conversation_panel_set_background (ConversationPanel      *panel);
static void drop_send_files_activated         (GtkAction              *action,
                                               GSList                 *files);
static void drop_paste_file_activated         (GtkAction              *action,
                                               GSList                 *files);
static void drop_paste_filename_activated     (GtkAction              *action,
                                               GSList                 *files);
static void drop_cancel_activated             (GtkAction              *action,
                                               GSList                 *files);
static void drag_data_received                 (GtkWidget             *widget,
                                                GdkDragContext        *context,
                                                gint                   x,
                                                gint                   y,
                                                GtkSelectionData      *selection_data,
                                                guint                  info,
                                                guint                  time,
                                                gpointer               data);
static void gconf_font_changed                (GConfClient            *client,
                                               guint                   cnxn_id,
                                               GConfEntry             *entry,
                                               ConversationPanel      *panel);
static void gconf_background_changed          (GConfClient            *client,
                                               guint                   cnxn_id,
                                               GConfEntry             *entry,
                                               ConversationPanel      *panel);
static void free_text                         (GtkMenuShell           *menu,
                                               gchar                  *text);

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

static void
conversation_panel_class_init (ConversationPanelClass *klass)
{
	GObjectClass   *gobject_class;
	GtkWidgetClass *widget_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = conversation_panel_finalize;

	widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->realize = conversation_panel_realize;
}

static void
conversation_panel_init (ConversationPanel *panel)
{
	GConfClient *client;

	client = gconf_client_get_default ();

	panel->priv = g_new0 (ConversationPanelPriv, 1);
	panel->priv->xtext     = gtk_xtext_new (colors, TRUE);
	panel->priv->frame     = gtk_frame_new (NULL);
	panel->priv->scrollbar = gtk_vscrollbar_new (GTK_XTEXT (panel->priv->xtext)->adj);

	gtk_box_pack_start (GTK_BOX (panel), panel->priv->frame,     TRUE,  TRUE, 0);
	gtk_box_pack_start (GTK_BOX (panel), panel->priv->scrollbar, FALSE, TRUE, 0);
	gtk_box_set_spacing (GTK_BOX (panel), 3);
	gtk_container_add (GTK_CONTAINER (panel->priv->frame), panel->priv->xtext);
	gtk_frame_set_shadow_type (GTK_FRAME (panel->priv->frame), GTK_SHADOW_ETCHED_IN);

	gtk_widget_show (panel->priv->xtext);
	gtk_widget_show (panel->priv->frame);
	gtk_widget_show (panel->priv->scrollbar);

	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts",
	                         (GConfClientNotifyFunc) gconf_font_changed,       NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/font",
	                         (GConfClientNotifyFunc) gconf_font_changed,       NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_type",
	                         (GConfClientNotifyFunc) gconf_background_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_image",
	                         (GConfClientNotifyFunc) gconf_background_changed, NULL, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_transparency",
	                         (GConfClientNotifyFunc) gconf_background_changed, NULL, NULL, NULL);

	if (text_popups == NULL) {
		text_popups = gtk_action_group_new ("TextPopups");
		gtk_action_group_set_translation_domain (text_popups, GETTEXT_PACKAGE);
		gtk_action_group_add_actions (text_popups, url_popup_entries,   G_N_ELEMENTS (url_popup_entries),   NULL);
		gtk_action_group_add_actions (text_popups, email_popup_entries, G_N_ELEMENTS (email_popup_entries), NULL);
		gtk_action_group_add_actions (text_popups, dnd_popup_entries,   G_N_ELEMENTS (dnd_popup_entries),   NULL);
		gtk_ui_manager_insert_action_group (gui.manager, text_popups, 0);
	}

	g_object_unref (client);

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click",         G_CALLBACK (conversation_panel_clicked_word), NULL);
	g_signal_connect (G_OBJECT (panel->priv->xtext), "drag_data_received", G_CALLBACK (drag_data_received), NULL);
	gtk_drag_dest_set (GTK_WIDGET (panel->priv->xtext),
	                   GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
	                   target_table, G_N_ELEMENTS (target_table), GDK_ACTION_COPY | GDK_ACTION_ASK);
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

static void
conversation_panel_realize (GtkWidget *widget)
{
	ConversationPanel *panel;

	panel = CONVERSATION_PANEL (widget);

	if (GTK_WIDGET_CLASS (parent_class)->realize)
		GTK_WIDGET_CLASS (parent_class)->realize (widget);

	palette_alloc (panel->priv->xtext);
	gtk_xtext_set_palette           (GTK_XTEXT (panel->priv->xtext), colors);
	gtk_xtext_set_max_lines         (GTK_XTEXT (panel->priv->xtext), 3000);
	gtk_xtext_set_show_separator    (GTK_XTEXT (panel->priv->xtext), prefs.show_separator);
	gtk_xtext_set_indent            (GTK_XTEXT (panel->priv->xtext), prefs.indent_nicks);
	gtk_xtext_set_max_indent        (GTK_XTEXT (panel->priv->xtext), prefs.max_auto_indent);
	gtk_xtext_set_thin_separator    (GTK_XTEXT (panel->priv->xtext), prefs.thin_separator);
	gtk_xtext_set_wordwrap          (GTK_XTEXT (panel->priv->xtext), prefs.wordwrap);
	gtk_xtext_set_urlcheck_function (GTK_XTEXT (panel->priv->xtext), conversation_panel_check_word);

	conversation_panel_set_font (panel);
	conversation_panel_set_background (panel);
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
		{
			gchar *cword = g_strdup (word);
			menu = gtk_ui_manager_get_widget (gui.manager, "/TextURLPopup");
			g_signal_connect (G_OBJECT (menu), "deactivate", G_CALLBACK (free_text), cword);
			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, cword, 3, gtk_get_current_event_time ());

			return;
		}
		case WORD_NICK:
		{
			struct User *user;
			menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");

			user = userlist_find (gui.current_session, word);
			if (user) {
				gchar *cword = g_strdup (word);
				current_user = user;
				g_signal_connect (G_OBJECT (menu), "deactivate", G_CALLBACK (free_text), cword);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, word, 3, gtk_get_current_event_time ());
			}
			return;
		}
		case WORD_CHANNEL:
			 /* FIXME: show channel context menu */
			return;
		case WORD_EMAIL:
		{
			gchar *cword = g_strdup (word);
			menu = gtk_ui_manager_get_widget (gui.manager, "/TextEmailPopup");
			g_signal_connect (G_OBJECT (menu), "deactivate", G_CALLBACK (free_text), cword);
			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, word, 3, gtk_get_current_event_time ());

			return;
		}
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
}

static void
conversation_panel_copy_text (GtkAction *action, gchar *text)
{
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text (clipboard, text, strlen (text));
	g_object_unref (clipboard);
}

static void
conversation_panel_send_email (GtkAction *action, gchar *text)
{
}

static void
conversation_panel_set_font (ConversationPanel *panel)
{
	GConfClient *client;
	gchar *font;

	client = gconf_client_get_default ();
	if (gconf_client_get_bool (client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);
	if (font == NULL)
		font = g_strdup ("fixed 11");
	g_object_unref (client);

	gtk_xtext_set_font (GTK_XTEXT (panel->priv->xtext), font);
	gtk_adjustment_set_value (GTK_XTEXT (panel->priv->xtext)->adj,
	                          GTK_XTEXT (panel->priv->xtext)->adj->upper -
	                          GTK_XTEXT (panel->priv->xtext)->adj->page_size);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), FALSE);
	g_free (font);
}

static void
conversation_panel_set_background (ConversationPanel *panel)
{
	GConfClient *client;
	guint background_type;

	client = gconf_client_get_default ();

	background_type = gconf_client_get_int (client, "/apps/xchat/main_window/background_type", NULL);
	if (background_type == 0) {
		/* solid color */
		gtk_xtext_set_tint       (GTK_XTEXT (panel->priv->xtext), 0, 0, 0);
		gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, FALSE);
	} else if (background_type == 1) {
		/* image */
		gchar *filename;
		GdkPixbuf *pixbuf;

		filename = gconf_client_get_string (client, "/apps/xchat/main_window/background_image", NULL);
		gtk_xtext_set_tint (GTK_XTEXT (panel->priv->xtext), 0, 0, 0);

		if (filename && (pixbuf = gdk_pixbuf_new_from_file (filename, NULL))) {
			gint width, height;
			GdkPixmap *image;

			width  = gdk_pixbuf_get_width  (pixbuf);
			height = gdk_pixbuf_get_height (pixbuf);

			image = gdk_pixmap_new (NULL, width, height, 24);
			gdk_draw_pixbuf (image, NULL, pixbuf, 0, 0, 0, 0, width, height, GDK_RGB_DITHER_NONE, 0, 0);

			gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), image, FALSE);

			g_object_unref (pixbuf);
			g_object_unref (image);
		} else {
			gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, FALSE);
		}
	} else {
		/* tinted */
		float transparency = gconf_client_get_float (client, "/apps/xchat/main_window/background_transparency", NULL);
		int value = 255 - ((int) (transparency * 255));

		gtk_xtext_set_tint       (GTK_XTEXT (panel->priv->xtext), value, value, value);
		gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, TRUE);
	}
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), TRUE);

	g_object_unref (client);
}

static void
drop_send_files_activated (GtkAction *action, GSList *files)
{
	/* Should be dropped in a query */
	g_return_if_fail (gui.current_session->type == 3);
	drop_send_files ();
}

static void
drop_paste_file_activated (GtkAction *action, GSList *files)
{
}

static void
drop_paste_filename_activated (GtkAction *action, GSList *files)
{
}

static void
drop_cancel_activated (GtkAction *action, GSList *files)
{
}

static void
drag_data_received (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *selection_data, guint info, guint time, gpointer data)
{
	GSList *dropped_files = NULL;

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

			if ((gui.current_session->type != SESS_CHANNEL) &&
			    (gui.current_session->type != SESS_DIALOG))
				return;

			if (selection_data->format != 8 || selection_data->length == 0) {
				g_printerr (_("URI list dropped on xchat-gnome had wrong format (%d) or length (%d)\n"),
				            selection_data->format, selection_data->length);
				return;
			}

			uri_list = g_strndup (selection_data->data, selection_data->length);
			uris = g_strsplit (uri_list, "\r\n", 0);
			g_free (uri_list);

			for (nb_uri = 0; uris[nb_uri] && strlen (uris[nb_uri]) > 0; nb_uri++)
				dropped_files = g_slist_prepend (dropped_files, uris[nb_uri]);
			g_free (uris);
			dropped_files = g_slist_reverse (dropped_files);

			if (context->action == GDK_ACTION_ASK) {
				/* Display the context menu */
				GtkWidget *menu, *entry;

				menu = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup");

				/* Enable/Disable paste file content */
				entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropPasteFile");

				if ((nb_uri > 1) ||
				    (uri_is_text (dropped_files->data) == FALSE) ||
				    (check_file_size (dropped_files->data) == FALSE))
					gtk_widget_set_sensitive (entry, FALSE);
				else
					gtk_widget_set_sensitive (entry, TRUE);

				/* Enable/Disable send files */
				entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropSendFiles");
				gtk_widget_set_sensitive (entry, (gui.current_session->type != SESS_CHANNEL));

				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, dropped_files, 2, gtk_get_current_event_time ());
			} else {
				/* Do the default action */
				if (gui.current_session->type == SESS_CHANNEL) {
					/* Dropped in a channel */
					if (nb_uri == 1 && uri_is_text (dropped_files->data) && check_file_size (dropped_files->data))
						drop_paste_file ();
				} else {
					/* Dropped in a query */
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
gconf_font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, ConversationPanel *panel)
{
	conversation_panel_set_font (panel);
}

static void
gconf_background_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, ConversationPanel *panel)
{
	conversation_panel_set_background (panel);
}

static void
free_text (GtkMenuShell *menu, gchar *text)
{
	g_signal_handlers_disconnect_matched (menu, G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
	                                      "deactivate", NULL, NULL, free_text, text);
	g_free (text);
}

GtkWidget *conversation_panel_new (void)
{
	return GTK_WIDGET (g_object_new (conversation_panel_get_type (), NULL));
}
