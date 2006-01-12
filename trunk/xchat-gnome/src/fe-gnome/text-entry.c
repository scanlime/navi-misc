/*
 * textentry.c - Widget encapsulating the text entry
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
#include <gdk/gdkkeysyms.h>
#include "conversation-panel.h"
#include "gui.h"
#include "palette.h"
#include "text-entry.h"
#include "userlist.h"
#include "../common/outbound.h"

static void       text_entry_class_init     (TextEntryClass *klass);
static void       text_entry_init           (TextEntry      *entry);
static void       text_entry_finalize       (GObject        *object);
static gboolean   text_entry_key_press      (GtkWidget      *widget,
                                             GdkEventKey    *event,
                                             gpointer        data);
#ifdef HAVE_LIBSEXY
static gboolean   text_entry_spell_check    (TextEntry      *entry,
                                             gchar          *text,
                                             gpointer        data);
#endif
static void       text_entry_activate       (GtkWidget      *widget,
                                             gpointer        data);
static void       text_entry_history_up     (GtkEntry       *entry);
static void       text_entry_history_down   (GtkEntry       *entry);
static gboolean   text_entry_tab_complete   (GtkEntry       *entry);
static void       text_entry_populate_popup (GtkEntry       *entry,
                                             GtkMenu        *menu,
                                             gpointer        data);

static gboolean   tab_complete_command      (GtkEntry       *entry);
static gboolean   tab_complete_nickname     (GtkEntry       *entry,
                                             int             start);
static GtkWidget *get_color_icon            (int             c,
                                             GtkStyle       *style);
static void       color_code_activate       (GtkMenuItem    *item,
                                             gpointer        data);

#ifdef HAVE_LIBSEXY
static SexySpellEntryClass *parent_class = NULL;
G_DEFINE_TYPE (TextEntry, text_entry, SEXY_TYPE_SPELL_ENTRY);
#else
static GtkEntryClass       *parent_class = NULL;
G_DEFINE_TYPE (TextEntry, text_entry, GTK_TYPE_ENTRY);
#endif

struct _TextEntryPriv
{
	GCompletion    *command_completion;

	GHashTable     *entries;
	struct session *current;
};

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
	GList *items = NULL;
	int i;

	g_signal_connect_after (G_OBJECT (entry), "key_press_event", G_CALLBACK (text_entry_key_press),      NULL);
	g_signal_connect       (G_OBJECT (entry), "activate",        G_CALLBACK (text_entry_activate),       NULL);
	g_signal_connect       (G_OBJECT (entry), "populate-popup",  G_CALLBACK (text_entry_populate_popup), NULL);
#ifdef HAVE_LIBSEXY
	g_signal_connect_after (G_OBJECT (entry), "word-check",      G_CALLBACK (text_entry_spell_check),    NULL);
#endif

	entry->priv = g_new0 (TextEntryPriv, 1);

	entry->priv->entries = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);

	/* Initialize & populate a GCompletion for commands */
	entry->priv->command_completion = g_completion_new (NULL);
	/* FIXME - need to convert command_list to a GList
	g_completion_add_items (entry->priv->command_completion, command_list);
	*/
	for (i = 0; xc_cmds[i].name != NULL; i++) {
		items = g_list_prepend (items, xc_cmds[i].name);
	}
	items = g_list_reverse (items);
	g_completion_add_items (entry->priv->command_completion, items);
	g_list_free (items);

	gtk_widget_show (GTK_WIDGET (entry));
}

static void
text_entry_finalize (GObject *object)
{
	TextEntry *entry;

	entry = TEXT_ENTRY (object);

	if (entry->priv->entries)
		g_hash_table_destroy (entry->priv->entries);
	if (entry->priv->command_completion)
		g_completion_free (entry->priv->command_completion);
	if (entry->priv)
		g_free (entry->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
text_entry_key_press (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	switch (event->keyval) {
		case GDK_Down:
			text_entry_history_down (GTK_ENTRY (widget));
			return TRUE;
		case GDK_Up:
			text_entry_history_up (GTK_ENTRY (widget));
			return TRUE;
		case GDK_Tab:
			return text_entry_tab_complete (GTK_ENTRY (widget));
		default:
			break;
	}
	return FALSE;
}

#ifdef HAVE_LIBSEXY
static gboolean
text_entry_spell_check (TextEntry *entry, gchar *text, gpointer data)
{
	GtkTreeModel *store = GTK_TREE_MODEL (userlist_get_store (u, entry->priv->current));
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
#endif

static void
text_entry_activate (GtkWidget *widget, gpointer data)
{
	char *entry_text = g_strdup (gtk_entry_get_text (GTK_ENTRY (widget)));
	gtk_entry_set_text (GTK_ENTRY (widget), "");
	if (TEXT_ENTRY (widget)->priv->current != NULL)
		handle_multiline (TEXT_ENTRY (widget)->priv->current, (char *) entry_text, TRUE, FALSE);
	g_free (entry_text);
}

static void
text_entry_history_up (GtkEntry *entry)
{
	TextEntry *text_entry;
	char *new_line;

	text_entry = TEXT_ENTRY (entry);
	if (text_entry->priv->current == NULL)
		return;

	new_line = history_up (&(text_entry->priv->current->history), (char *)entry->text);
	if (new_line) {
		gtk_entry_set_text (entry, new_line);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
}

static void
text_entry_history_down (GtkEntry *entry)
{
	TextEntry *text_entry;
	char *new_line;

	text_entry = TEXT_ENTRY (entry);
	if (text_entry->priv->current == NULL)
		return;

	new_line = history_down (&(text_entry->priv->current->history));
	if (new_line) {
		gtk_entry_set_text (entry, new_line);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
}

static gboolean
text_entry_tab_complete (GtkEntry *entry)
{
	const char *text;
	int start, cursor_pos;

	/* FIXME: this probably isn't unicode clean */

	text = gtk_entry_get_text (entry);
	cursor_pos = gtk_editable_get_position (GTK_EDITABLE (entry));

	if (cursor_pos == 0)
		return TRUE;

	/* If we're directly after a space, we have nothing to tab complete */
	if (text[cursor_pos - 1] == ' ')
		return TRUE;

	/* search backwards to find /, #, ' ' or start */
	for (start = cursor_pos - 1; start >= 0; --start) {
		/* check if we can match a channel */
		/* FIXME: implement
		if (text[start] == '#') {
			if (start == 0 || text[start - 1] == ' ') {
				tab_complete_channel (entry, start);
				return;
			}
		}
		*/

		/* check if we can match a command */
		if (start == 0 && text[0] == '/') {
			return tab_complete_command (entry);
		}

		/* check if we can match a nickname */
		if (start == 0 || text[start] == ' ') {
			return tab_complete_nickname (entry, start == 0 ? start : start + 1);
		}
	}
	return TRUE;
}

static void
text_entry_populate_popup (GtkEntry *entry, GtkMenu *menu, gpointer data)
{
	GtkWidget *item;
	GtkWidget *submenu;

	item = gtk_menu_item_new_with_mnemonic (_("I_nsert Color Code"));
	gtk_widget_show (item);

	submenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), submenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

	item = gtk_image_menu_item_new_with_label (_("Black"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (1, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 1);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Dark Blue"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (2, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 2);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Dark Green"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (3, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 3);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Red"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (4, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 4);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Brown"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (5, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 5);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Purple"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (6, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 6);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Orange"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (7, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 7);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Yellow"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (8, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 8);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Light Green"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (9, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 9);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Aqua"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (10, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 10);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Light Blue"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (11, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 11);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Blue"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (12, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 12);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Violet"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (13, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 13);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Grey"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (14, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 14);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("Light Grey"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (15, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 15);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);
	item = gtk_image_menu_item_new_with_label (_("White"));
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item), get_color_icon (0, gtk_widget_get_style (item)));
	g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (color_code_activate), (gpointer) 0);
	gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

	gtk_widget_show_all (submenu);
}

static gboolean
tab_complete_command (GtkEntry *entry)
{
	TextEntry *text_entry;
	int cursor, length, pos;
	char *prefix, *new_prefix, *printtext, *npt = NULL;
	const gchar *text;
	GList *options, *list;

	text_entry = TEXT_ENTRY (entry);

	cursor = gtk_editable_get_position (GTK_EDITABLE (entry));
	prefix = g_new0 (char, cursor);
	text = gtk_entry_get_text (entry);
	strncpy (prefix, &text[1], cursor - 1);
	length = strlen (text);

	options = g_completion_complete (text_entry->priv->command_completion, prefix, &new_prefix);

	if (g_list_length (options) == 0) {
		/* no matches */
		g_free (prefix);
		return TRUE;
	}

	if (g_list_length (options) == 1) {
		/* one match */

		if (length - cursor == 0) {
			/* at the end of the entry, just insert */

			npt = g_strdup_printf ("/%s ", (char *) options->data);
			pos = strlen (npt);
		} else {
			npt = g_strdup_printf ("/%s %s", (char *) options->data, &text[cursor]);
			pos = strlen ((char *) options->data) + 2;
		}
		gtk_entry_set_text (entry, npt);
		gtk_editable_set_position (GTK_EDITABLE (entry), pos);
		g_free (npt);
		g_free (prefix);
		return TRUE;
	} else {
		/* more than one match - print a list of options
		 * to the window and update the prefix
		 */
		list = options;
		printtext = g_strdup ((char *) list->data);
		for (list = g_list_next (list); list; list = g_list_next (list)) {
			npt = g_strdup_printf ("%s %s", printtext, (char *) list->data);
			g_free (printtext);
			printtext = npt;
		}
		conversation_panel_print (CONVERSATION_PANEL (gui.conversation_panel), text_entry->priv->current, (guchar *) printtext, TRUE);
		g_free (printtext);

		if (strcasecmp (prefix, new_prefix) != 0) {
			/* insert the new prefix into the entry */
			npt = g_strdup_printf ("/%s%s", new_prefix, &text[cursor]);
			gtk_entry_set_text (entry, npt);
			g_free (npt);
			gtk_editable_set_position (GTK_EDITABLE (entry), strlen (new_prefix));
		}
		g_free (prefix);
		g_free (npt);
		return TRUE;
	}

	return TRUE;
}

static gboolean
tab_complete_nickname (GtkEntry *entry, int start)
{
	GCompletion *completion;
	int cursor, length;
	char *text;
	GList *list;
	char *printtext, *npt;
	GList *options;
	gchar *new_prefix;
	gchar *prefix;
	TextEntry *text_entry;

	completion = userlist_get_completion (u, TEXT_ENTRY (entry)->priv->current);
	g_completion_set_compare (completion, (GCompletionStrncmpFunc) strncasecmp);
	text = g_strdup (gtk_entry_get_text (entry));
	length = strlen (text);
	cursor = gtk_editable_get_position (GTK_EDITABLE (entry));

	text_entry = TEXT_ENTRY (entry);

	prefix = g_new0 (char, cursor - start + 1);
	strncpy (prefix, &text[start], cursor - start);
	options = g_completion_complete (completion, prefix, &new_prefix);

	if (g_list_length (options) == 0) {
		/* no matches */
		g_free (text);
		g_free (prefix);
		return TRUE;
	}

	if (g_list_length (options) == 1) {
		int pos;

		/* one match */
		if (length - cursor == 0) {
			/* at the end of the entry, just insert */

			if (start != 0) {
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s", text, (char *) options->data);
				pos = strlen ((char *) options->data) + start;
			} else {
				npt = g_strdup_printf ("%s: ", (char *) options->data);
				pos = strlen ((char *) options->data) + 2;
			}
		} else {
			/* somewhere in the middle of the entry */

			if (start != 0) {
				text[start] = '\0';
				npt = g_strdup_printf ("%s%s%s", text, (char *) options->data, &text[cursor]);
				pos = strlen ((char *) options->data) + start;
			} else {
				npt = g_strdup_printf ("%s: %s", (char *) options->data, &text[cursor]);
				pos = strlen ((char *) options->data) + 2;
			}
		}
		gtk_entry_set_text (entry, npt);
		gtk_editable_set_position (GTK_EDITABLE (entry), pos);
		g_free (npt);
		g_free (text);
		g_free (prefix);
		return TRUE;
	} else {
		/* more than one match - print a list of options
		 * to the window and update the prefix
		 */
		list = options;
		printtext = g_strdup ((char *) list->data);
		for (list = g_list_next (list); list; list = g_list_next (list)) {
			npt = g_strdup_printf ("%s %s", printtext, (char *) list->data);
			g_free (printtext);
			printtext = npt;
		}
		conversation_panel_print (CONVERSATION_PANEL (gui.conversation_panel), text_entry->priv->current, (guchar *) printtext, TRUE);
		g_free (printtext);

		if (strcasecmp (prefix, new_prefix) != 0) {
			/* insert the new prefix into the entry */
			text[start] = '\0';
			npt = g_strdup_printf ("%s%s%s", text, new_prefix, &text[cursor]);
			gtk_entry_set_text (entry, npt);
			g_free (npt);
			gtk_editable_set_position (GTK_EDITABLE (entry), start + strlen (new_prefix));
		}
		g_free (text);
		g_free (prefix);
		return TRUE;
	}
	return TRUE;
}

static GtkWidget *
get_color_icon (int c, GtkStyle *style)
{
	GtkWidget *image;
	GdkPixmap *pixmap;
	GdkGC *color;

	pixmap = gdk_pixmap_new (NULL, 16, 16, 24);

	color = gdk_gc_new (GDK_DRAWABLE (pixmap));
	gdk_gc_set_foreground (color, &style->dark[GTK_STATE_NORMAL]);
	gdk_draw_rectangle (GDK_DRAWABLE (pixmap), color, TRUE, 0, 0, 16, 16);
	gdk_gc_set_foreground (color, &colors[c]);
	gdk_draw_rectangle (GDK_DRAWABLE (pixmap), color, TRUE, 1, 1, 14, 14);
	g_object_unref (color);

	image = gtk_image_new_from_pixmap (pixmap, NULL);
	g_object_unref (pixmap);
	return image;
}

static void
color_code_activate (GtkMenuItem *item, gpointer data)
{
	int color = (int) data;
	char *code = g_strdup_printf ("%%C%d", color);
	int position = gtk_editable_get_position (GTK_EDITABLE (gui.text_entry));
	gtk_editable_insert_text (GTK_EDITABLE (gui.text_entry), code, strlen (code), &position);
	gtk_editable_set_position (GTK_EDITABLE (gui.text_entry), position + strlen (code));
	g_free (code);
}

GtkWidget *
text_entry_new (void)
{
	return GTK_WIDGET (g_object_new (text_entry_get_type (), NULL));
}

void
text_entry_set_current (TextEntry *entry, struct session *sess)
{
	gchar *text;

	if (sess == entry->priv->current)
		return;

	if (sess == NULL) {
		gtk_entry_set_text (GTK_ENTRY (entry), "");
	} else {
		g_hash_table_insert (entry->priv->entries,
		                     entry->priv->current,
		                     g_strdup (gtk_entry_get_text (GTK_ENTRY (entry))));
		text = g_hash_table_lookup (entry->priv->entries, sess);
		if (text)
			gtk_entry_set_text (GTK_ENTRY (entry), text);
		else
			gtk_entry_set_text (GTK_ENTRY (entry), "");
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
	}
	entry->priv->current = sess;
}

void
text_entry_remove_session (TextEntry *entry, struct session *sess)
{
	g_hash_table_remove (entry->priv->entries, sess);
	if (sess == entry->priv->current)
		gtk_entry_set_text (GTK_ENTRY (entry), "");
}
