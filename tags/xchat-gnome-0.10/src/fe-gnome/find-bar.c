/*
 * find-bar.c - Widget encapsulating the find bar
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
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include "conversation-panel.h"
#include "find-bar.h"
#include "gui.h"

static void     find_bar_class_init     (FindBarClass *klass);
static void     find_bar_init           (FindBar      *bar);
static void     find_bar_finalize       (GObject      *object);
static void     find_bar_close_cb       (GtkWidget    *button,
                                         FindBar      *bar);
static void     find_bar_search_changed (GtkWidget    *entry,
                                         FindBar      *bar);
static gboolean find_bar_key_press      (GtkWidget    *entry,
                                         GdkEventKey  *event,
                                         FindBar      *bar);
static void     find_bar_search         (FindBar      *bar,
                                         gboolean      reverse);
static void     find_bar_set_status     (FindBar      *bar,
                                         guint         status);
static void     find_bar_next           (GtkWidget    *button,
                                         FindBar      *bar);
static void     find_bar_previous       (GtkWidget    *button,
                                         FindBar      *bar);

struct _FindBarPriv
{
	GtkWidget *entry;
	GtkWidget *status_wrapped;
	GtkWidget *status_notfound;
	GtkWidget *status_label;

	gpointer   last_position;
};

enum
{
	STATUS_NORMAL,
	STATUS_WRAPPED,
	STATUS_WRAPPED_REVERSE,
	STATUS_NOTFOUND,
};

GtkHBoxClass *parent_class;
G_DEFINE_TYPE (FindBar, find_bar, GTK_TYPE_HBOX);

static void
find_bar_class_init (FindBarClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = find_bar_finalize;
}

static void
find_bar_init (FindBar *bar)
{
	GtkWidget *close;
	GtkWidget *close_image;
	GtkWidget *find_label;
	GtkWidget *previous;
	GtkWidget *previous_image;
	GtkWidget *next;
	GtkWidget *next_image;

	bar->priv = g_new0 (FindBarPriv, 1);

	close            = gtk_button_new ();
	close_image      = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
	find_label       = gtk_label_new (_("Find:"));
	bar->priv->entry = gtk_entry_new ();
	previous         = gtk_button_new_with_mnemonic (_("_Previous"));
	previous_image   = gtk_image_new_from_stock (GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON);
	next             = gtk_button_new_with_mnemonic (_("_Next"));
	next_image       = gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON);
	if (g_file_test ("../../data/wrap.png", G_FILE_TEST_EXISTS))
		bar->priv->status_wrapped = gtk_image_new_from_file ("../../data/wrap.png");
	else
		bar->priv->status_wrapped = gtk_image_new_from_file (XCHATSHAREDIR "/wrap.png");
	bar->priv->status_notfound = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_BUTTON);
	bar->priv->status_label = gtk_label_new (NULL);

	gtk_container_add (GTK_CONTAINER (close), close_image);
	gtk_button_set_image (GTK_BUTTON (previous), previous_image);
	gtk_button_set_image (GTK_BUTTON (next),     next_image);
	gtk_misc_set_alignment (GTK_MISC (bar->priv->status_label), 0.0f, 0.5f);

	gtk_box_set_spacing (GTK_BOX (bar), 6);
	gtk_box_pack_start  (GTK_BOX (bar), close,                      FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), find_label,                 FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), bar->priv->entry,           FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), previous,                   FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), next,                       FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), bar->priv->status_wrapped,  FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), bar->priv->status_notfound, FALSE, TRUE, 0);
	gtk_box_pack_start  (GTK_BOX (bar), bar->priv->status_label,    TRUE, TRUE, 0);

	gtk_button_set_relief (GTK_BUTTON (close),    GTK_RELIEF_NONE);
	gtk_button_set_relief (GTK_BUTTON (previous), GTK_RELIEF_NONE);
	gtk_button_set_relief (GTK_BUTTON (next),     GTK_RELIEF_NONE);

	gtk_widget_show (close);
	gtk_widget_show (close_image);
	gtk_widget_show (find_label);
	gtk_widget_show (bar->priv->entry);
	gtk_widget_show (previous);
	gtk_widget_show (next);
	gtk_widget_show (bar->priv->status_label);

	g_signal_connect (G_OBJECT (close),            "clicked",         G_CALLBACK (find_bar_close_cb),       bar);
	g_signal_connect (G_OBJECT (bar->priv->entry), "changed",         G_CALLBACK (find_bar_search_changed), bar);
	g_signal_connect (G_OBJECT (bar->priv->entry), "key-press-event", G_CALLBACK (find_bar_key_press),      bar);
	g_signal_connect (G_OBJECT (bar->priv->entry), "activate",        G_CALLBACK (find_bar_next),           bar);
	g_signal_connect (G_OBJECT (next),             "clicked",         G_CALLBACK (find_bar_next),           bar);
	g_signal_connect (G_OBJECT (previous),         "clicked",         G_CALLBACK (find_bar_previous),       bar);
}

static void
find_bar_finalize (GObject *object)
{
	FindBar *bar;

	bar = FIND_BAR (object);

	g_free (bar->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
find_bar_close_cb (GtkWidget *button, FindBar *bar)
{
	find_bar_close (bar);
}

static void
find_bar_search_changed (GtkWidget *entry, FindBar *bar)
{
	bar->priv->last_position = NULL;
	find_bar_search (bar, FALSE);
}

static gboolean
find_bar_key_press (GtkWidget *entry, GdkEventKey *event, FindBar *bar)
{
	if (event->keyval == GDK_Escape)
		find_bar_close (bar);
	return FALSE;
}

static void
find_bar_search (FindBar *bar, gboolean reverse)
{
	gpointer position;
	const gchar *text;

	text = gtk_entry_get_text (GTK_ENTRY (bar->priv->entry));

	position = conversation_panel_search (CONVERSATION_PANEL (gui.conversation_panel), text, bar->priv->last_position, FALSE, reverse);

	if ((position == NULL) && (bar->priv->last_position != NULL)) {
		if (reverse) {
			position = conversation_panel_search (CONVERSATION_PANEL (gui.conversation_panel), text, NULL, FALSE, reverse);
			find_bar_set_status (bar, STATUS_WRAPPED_REVERSE);
		} else {
			position = conversation_panel_search (CONVERSATION_PANEL (gui.conversation_panel), text, NULL, FALSE, reverse);
			find_bar_set_status (bar, STATUS_WRAPPED);
		}
	} else if (position == NULL) {
		find_bar_set_status (bar, STATUS_NOTFOUND);
	} else {
		find_bar_set_status (bar, STATUS_NORMAL);
	}

	bar->priv->last_position = position;
}

static void
find_bar_set_status (FindBar *bar, guint status)
{
	switch (status) {
	case STATUS_NORMAL:
		gtk_widget_hide (bar->priv->status_wrapped);
		gtk_widget_hide (bar->priv->status_notfound);
		gtk_label_set_text (GTK_LABEL (bar->priv->status_label), "");
		break;

	case STATUS_WRAPPED:
		gtk_widget_show (bar->priv->status_wrapped);
		gtk_widget_hide (bar->priv->status_notfound);
		gtk_label_set_markup (GTK_LABEL (bar->priv->status_label),
		                      _("<span foreground=\"dark grey\">Reached end, continuing from top</span>"));
		break;

	case STATUS_WRAPPED_REVERSE:
		gtk_widget_show (bar->priv->status_wrapped);
		gtk_widget_hide (bar->priv->status_notfound);
		gtk_label_set_markup (GTK_LABEL (bar->priv->status_label),
		                      _("<span foreground=\"dark grey\">Reached beginning, continuing from bottom</span>"));
		break;

	case STATUS_NOTFOUND:
		gtk_widget_hide (bar->priv->status_wrapped);
		gtk_widget_show (bar->priv->status_notfound);
		gtk_label_set_markup (GTK_LABEL (bar->priv->status_label),
		                      _("<span foreground=\"red\">Search string not found</span>"));
		break;
	}
}

static void
find_bar_next (GtkWidget *button, FindBar *bar)
{
	find_bar_search (bar, FALSE);
}

static void
find_bar_previous (GtkWidget *button, FindBar *bar)
{
	find_bar_search (bar, TRUE);
}

GtkWidget *
find_bar_new (void)
{
	return GTK_WIDGET (g_object_new (find_bar_get_type (), NULL));
}

void
find_bar_close (FindBar *bar)
{
	if (!GTK_WIDGET_VISIBLE (bar))
		return;

	conversation_panel_clear_selection (CONVERSATION_PANEL (gui.conversation_panel));
	gtk_entry_set_text (GTK_ENTRY (bar->priv->entry), "");
	find_bar_set_status (bar, STATUS_NORMAL);


	gtk_widget_hide (GTK_WIDGET (bar));

	gtk_widget_grab_focus (gui.text_entry);
}

void
find_bar_open (FindBar *bar)
{
	gtk_widget_show (GTK_WIDGET (bar));
	gtk_widget_grab_focus (bar->priv->entry);
}
