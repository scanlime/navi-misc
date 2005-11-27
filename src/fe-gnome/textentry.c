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
#include "textentry.h"

static void text_entry_class_init (TextEntryClass *klass);
static void text_entry_init       (TextEntry      *entry);
static void text_entry_finalize   (GObject        *object);

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
#endif
}

static void
text_entry_finalize (GObject *object)
{
	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

GtkWidget *
text_entry_new (void)
{
	return GTK_WIDGET (g_object_new (text_entry_get_type (), NULL));
}
