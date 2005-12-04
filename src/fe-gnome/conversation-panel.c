/*
 * conversation-panel.c - Widget encapsulating the conversation panel
 *
 * Copyright (C) 2005 David Trowbridge
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
#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "util.h"
#include "xtext.h"
#include "../common/text.h"
#include "../common/util.h"
#include "../common/xchatc.h"

typedef struct _fe_lastlog_info fe_lastlog_info;

static void conversation_panel_class_init         (ConversationPanelClass *klass);
static void conversation_panel_init               (ConversationPanel      *panel);
static void conversation_panel_finalize           (GObject                *object);
static void conversation_panel_realize            (GtkWidget              *widget);
static int  conversation_panel_check_word         (GtkWidget              *xtext,
                                                   char                   *word,
                                                   int                     len);
static void conversation_panel_clicked_word       (GtkWidget              *xtext,
                                                   char                   *word,
                                                   GdkEventButton         *event,
                                                   gpointer                data);
static void conversation_panel_set_font           (ConversationPanel      *panel);
static void conversation_panel_font_changed       (GConfClient            *client,
                                                   guint                   cnxn_id,
                                                   GConfEntry             *entry,
                                                   ConversationPanel      *panel);
static void conversation_panel_set_background     (ConversationPanel      *panel);
static void conversation_panel_background_changed (GConfClient            *client,
                                                   guint                   cnxn_id,
                                                   GConfEntry             *entry,
                                                   ConversationPanel      *panel);
static void timestamps_changed                    (GConfClient            *client,
                                                   guint                   cnxn_id,
                                                   GConfEntry             *entry,
                                                   xtext_buffer           *buffer);
static void conversation_panel_print_line         (xtext_buffer           *buffer,
                                                   guchar                  *text,
                                                   int                     len,
                                                   gboolean                indent);
static void conversation_panel_lastlog_foreach    (GtkXText               *xtext,
                                                   guchar                 *text,
                                                   fe_lastlog_info        *info);

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
};

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

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click", G_CALLBACK (conversation_panel_clicked_word), NULL);
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
}

static void
conversation_panel_clicked_word (GtkWidget *xtext, char *word, GdkEventButton *event, gpointer data)
{
}

static void
conversation_panel_set_font (ConversationPanel *panel)
{
	GConfClient *client;
	gchar       *font;

	client = gconf_client_get_default ();
	if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);
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
	conversation_panel_set_font (panel);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), TRUE);
}

static void
timestamps_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, xtext_buffer *buffer)
{
	gtk_xtext_set_time_stamp (buffer, gconf_client_get_bool (client, entry->key, NULL));
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
conversation_panel_add_session (ConversationPanel *panel, struct session *sess)
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
	strftime (dates, 32, "%F-%R", &date);

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
