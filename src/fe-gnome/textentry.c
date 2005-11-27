/*
 * textentry.c - Widget encapsulating the text entry
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
#include "textentry.h"
#include "userlist.h"
#include "gui.h"
#include "../common/outbound.h"

static void     text_entry_class_init  (TextEntryClass *klass);
static void     text_entry_init        (TextEntry      *entry);
static void     text_entry_finalize    (GObject        *object);
static gboolean text_entry_key_press   (GtkWidget      *widget,
                                        GdkEventKey    *event,
                                        gpointer        data);
static gboolean text_entry_spell_check (SexySpellEntry *entry,
                                        gchar          *text,
                                        gpointer        data);
static void     text_entry_activate    (GtkWidget      *widget,
                                        gpointer        data);

#ifdef HAVE_LIBSEXY
static SexySpellEntryClass *parent_class = NULL;
G_DEFINE_TYPE (TextEntry, text_entry, SEXY_TYPE_SPELL_ENTRY);
#else
static GtkEntryClass       *parent_class = NULL;
G_DEFINE_TYPE (TextEntry, text_entry, GTK_TYPE_ENTRY);
#endif

static void
text_entry_class_init (TextEntryClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = text_entry_finalize;
}

static void
text_entry_init (TextEntry *entry)
{
#ifdef HAVE_LIBSEXY
	g_signal_connect_after (G_OBJECT (entry), "word-check",      G_CALLBACK (text_entry_spell_check), NULL);
#endif
//	g_signal_connect_after (G_OBJECT (entry), "key_press_event", G_CALLBACK (text_entry_key_press),   NULL);
	g_signal_connect       (G_OBJECT (entry), "activate",        G_CALLBACK (text_entry_activate),    NULL);
}

static void
text_entry_finalize (GObject *object)
{
	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
text_entry_key_press (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	/*
	switch (event->keyval) {
		case GDK_Down:
			history_key_down (GTK_ENTRY (widget));
			return TRUE;
		case GDK_Up:
			history_key_up (GTK_ENTRY (widget));
			return TRUE;
		case GDK_Tab:
			return text_entry_tab_complete (widget);
		default:
			break;
	}
	*/
	return FALSE;
}

static gboolean
text_entry_spell_check (SexySpellEntry *entry, gchar *text, gpointer data)
{
	GtkTreeModel *store = GTK_TREE_MODEL (userlist_get_store (u, gui.current_session));
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter_first (store, &iter) == FALSE)
		return TRUE;
	do {
		gchar *nick;
		gboolean match = FALSE;

		gtk_tree_model_get (store, &iter, 1, &nick, -1);
		if (strncmp (text, nick, strlen (nick)) == 0)
			match = TRUE;

		g_free (nick);
		if (match)
			return FALSE;
	} while (gtk_tree_model_iter_next (store, &iter));
	return TRUE;
}

static void
text_entry_activate (GtkWidget *widget, gpointer data)
{
	char *entry_text = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
	gtk_entry_set_text (GTK_ENTRY (widget), "");
	if (gui.current_session != NULL)
		handle_multiline (gui.current_session, (char *) entry_text, TRUE, FALSE);
	g_free (entry_text);
}

GtkWidget *
text_entry_new (void)
{
	return GTK_WIDGET (g_object_new (text_entry_get_type (), NULL));
}
