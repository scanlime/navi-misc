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
                                               gpointer                data);
static void drop_paste_file_activated         (GtkAction              *action,
                                               gpointer                data);
static void drop_paste_filename_activated     (GtkAction              *action,
                                               gpointer                data);
static void drop_cancel_activated             (GtkAction              *action,
                                               gpointer                data);
static void gconf_font_changed                (GConfClient            *client,
                                               guint                   cnxn_id,
                                               GConfEntry             *entry,
                                               ConversationPanel      *panel);
static void gconf_background_changed          (GConfClient            *client,
                                               guint                   cnxn_id,
                                               GConfEntry             *entry,
                                               ConversationPanel      *panel);

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

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click", G_CALLBACK (conversation_panel_clicked_word), NULL);
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

GtkWidget *conversation_panel_new (void)
{
	return GTK_WIDGET (g_object_new (conversation_panel_get_type (), NULL));
}
