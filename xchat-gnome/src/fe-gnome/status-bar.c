/*
 * status-bar.c - Widget encapsulating the status bar
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

#include <config.h>
#include "status-bar.h"

static void status_bar_class_init (StatusBarClass *klass);
static void status_bar_init       (StatusBar      *bar);
static void status_bar_finalize   (GObject        *object);
static void status_bar_update     (StatusBar      *bar);

static GtkStatusbarClass *parent_class;
G_DEFINE_TYPE (StatusBar, status_bar, GTK_TYPE_STATUSBAR);

struct _StatusBarPriv
{
	GHashTable     *lags;
	GHashTable     *queues;

	struct server  *current;
	guint           context;
};

static void
status_bar_class_init (StatusBarClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = status_bar_finalize;
}

static void
status_bar_init (StatusBar *bar)
{
	bar->priv = g_new0 (StatusBarPriv, 1);
	bar->priv->lags   = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
	bar->priv->queues = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);

	bar->priv->context = gtk_statusbar_get_context_id (GTK_STATUSBAR (bar), "context");

	gtk_widget_show (GTK_WIDGET (bar));
}

static void
status_bar_finalize (GObject *object)
{
	StatusBar *bar;

	bar = STATUS_BAR (object);

	g_hash_table_destroy (bar->priv->lags);
	g_hash_table_destroy (bar->priv->queues);
	g_free (bar->priv);

	if (G_OBJECT_CLASS (parent_class)->finalize) {
		G_OBJECT_CLASS (parent_class)->finalize (object);
	}
}

GtkWidget *
status_bar_new (void)
{
	return GTK_WIDGET (g_object_new (status_bar_get_type (), NULL));
}

void
status_bar_set_lag (StatusBar *bar, struct server *sess, float seconds, gboolean sent)
{
	if (seconds == 0.0f) {
		g_hash_table_remove (bar->priv->lags, sess);
	} else {
		gchar *lag = g_strdup_printf (_("%s%.1fs lag"), sent ? "+" : "", seconds);
		g_hash_table_insert (bar->priv->lags, sess, lag);
	}

	if (bar->priv->current == sess)
		status_bar_update (bar);
}

void
status_bar_set_queue (StatusBar *bar, struct server *sess, int bytes)
{
	if (bytes == 0) {
		g_hash_table_remove (bar->priv->queues, sess);
	} else {
		gchar *queue = g_strdup_printf (_("%d bytes buffered"), bytes);
		g_hash_table_insert (bar->priv->queues, sess, queue);
	}

	if (bar->priv->current == sess) {
		status_bar_update (bar);
	}
}

void
status_bar_set_current (StatusBar *bar, struct server *sess)
{
	if (bar->priv->current == sess) {
		return;
	}

	bar->priv->current = sess;
	status_bar_update (bar);
}

void
status_bar_remove_server (StatusBar *bar, struct server *sess)
{
	if (bar->priv->current == sess) {
		status_bar_set_current (bar, NULL);
	}

	g_hash_table_remove (bar->priv->lags,   sess);
	g_hash_table_remove (bar->priv->queues, sess);
}

static void
status_bar_update (StatusBar *bar)
{
	if (bar->priv->current == NULL) {
		gtk_statusbar_pop  (GTK_STATUSBAR (bar), bar->priv->context);
		return;
	}

	gchar *lag   = g_hash_table_lookup (bar->priv->lags,   bar->priv->current);
	gchar *queue = g_hash_table_lookup (bar->priv->queues, bar->priv->current);

	gchar *text;
	if (lag && queue) {
		text = g_strdup_printf ("%s, %s", lag, queue);
	} else if (lag) {
		text = g_strdup (lag);
	} else if (queue) {
		text = g_strdup (queue);
	} else {
		text = g_strdup ("");
	}

	gtk_statusbar_pop  (GTK_STATUSBAR (bar), bar->priv->context);
	gtk_statusbar_push (GTK_STATUSBAR (bar), bar->priv->context, text);

	g_free (text);
}
