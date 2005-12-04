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
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "conversation-panel.h"
#include "palette.h"
#include "xtext.h"
#include "../common/xchatc.h"

static void conversation_panel_class_init   (ConversationPanelClass *klass);
static void conversation_panel_init         (ConversationPanel      *panel);
static void conversation_panel_finalize     (GObject                *object);
static void conversation_panel_realize      (GtkWidget              *widget);
static int  conversation_panel_check_word   (GtkWidget              *xtext,
                                             char                   *word,
                                             int                     len);
static void conversation_panel_clicked_word (GtkWidget              *xtext,
                                             char                   *word,
                                             GdkEventButton         *event,
                                             gpointer                data);
static void conversation_panel_set_font     (ConversationPanel      *panel);
static void conversation_panel_font_changed (GConfClient            *client,
                                             guint                   cnxn_id,
                                             GConfEntry             *entry,
                                             ConversationPanel      *panel);

struct _ConversationPanelPriv
{
	GtkWidget  *scrollbar;
	GtkWidget  *xtext;

	GHashTable *buffers;
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

	conversation_panel_set_font (panel);

	g_signal_connect (G_OBJECT (panel->priv->xtext), "word_click", G_CALLBACK (conversation_panel_clicked_word), NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/use_sys_fonts", (GConfClientNotifyFunc) conversation_panel_font_changed, panel, NULL, NULL);
	gconf_client_notify_add (client, "/apps/xchat/main_window/font",          (GConfClientNotifyFunc) conversation_panel_font_changed, panel, NULL, NULL);

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
	GConfClient   *client;
	gchar         *font;
	GtkAdjustment *adj;

	client = gconf_client_get_default ();
	if (gconf_client_get_bool(client, "/apps/xchat/main_window/use_sys_fonts", NULL))
		font = gconf_client_get_string (client, "/desktop/gnome/interface/monospace_font_name", NULL);
	else
		font = gconf_client_get_string (client, "/apps/xchat/main_window/font", NULL);
	g_object_unref (client);

	if (font == NULL)
		font = g_strdup ("fixed 11");

	gtk_xtext_set_font (GTK_XTEXT (panel->priv->xtext), font);
	adj = GTK_XTEXT (panel->priv->xtext)->adj;
	gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
	gtk_xtext_refresh (GTK_XTEXT (panel->priv->xtext), FALSE);

	g_free (font);
}

static void
conversation_panel_font_changed (GConfClient *client, guint cnxn_id, GConfEntry *entry, ConversationPanel *panel)
{
	conversation_panel_set_font (panel);
}

GtkWidget *
conversation_panel_new (void)
{
	return GTK_WIDGET (g_object_new (conversation_panel_get_type (), NULL));
}
