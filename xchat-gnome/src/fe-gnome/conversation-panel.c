/*
 * conversation-panel.c - Widget encapsulating the conversation panel
 *
 * Copyright (C) 2004-2006 xchat-gnome team
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
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <libgnomeui/gnome-stock-icons.h>
#include <libgnomevfs/gnome-vfs.h>

#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "userlist-gui.h"
#include "util.h"
#include "xtext.h"

#include "../common/fe.h"
#include "../common/outbound.h"
#include "../common/text.h"
#include "../common/url.h"
#include "../common/userlist.h"
#include "../common/util.h"
#include "../common/xchatc.h"

typedef struct _fe_lastlog_info fe_lastlog_info;

static void     conversation_panel_class_init         (ConversationPanelClass *klass);
static void     conversation_panel_init               (ConversationPanel      *panel);
static void     conversation_panel_finalize           (GObject                *object);
static void     conversation_panel_realize            (GtkWidget              *widget);
static int      conversation_panel_check_word         (GtkWidget              *xtext,
                                                       char                   *word,
                                                       int                     len);
static void     conversation_panel_clicked_word       (GtkWidget              *xtext,
                                                       char                   *word,
                                                       GdkEventButton         *event,
                                                       ConversationPanel      *panel);
static void     conversation_panel_set_font           (ConversationPanel      *panel);
static void     conversation_panel_font_changed       (GConfClient            *client,
                                                       guint                   cnxn_id,
                                                       GConfEntry             *entry,
                                                       ConversationPanel      *panel);
static void     conversation_panel_set_background     (ConversationPanel      *panel);
static void     conversation_panel_background_changed (GConfClient            *client,
                                                       guint                   cnxn_id,
                                                       GConfEntry             *entry,
                                                       ConversationPanel      *panel);
static void     timestamps_changed                    (GConfClient            *client,
                                                       guint                   cnxn_id,
                                                       GConfEntry             *entry,
                                                       xtext_buffer           *buffer);
static void     conversation_panel_print_line         (xtext_buffer           *buffer,
                                                       guchar                  *text,
                                                       int                     len,
                                                       gboolean                indent);
static void     conversation_panel_lastlog_foreach    (GtkXText               *xtext,
                                                       guchar                 *text,
                                                       fe_lastlog_info        *info);

static void     open_url                              (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     copy_text                             (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     send_email                            (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     drop_send_files                       (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     drop_paste_file                       (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     drop_paste_filename                   (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     drop_cancel                           (GtkAction              *action,
                                                       ConversationPanel      *panel);
static void     drag_data_received                    (GtkWidget              *widget,
                                                       GdkDragContext         *context,
                                                       gint                    x,
                                                       gint                    y,
                                                       GtkSelectionData       *selection_data,
                                                       guint                   info,
                                                       guint                   time,
                                                       ConversationPanel      *panel);
static void     free_dropped_files                    (ConversationPanel      *panel);
static void     drop_send                             (ConversationPanel      *panel);
static void     drop_paste                            (ConversationPanel      *panel);
static void     send_file                             (gpointer                file,
                                                       gpointer                user_data);
static void     on_default_copy_activate              (GtkAction              *action,
                                                       ConversationPanel      *panel);
static gboolean uri_is_text                           (gchar *uri);
static gboolean check_file_size                       (gchar *uri);

struct _fe_lastlog_info
{
	ConversationPanel *panel;
	struct session    *sess;
	guchar            *sstr;
};

struct _ConversationPanelPriv
{
	GtkWidget      *scrollbar;
	GtkWidget      *xtext;

	struct session *current;

	GHashTable     *buffers;
	GHashTable     *timestamp_notifies;

	gchar          *selected_word;
	GSList         *dropped_files;
};

static GtkActionEntry url_actions[] = {
	{ "TextURLOpen", GTK_STOCK_OPEN, N_("_Open Link in Browser"), NULL, NULL, G_CALLBACK (open_url) },
        { "TextURLCopy", GTK_STOCK_COPY, N_("_Copy Link Location"),   NULL, NULL, G_CALLBACK (copy_text) },
};

static GtkActionEntry email_actions[] = {
	{ "TextEmailSend", GNOME_STOCK_MAIL, N_("Se_nd Message To..."), NULL, NULL, G_CALLBACK (send_email) },
        { "TextEmailCopy", GTK_STOCK_COPY,   N_("_Copy Address"),       NULL, NULL, G_CALLBACK (copy_text) },
};

static GtkActionEntry dnd_actions[] = {
	{ "DropSendFiles",     NULL, N_("_Send File"),           NULL, NULL, G_CALLBACK (drop_send_files) },
	{ "DropPasteFile",     NULL, N_("Paste File _Contents"), NULL, NULL, G_CALLBACK (drop_paste_file) },
	{ "DropPasteFileName", NULL, N_("Paste File_name"),      NULL, NULL, G_CALLBACK (drop_paste_filename) },
	{ "DropCancel",        NULL, N_("_Cancel"),              NULL, NULL, G_CALLBACK (drop_cancel) },
};

static GtkActionEntry default_actions[] = {
	{ "DefaultCopy",     GTK_STOCK_COPY, N_("_Copy"),        NULL, NULL, G_CALLBACK (on_default_copy_activate) },
};

#define DROP_FILE_PASTE_MAX_SIZE 1024
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

static struct User dialog_user;

static GtkHBoxClass *parent_class;
G_DEFINE_TYPE (ConversationPanel, conversation_panel, GTK_TYPE_HBOX);

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
	GtkWidget *frame;
	GtkActionGroup *action_group;

	panel->priv = g_new0 (ConversationPanelPriv, 1);
	panel->priv->xtext     = gtk_xtext_new (colors, TRUE);
	panel->priv->scrollbar = gtk_vscrollbar_new (GTK_XTEXT (panel->priv->xtext)->adj);
	frame                  = gtk_frame_new (NULL);

	panel->priv->buffers            = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, (GDestroyNotify) gtk_xtext_buffer_free);
	panel->priv->timestamp_notifies = g_hash_table_new      (g_direct_hash, g_direct_equal);

	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
	gtk_container_add (GTK_CONTAINER (frame), panel->priv->xtext);

	gtk_box_set_spacing (GTK_BOX (panel), 6);
	gtk_box_pack_start  (GTK_BOX (panel), frame,                  TRUE,  TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (panel), panel->priv->scrollbar, FALSE, TRUE, 0);

	gtk_widget_show (panel->priv->xtext);
	gtk_widget_show (panel->priv->scrollbar);
	gtk_widget_show (frame);
	gtk_widget_show (GTK_WIDGET (panel));

	action_group = gtk_action_group_new ("TextPopups");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions (action_group, url_actions,   G_N_ELEMENTS (url_actions),   panel);
	gtk_action_group_add_actions (action_group, email_actions, G_N_ELEMENTS (email_actions), panel);
	gtk_action_group_add_actions (action_group, dnd_actions,   G_N_ELEMENTS (dnd_actions),   panel);
	gtk_action_group_add_actions (action_group, default_actions, G_N_ELEMENTS (default_actions),   panel);
	gtk_ui_manager_insert_action_group (gui.manager, action_group, 0);
	g_object_unref (action_group);

	g_signal_connect (G_OBJECT (panel->priv->xtext), "drag_data_received", G_CALLBACK (drag_data_received), panel);
	gtk_drag_dest_set (panel->priv->xtext, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
	                   target_table, G_N_ELEMENTS (target_table), GDK_ACTION_COPY | GDK_ACTION_ASK);
}

static void
conversation_panel_finalize (GObject *object)
{
	ConversationPanel *panel;

	panel = CONVERSATION_PANEL (object);

	g_hash_table_destroy (panel->priv->buffers);
	g_hash_table_destroy (panel->priv->timestamp_notifies);
	g_free (panel->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
conversation_panel_realize (GtkWidget *widget)
{
	ConversationPanel *panel;
	GConfClient       *client;

	if (GTK_WIDGET_CLASS (parent_class)->realize)
		GTK_WIDGET_CLASS (parent_class)->realize (widget);

	panel  = CONVERSATION_PANEL (widget);
	client = gconf_client_get_default ();

	palette_alloc (panel->priv->xtext);
	gtk_xtext_set_palette           (GTK_XTEXT (panel->priv->xtext), colors);
	gtk_xtext_set_max_lines         (GTK_XTEXT (panel->priv->xtext), 3000);
	gtk_xtext_set_show_separator    (GTK_XTEXT (panel->priv->xtext), prefs.show_separator);
	gtk_xtext_set_indent            (GTK_XTEXT (panel->priv->xtext), prefs.indent_nicks);
	gtk_xtext_set_max_indent        (GTK_XTEXT (panel->priv->xtext), prefs.max_auto_indent);
	gtk_xtext_set_thin_separator    (GTK_XTEXT (panel->priv->xtext), prefs.thin_separator);
	gtk_xtext_set_wordwrap          (GTK_XTEXT (panel->priv->xtext), prefs.wordwrap);
	gtk_xtext_set_urlcheck_function (GTK_XTEXT (panel->priv->xtext), conversation_panel_check_word);

	conversation_panel_set_font       (panel);
	conversation_panel_set_background (panel);

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click", G_CALLBACK (conversation_panel_clicked_word), panel);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts",
	                         (GConfClientNotifyFunc) conversation_panel_font_changed,       panel, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/font",
	                         (GConfClientNotifyFunc) conversation_panel_font_changed,       panel, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_type",
	                         (GConfClientNotifyFunc) conversation_panel_background_changed, panel, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_image",
	                         (GConfClientNotifyFunc) conversation_panel_background_changed, panel, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/background_transparency",
	                         (GConfClientNotifyFunc) conversation_panel_background_changed, panel, NULL, NULL);

	g_object_unref (client);
}

static int
conversation_panel_check_word (GtkWidget *xtext, char *word, int len)
{
	int url;
        current_sess = gui.current_session;

	url = url_check_word (word, len);
	if (url == 0) {
		if (current_sess->type == SESS_DIALOG) {
			if (strcmp (word, current_sess->channel) == 0)
				return WORD_NICK;
			return WORD_DIALOG;
		} else if (((word[0]=='@' || word[0]=='+') && userlist_find (current_sess, word+1)) ||
			   userlist_find (current_sess, word)) {
			if (strcmp (word, current_sess->server->nick) != 0)
				return WORD_NICK;
		}
	}
	return url;
}

static void
conversation_panel_clicked_word (GtkWidget *xtext, char *word, GdkEventButton *event, ConversationPanel *panel)
{
	if (word == NULL)
		return;

	if (event->button == 1) {
		switch (conversation_panel_check_word (xtext, word, strlen (word))) {
		case WORD_URL:
		case WORD_HOST:
			{
				char *command;
				command = g_strdup_printf ("URL %s", word);
				handle_command (gui.current_session, command, 1);
				g_free (command);
			}
			break;
		case WORD_NICK:
			{
				char *command;
				struct session *sess;

				if ((gui.current_session->type == SESS_DIALOG) &&
				    (strcmp (gui.current_session->channel, word) == 0))
					break;

				sess = find_dialog (gui.current_session->server, word);
				if (sess)
					navigation_tree_select_session (gui.server_tree, sess);
				else {
					command = g_strdup_printf ("QUERY %s", word);
					handle_command (gui.current_session, command, 1);
					g_free (command);
				}
			}
			break;
		case WORD_CHANNEL:
			{
				char *command;
				struct session *sess;

				sess = find_channel (gui.current_session->server, word);
				if (sess)
					navigation_tree_select_session (gui.server_tree, sess);
				/* Having the channel opened doesn't mean that we're still on it
				 * (if channel was left) */
				command = g_strdup_printf ("JOIN %s", word);
				handle_command (gui.current_session, command, 1);
				g_free (command);
			}
			break;
		}
	}
	if (event->button == 3) {
		switch (conversation_panel_check_word (xtext, word, strlen (word))) {
		case 0:
			{
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/DefaultPopup");
				/* FIXME: we should not display the copy action if no text is selected */
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
			}
			break;
		case WORD_URL:
		case WORD_HOST:
			{
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/TextURLPopup");
				if (panel->priv->selected_word)
					g_free (panel->priv->selected_word);
				panel->priv->selected_word = g_strdup (word);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
			}
			break;
		case WORD_NICK:
			{
				struct User *user;
				GtkWidget   *menu;

				if (panel->priv->selected_word)
					g_free (panel->priv->selected_word);
				panel->priv->selected_word = g_strdup (word);

				if (gui.current_session->type == SESS_CHANNEL) {
					menu = gtk_ui_manager_get_widget (gui.manager, "/UserlistPopup");
					user = userlist_find (gui.current_session, word);
					if (user) {
						current_user = user;
						gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
					}
				} else if (gui.current_session->type == SESS_DIALOG) {
					menu = gtk_ui_manager_get_widget (gui.manager, "/UserDialogPopup");
					strcpy (dialog_user.nick, word);
					dialog_user.nick[strlen (word)] = '\0';
					current_user = &dialog_user;
					gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
				}
			}
			break;
		case WORD_CHANNEL:
			/* FIXME: show channel context menu */
			break;
		case WORD_EMAIL:
			{
				GtkWidget *menu;
				menu = gtk_ui_manager_get_widget (gui.manager, "/TextEmailPopup");
				if (panel->priv->selected_word)
					g_free (panel->priv->selected_word);
				panel->priv->selected_word = g_strdup (word);
				gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time ());
			}
			break;
		case WORD_DIALOG:
			/* FIXME: show dialog context menu */
			break;
		}
	}
}

static void
conversation_panel_set_font (ConversationPanel *panel)
{
	GConfClient *client;
	gchar       *font = NULL;

	client = gconf_client_get_default ();
	if (!gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);

	/* Either use_sys_fonts==TRUE, or there is no current font preference.
	 * In both cases we try the GNOME monospace font. */
	if (font == NULL)
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);

	g_object_unref (client);

	if (font == NULL)
		font = g_strdup ("fixed 11");

	gtk_xtext_set_font (GTK_XTEXT (panel->priv->xtext), font);

	g_free (font);
}

static void
conversation_panel_font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, ConversationPanel *panel)
{
	GtkAdjustment *adj;

	conversation_panel_set_font (panel);

	adj = GTK_XTEXT (panel->priv->xtext)->adj;
	gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), FALSE);
}

static void
conversation_panel_set_background (ConversationPanel *panel)
{
	GConfClient *client;
	gint         background_type;

	client = gconf_client_get_default ();
	background_type = gconf_client_get_int (client, "/apps/xchat/main_window/background_type", NULL);
	if (background_type == 0) {
		gtk_xtext_set_tint       (GTK_XTEXT (panel->priv->xtext), 0, 0, 0);
		gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, FALSE);
	} else if (background_type == 1) {
		gchar *filename = gconf_client_get_string (client, "/apps/xchat/main_window/background_image", NULL);
		gtk_xtext_set_tint       (GTK_XTEXT (panel->priv->xtext), 0, 0, 0);
		gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, FALSE);
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

				gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), image, FALSE);
				g_object_unref (pixbuf);
				g_object_unref (image);
			}
			g_free (filename);
		}
	} else {
		float transparency = gconf_client_get_float (client, "/apps/xchat/main_window/background_transparency", NULL);
		int value = 255 - ((int) (transparency * 255));
		gtk_xtext_set_tint       (GTK_XTEXT (panel->priv->xtext), value, value, value);
		gtk_xtext_set_background (GTK_XTEXT (panel->priv->xtext), NULL, TRUE);
	}
	g_object_unref (client);
}

static void
conversation_panel_background_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, ConversationPanel *panel)
{
	conversation_panel_set_background (panel);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), TRUE);
}

static void
timestamps_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, xtext_buffer *buffer)
{
	gtk_xtext_set_time_stamp (buffer, gconf_client_get_bool (client, entry->key, NULL));
}

static void
open_url (GtkAction *action, ConversationPanel *panel)
{
	char *command;
	command = g_strdup_printf ("URL %s", panel->priv->selected_word);
	handle_command (gui.current_session, command, 1);
	g_free (command);
}

static void
copy_text (GtkAction *action, ConversationPanel *panel)
{
	GtkClipboard *clipboard = gtk_widget_get_clipboard (GTK_WIDGET (panel), GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_set_text (clipboard, panel->priv->selected_word, -1);
}

static void
send_email (GtkAction *action, ConversationPanel *panel)
{
}

static void
drop_send_files  (GtkAction *action, ConversationPanel *panel)
{
	if (panel->priv->current->type != SESS_DIALOG)
		return;
	drop_send (panel);
}

static void
drop_paste_file  (GtkAction *action, ConversationPanel *panel)
{
	drop_paste (panel);
}

static void
drop_paste_filename  (GtkAction *action, ConversationPanel *panel)
{
	gchar *txt = NULL, *path, *tmp;
	GSList *l;

	g_return_if_fail (panel->priv->dropped_files != NULL);

	for (l = panel->priv->dropped_files; l != NULL; l = g_slist_next (l)) {
		path = g_filename_from_uri (l->data, NULL, NULL);
		if (path == NULL)
			path = g_strdup (l->data);

		if (txt == NULL) {
			txt = g_strdup (path);
		} else {
			tmp = txt;
			txt = g_strdup_printf ("%s %s", tmp, path);

			g_free (tmp);
		}

		g_free (path);
	}

	if (panel->priv->current != NULL)
		handle_multiline (panel->priv->current, txt, TRUE, FALSE);

	g_free (txt);
	free_dropped_files (panel);
}

static void
drop_cancel  (GtkAction *action, ConversationPanel *panel)
{
	free_dropped_files (panel);
}

static void
drag_data_received (GtkWidget *widget, GdkDragContext *context, gint x, gint y,
                    GtkSelectionData *selection_data, guint info, guint time, ConversationPanel *panel)
{
	switch (info) {
	case TARGET_TEXT:
	case TARGET_STRING:
	case TARGET_COMPOUND_TEXT:
	case TARGET_UTF8_STRING:
	case TARGET_TEXT_PLAIN:
	{
		guchar *txt;

		txt = gtk_selection_data_get_text (selection_data);
		if (gui.current_session != NULL)
		handle_multiline (gui.current_session, txt, TRUE, FALSE);

		g_free (txt);
		break;
	}
	case TARGET_URI_LIST:
	{
		guchar *uri_list;
		gchar **uris;
		gint nb_uri;

		if ((panel->priv->current->type != SESS_CHANNEL) &&
		    (panel->priv->current->type != SESS_DIALOG))
			return;

		if (selection_data->format != 8 || selection_data->length == 0) {
			g_printerr (_("URI list dropped on xchat-gnome had wrong format (%d) or length (%d)\n"),
			            selection_data->format, selection_data->length);
			return;
		}

		uri_list = g_strndup (selection_data->data, selection_data->length);
		uris = g_strsplit (uri_list, "\r\n", 0);
		g_free (uri_list);

		free_dropped_files (panel);

		for (nb_uri = 0; uris[nb_uri] && strlen (uris[nb_uri]) > 0; nb_uri++)
			panel->priv->dropped_files = g_slist_prepend (panel->priv->dropped_files, uris[nb_uri]);
		g_free (uris); /* String in uris will be freed in free_dropped_files */
		panel->priv->dropped_files = g_slist_reverse (panel->priv->dropped_files);

		if (context->action == GDK_ACTION_ASK) {
			/* Display the context menu */
			GtkWidget *menu, *entry;

			menu = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup");
			entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropPasteFile");
			if (nb_uri > 1 ||
			    (uri_is_text (panel->priv->dropped_files->data) == FALSE) ||
			    (check_file_size (panel->priv->dropped_files->data) == FALSE))
				gtk_widget_set_sensitive (entry, FALSE);
			else
				gtk_widget_set_sensitive (entry, TRUE);

			/* Enable/Disable send files */
			entry = gtk_ui_manager_get_widget (gui.manager, "/DropFilePopup/DropSendFiles");
			if (panel->priv->current->type == SESS_CHANNEL)
				gtk_widget_set_sensitive (entry, FALSE);
			else
				gtk_widget_set_sensitive (entry, TRUE);

			gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 2, gtk_get_current_event_time ());
		} else {
			/* Do the default action */
			if (gui.current_session->type == SESS_CHANNEL) {
				/* Dropped in a channel */
				if (nb_uri == 1 &&
				    uri_is_text (panel->priv->dropped_files->data) &&
				    check_file_size (panel->priv->dropped_files->data))
					drop_paste (panel);
			} else {
				/* Dropped in a query */
				if (nb_uri == 1 &&
				    uri_is_text (panel->priv->dropped_files->data) &&
				    check_file_size (panel->priv->dropped_files->data))
					drop_paste (panel);
				else
					drop_send (panel);
			}
		}
	}
	}
}

static void
free_dropped_files (ConversationPanel *panel)
{
	if (panel->priv->dropped_files) {
		g_slist_foreach (panel->priv->dropped_files, (GFunc) g_free, NULL);
		g_slist_free    (panel->priv->dropped_files);
		panel->priv->dropped_files = NULL;
	}
}

static void
drop_send (ConversationPanel *panel)
{
	g_return_if_fail (panel->priv->dropped_files != NULL);
	g_slist_foreach (panel->priv->dropped_files, send_file, NULL);
        free_dropped_files (panel);
}

static void
drop_paste (ConversationPanel *panel)
{
	GnomeVFSResult result;
	gchar *contents, *uri;

	g_return_if_fail (g_slist_length (panel->priv->dropped_files) == 1);

	uri = panel->priv->dropped_files->data;
	result = gnome_vfs_read_entire_file (uri, NULL, &contents);
	if (result == GNOME_VFS_OK) {
		if (panel->priv->current != NULL)
			handle_multiline (panel->priv->current, (char *) contents, TRUE, FALSE);
		g_free (contents);
	} else {
		g_printerr (_("Error reading file \"%s\": %s\n"), uri, gnome_vfs_result_to_string (result));
	}

	free_dropped_files (panel);
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
on_default_copy_activate (GtkAction *action, ConversationPanel *panel)
{
	conversation_panel_copy_selection (panel);
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

GtkWidget *
conversation_panel_new (void)
{
	return GTK_WIDGET (g_object_new (conversation_panel_get_type (), NULL));
}

void
conversation_panel_update_colors (ConversationPanel *panel)
{
	palette_alloc (panel->priv->xtext);
	gtk_xtext_set_palette (GTK_XTEXT (panel->priv->xtext), colors);
	gtk_xtext_refresh     (GTK_XTEXT (panel->priv->xtext), FALSE);
}

void
conversation_panel_add_session (ConversationPanel *panel, struct session *sess, gboolean focus)
{
	GConfClient  *client;
	xtext_buffer *buffer;
	gint          notify;

	buffer = gtk_xtext_buffer_new (GTK_XTEXT (panel->priv->xtext));

	client = gconf_client_get_default ();
	gtk_xtext_set_time_stamp (buffer, gconf_client_get_bool (client, "/apps/xchat/irc/showtimestamps", NULL));
	notify = gconf_client_notify_add (client, "/apps/xchat/irc/showtimestamps",
	                                  (GConfClientNotifyFunc) timestamps_changed, buffer, NULL, NULL);
	g_object_unref (client);

	g_hash_table_insert (panel->priv->buffers,            sess, buffer);
	g_hash_table_insert (panel->priv->timestamp_notifies, sess, GINT_TO_POINTER (notify));

	if (focus)
		conversation_panel_set_current (panel, sess);
}

void
conversation_panel_set_current (ConversationPanel *panel, struct session *sess)
{
	xtext_buffer *buffer;

	panel->priv->current = sess;
	buffer = g_hash_table_lookup (panel->priv->buffers, sess);
	gtk_xtext_buffer_show (GTK_XTEXT (panel->priv->xtext), buffer, TRUE);
}

void
conversation_panel_save_current (ConversationPanel *panel)
{
	GtkWidget *file_chooser;
	gchar     *default_filename;
	gchar      dates[32];
	struct tm  date;
	time_t     dtime;

	file_chooser = gtk_file_chooser_dialog_new (_("Save Transcript"),
	                                            GTK_WINDOW (gui.main_window),
	                                            GTK_FILE_CHOOSER_ACTION_SAVE,
	                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                            GTK_STOCK_SAVE,   GTK_RESPONSE_ACCEPT,
	                                            NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (file_chooser), TRUE);
	gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (file_chooser), TRUE);
	gtk_dialog_set_default_response (GTK_DIALOG (file_chooser), GTK_RESPONSE_ACCEPT);

	time (&dtime);
	localtime_r (&dtime, &date);
	strftime (dates, 32, "%F-%Hh%M", &date);

	default_filename = g_strdup_printf ("%s-%s.log", panel->priv->current->channel, dates);
	gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (file_chooser), default_filename);
	g_free (default_filename);

	if (gtk_dialog_run (GTK_DIALOG (file_chooser)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename;
		GIOChannel *file;
		GError *error = NULL;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser));
		file = g_io_channel_new_file (filename, "w", &error);
		if (error) {
			gchar *header = g_strdup_printf (_("Error saving %s"), filename);
			error_dialog (header, error->message);
			g_free (header);
			g_error_free (error);
		} else {
			gint fd = g_io_channel_unix_get_fd (file);
			gtk_xtext_save (GTK_XTEXT (panel->priv->xtext), fd);
			g_io_channel_shutdown (file, TRUE, &error);

			if (error) {
				gchar *header = g_strdup_printf (_("Error saving %s"), filename);
				error_dialog (header, error->message);
				g_free (header);
				g_error_free (error);
			}
		}
		g_free (filename);
	}
	gtk_widget_destroy (file_chooser);
}

void
conversation_panel_clear (ConversationPanel *panel, struct session *sess)
{
	xtext_buffer *buffer;

	buffer = g_hash_table_lookup (panel->priv->buffers, sess);
	gtk_xtext_clear (buffer);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), FALSE);
}

static void
conversation_panel_print_line (xtext_buffer *buffer, guchar *text, int len, gboolean indent)
{
	int            leftlen;
	unsigned char *tab;

	if (len == 0)
		len = 1;

	if (indent == FALSE) {
		int     stamp_size;
		char   *stamp;
		guchar *new_text;

		stamp_size = get_stamp_str (prefs.stamp_format, time(NULL), &stamp);
		new_text = g_malloc (len + stamp_size + 1);
		memcpy (new_text, stamp, stamp_size);
		g_free (stamp);
		memcpy (new_text + stamp_size, text, len);
		gtk_xtext_append (buffer, new_text, len + stamp_size);
		g_free (new_text);
		return;
	}

	tab = strchr (text, '\t');
	if (tab && tab < (text + len)) {
		leftlen = tab - text;
		gtk_xtext_append_indent (buffer, text, leftlen, tab + 1, len - (leftlen + 1));
	} else {
		gtk_xtext_append_indent (buffer, 0, 0, text, len);
	}
}

void
conversation_panel_print (ConversationPanel *panel, struct session *sess, guchar *text, gboolean indent)
{
	xtext_buffer *buffer;
	guchar       *last_text = text;
	int           len       = 0;

	buffer = g_hash_table_lookup (panel->priv->buffers, sess);

	/* split the text into separate lines */
	while (1) {
		switch (*text) {
		case '\0':
			conversation_panel_print_line (buffer, last_text, len, indent);
			return;
		case '\n':
			conversation_panel_print_line (buffer, last_text, len, indent);
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
conversation_panel_remove_session (ConversationPanel *panel, struct session *sess)
{
	GConfClient *client;
	gint         notify;

	client = gconf_client_get_default ();
	notify = GPOINTER_TO_INT (g_hash_table_lookup (panel->priv->timestamp_notifies, sess));
	g_hash_table_remove (panel->priv->timestamp_notifies, sess);
	gconf_client_notify_remove (client, notify);
	g_object_unref (client);

	g_hash_table_remove (panel->priv->buffers, sess);
}

static void
conversation_panel_lastlog_foreach (GtkXText *xtext, guchar *text, fe_lastlog_info *info)
{
	if (nocasestrstr (text, info->sstr))
		conversation_panel_print (info->panel, info->sess, text, prefs.indent_nicks);
}

void
conversation_panel_lastlog (ConversationPanel *panel, struct session *sess, struct session *lsess, char *sstr)
{
	xtext_buffer *buffer, *lbuffer;

	buffer  = g_hash_table_lookup (panel->priv->buffers, sess);
	lbuffer = g_hash_table_lookup (panel->priv->buffers, lsess);

	if (gtk_xtext_is_empty (buffer)) {
		conversation_panel_print (panel, lsess, _("Search buffer is empty.\n"), TRUE);
	} else {
		fe_lastlog_info info;
		info.panel = panel;
		info.sess  = lsess;
		info.sstr  = sstr;

		gtk_xtext_foreach (buffer, (GtkXTextForeach) conversation_panel_lastlog_foreach, &info);
	}
}

void
conversation_panel_clear_selection (ConversationPanel *panel)
{
	gtk_xtext_selection_clear_full (GTK_XTEXT (panel->priv->xtext)->buffer);
        gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), TRUE);
}

gpointer
conversation_panel_search (ConversationPanel *panel, const gchar *text, gpointer start, gboolean casem, gboolean reverse)
{
	return gtk_xtext_search (GTK_XTEXT (panel->priv->xtext), text, start, casem, reverse);
}

void
conversation_panel_page_up (ConversationPanel *panel)
{
	GtkAdjustment *adj;
	int end, value;

	adj = GTK_XTEXT (panel->priv->xtext)->adj;
	end = adj->upper - adj->lower - adj->page_size;
	value = adj->value - (adj->page_size - 1);
	if (value < 0)
		value = 0;
	if (value > end)
		value = end;
	gtk_adjustment_set_value (adj, value);
}

void
conversation_panel_page_down (ConversationPanel *panel)
{
	GtkAdjustment *adj;
	int value, end;

	adj = GTK_XTEXT (panel->priv->xtext)->adj;
	end = adj->upper - adj->lower - adj->page_size;
	value = adj->value + (adj->page_size - 1);
	if (value < 0)
		value = 0;
	if (value > end)
		value = end;
	gtk_adjustment_set_value (adj, value);
}

void
conversation_panel_copy_selection (ConversationPanel *panel)
{
	GtkClipboard *clipboard = gtk_widget_get_clipboard (GTK_WIDGET (panel), GDK_SELECTION_CLIPBOARD);
	gtk_xtext_copy_selection (GTK_XTEXT (panel->priv->xtext), clipboard);
}
