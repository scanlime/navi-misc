/*
 * completion-store.c - a subclass of GtkListStore that also keeps track of
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

#include "completion-store.h"
#include <gtk/gtk.h>

static void completion_store_class_init(CompletionStoreClass *klass);
static void completion_store_init(CompletionStore *self);

GType completion_store_get_type(void) {
	static GType cs_type = 0;

	if (!cs_type) {
		static const GTypeInfo cs_info = {
			sizeof(CompletionStoreClass),
			NULL, /* base init */
			NULL, /* base finalize */
			(GClassInitFunc) completion_store_class_init,
			NULL, /* class finalize */
			NULL, /* class data */
			sizeof(CompletionStore),
			0,
			(GInstanceInitFunc) completion_store_init,
		};

		cs_type = g_type_register_static(GTK_TYPE_LIST_STORE, "CompletionStore", &cs_info, 0);
	}
	return cs_type;
}

static void completion_store_class_init(CompletionStoreClass *klass) {
}

static void completion_store_init(CompletionStore *self) {
	self->list = NULL;
	self->completion = g_completion_new(NULL);
	self->completion_column = -1;
}

CompletionStore* completion_store_new(gint n_columns, ...) {
	return NULL;
}

CompletionStore* completion_store_newv(gint n_columns, GType *types) {
	return NULL;
}

GCompletion *completion_store_get_completion(CompletionStore *cs) {
	return NULL;
}

void completion_store_set(CompletionStore *cs, GtkTreeIter *iter, ...) {
}

void completion_store_set_valist(CompletionStore *cs, GtkTreeIter *iter, va_list var_args) {
}

void completion_store_set_value(CompletionStore *cs, GtkTreeIter *iter, gint column, GValue *value) {
}

void completion_store_clear(CompletionStore *cs) {
}

gboolean completion_store_set_completion_column(CompletionStore *cs, gint column) {
	/* FIXME: check type of column? does it matter? */
	if(cs->completion_column != column) {
		GtkTreeIter iter;
		GValue value;

		cs->completion_column = column;
		g_completion_clear_items(cs->completion);

		if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(&cs->store), &iter))
			return TRUE;
		do {
			gtk_tree_model_get_value(GTK_TREE_MODEL(&cs->store), &iter, cs->completion_column, &value);
			g_list_append(cs->list, g_value_peek_pointer(&value));
		} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(&cs->store), &iter));
	}
	return TRUE;
}

void completion_store_set_compare(CompletionStore *cs, GCompletionStrncmpFunc strncmp_func) {
	g_completion_set_compare(cs->completion, strncmp_func);
}
