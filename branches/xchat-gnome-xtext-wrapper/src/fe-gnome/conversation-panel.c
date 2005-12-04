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
#include "conversation-panel.h"
#include "palette.h"
#include "xtext.h"

static void conversation_panel_class_init (ConversationPanelClass *klass);
static void conversation_panel_init       (ConversationPanel      *panel);
static void conversation_panel_finalize   (GObject                *object);

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
	panel->priv->scrollbar = gtk_vscrollbar_new (GTK_XTEXT (panel->priv->xtext)->adj);

	gtk_box_set_spacing (GTK_BOX (panel), 6);
	gtk_box_pack_start  (GTK_BOX (panel), panel->priv->xtext,     TRUE,  TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (panel), panel->priv->scrollbar, FALSE, TRUE, 0);

	gtk_widget_show (panel->priv->xtext);
	gtk_widget_show (panel->priv->scrollbar);
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

GtkWidget *
conversation_panel_new (void)
{
	return GTK_WIDGET (g_object_new (conversation_panel_get_type (), NULL));
}
