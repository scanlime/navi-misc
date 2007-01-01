/*
 * textentry.h - Widget encapsulating the text entry
 *
 * Copyright (C) 2004-2007 xchat-gnome team
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
#ifndef XCHAT_GNOME_TEXTENTRY_H
#define XCHAT_GNOME_TEXTENTRY_H

#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-spell-entry.h>
#else
#include <gtk/gtkentry.h>
#endif
#include "../common/xchat.h"

G_BEGIN_DECLS

typedef struct _TextEntry      TextEntry;
typedef struct _TextEntryClass TextEntryClass;
typedef struct _TextEntryPriv  TextEntryPriv;

#define TEXT_ENTRY_TYPE            (text_entry_get_type ())
#define TEXT_ENTRY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEXT_ENTRY_TYPE, TextEntry))
#define TEXT_ENTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TEXT_ENTRY_TYPE, TextEntryClass))
#define IS_TEXT_ENTRY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEXT_ENTRY_TYPE))
#define IS_TEXT_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEXT_ENTRY_TYPE))

struct _TextEntry
{
#ifdef HAVE_LIBSEXY
	SexySpellEntry  parent;
#else
	GtkEntry        parent;
#endif

	TextEntryPriv  *priv;
};

struct _TextEntryClass
{
#ifdef HAVE_LIBSEXY
	SexySpellEntryClass parent_class;
#else
	GtkEntryClass       parent_class;
#endif
};

GType      text_entry_get_type       (void) G_GNUC_CONST;
GtkWidget *text_entry_new            (void);
void       text_entry_set_current    (TextEntry *entry, struct session *sess);
void       text_entry_remove_session (TextEntry *entry, struct session *sess);

G_END_DECLS

#endif
