/*
 * completion-store.h - a subclass of GtkListStore that also keeps track of
 *			its contents as a GCompletion
 *
 * Copyright (C) 2004 David Trowbridge and Dan Kuester
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

#ifndef COMPLETION_STORE_H
#define COMPLETION_STORE_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkliststore.h>

G_BEGIN_DECLS

#define COMPLETION_STORE_TYPE			(completion_store_get_type())
#define COMPLETION_STORE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), COMPLETION_STORE_TYPE, CompletionStore))
#define COMPLETION_STORE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), COMPLETION_STORE_TYPE, CompletionStore))
#define IS_COMPLETION_STORE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), COMPLETION_STORE_TYPE))
#define IS_COMPLETION_STORE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), COMPLETION_STORE_TYPE))

typedef struct _CompletionStore      CompletionStore;
typedef struct _CompletionStoreClass CompletionStoreClass;

struct _CompletionStore {
	GtkListStore store;
	GList *list;
	GCompletion *completion;
	gint completion_column;
};

struct _CompletionStoreClass {
	GtkListStoreClass parent_class;
};

GType			completion_store_get_type(void);
CompletionStore*	completion_store_new(gint n_columns, ...);
CompletionStore*	completion_store_newv(gint n_columns, GType *types);
GCompletion*		completion_store_get_completion(CompletionStore *cs);
void			completion_store_set(CompletionStore *cs, GtkTreeIter *iter, ...);
void			completion_store_set_valist(CompletionStore *cs, GtkTreeIter *iter, va_list var_args);
void			completion_store_set_value(CompletionStore *cs, GtkTreeIter *iter, gint column, GValue *value);
void			completion_store_clear(CompletionStore *cs);
gboolean		completion_store_set_completion_column(CompletionStore *cs, gint column);
void			completion_store_set_compare(CompletionStore *cs, GCompletionStrncmpFunc strncmp_func);

G_END_DECLS

#endif /* COMPLETION_STORE_H */
