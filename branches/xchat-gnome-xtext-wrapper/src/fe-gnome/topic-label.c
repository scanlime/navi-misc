/*
 * topic-label.c - Widget encapsulating the topic label
 *
 * Copyright (C) 2005 xchat-gnome team
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
#include <glade/glade.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif
#include "text-entry.h"
#include "topic-label.h"
#include "../common/fe.h"
#include "../common/url.h"

static void  topic_label_class_init       (TopicLabelClass *klass);
static void  topic_label_init             (TopicLabel      *label);
static void  topic_label_finalize         (GObject         *object);
static void  topic_label_expand_activate  (GtkExpander     *expander,
                                           TopicLabel      *label);
static char *topic_label_get_topic_string (const char *topic);
#ifdef HAVE_LIBSEXY
static void  topic_label_url_activated    (GtkWidget       *url_label,
                                           const char      *url,
                                           gpointer         data);
#endif

struct _TopicLabelPriv
{
	GtkWidget      *expander;
	GtkWidget      *label;

	GHashTable     *topics;
	struct session *current;
};

static GtkHBoxClass *parent_class;

G_DEFINE_TYPE (TopicLabel, topic_label, GTK_TYPE_HBOX);

static void
topic_label_class_init (TopicLabelClass *klass)
{
	GObjectClass *gobject_class;

	parent_class = g_type_class_peek_parent (klass);

	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = topic_label_finalize;
}

static void
topic_label_init (TopicLabel *label)
{
	label->priv = g_new0 (TopicLabelPriv, 1);
	label->priv->expander = gtk_expander_new (NULL);
#ifdef HAVE_LIBSEXY
	label->priv->label    = sexy_url_label_new ();
#else
	label->priv->label    = gtk_label_new (NULL);
#endif

	gtk_expander_set_expanded (GTK_EXPANDER (label->priv->expander), FALSE);
	gtk_expander_set_use_markup (GTK_EXPANDER (label->priv->expander), TRUE);
	gtk_label_set_ellipsize (GTK_LABEL (label->priv->label), PANGO_ELLIPSIZE_END);
	gtk_label_set_selectable (GTK_LABEL (label->priv->label), TRUE);

	gtk_widget_show (label->priv->expander);
	gtk_widget_show (label->priv->label);
	gtk_widget_show (GTK_WIDGET (label));

	gtk_box_pack_start (GTK_BOX (label), label->priv->expander, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (label), label->priv->label,    TRUE,  TRUE, 0);
	gtk_box_set_spacing (GTK_BOX (label), 6);

	g_signal_connect (G_OBJECT (label->priv->expander), "activate",      G_CALLBACK (topic_label_expand_activate), (gpointer) label);
#ifdef HAVE_LIBSEXY
	g_signal_connect (G_OBJECT (label->priv->label),    "url_activated", G_CALLBACK (topic_label_url_activated),   NULL);
#endif

	label->priv->topics = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
}

static void
topic_label_finalize (GObject *object)
{
	if (G_OBJECT_CLASS (parent_class)->finalize)
		G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
topic_label_expand_activate (GtkExpander *expander, TopicLabel *label)
{
	if (gtk_expander_get_expanded (expander)) {
		gtk_label_set_ellipsize (GTK_LABEL (label->priv->label), PANGO_ELLIPSIZE_END);
		gtk_label_set_line_wrap (GTK_LABEL (label->priv->label), FALSE);
	} else {
		gtk_label_set_ellipsize (GTK_LABEL (label->priv->label), PANGO_ELLIPSIZE_NONE);
		gtk_label_set_line_wrap (GTK_LABEL (label->priv->label), TRUE);
	}
}

#ifdef HAVE_LIBSEXY
static void
topic_label_url_activated (GtkWidget *url_label, const char *url, gpointer data)
{
	fe_open_url (url);
}
#endif

GtkWidget *
topic_label_new (void)
{
	return GTK_WIDGET (g_object_new (topic_label_get_type (), NULL));
}

void
topic_label_set_topic (TopicLabel *label, struct session *sess, const char *topic)
{
	gchar *escaped;

	escaped = topic_label_get_topic_string (topic);

	g_hash_table_insert (label->priv->topics, sess, escaped);

	if (sess == label->priv->current)
		topic_label_set_current (label, sess);
}

void
topic_label_remove_session (TopicLabel *label, struct session *sess)
{
	g_hash_table_remove (label->priv->topics, sess);
	if (sess == label->priv->current)
		gtk_label_set_text (GTK_LABEL (label->priv->label), "");
}

void
topic_label_set_current (TopicLabel *label, struct session *sess)
{
	gchar *topic;

	topic = g_hash_table_lookup (label->priv->topics, sess);
#ifdef HAVE_LIBSEXY
	if (topic) sexy_url_label_set_markup (SEXY_URL_LABEL (label->priv->label), topic);
	else       gtk_label_set_text (GTK_LABEL (label->priv->label), "");
#else
	if (topic) gtk_label_set_text (GTK_LABEL (label->priv->label), topic);
	else       gtk_label_set_text (GTK_LABEL (label->priv->label), "");
#endif

	label->priv->current = sess;
}

static char *
topic_label_get_topic_string (const char *topic)
{
#ifdef HAVE_LIBSEXY
	/* FIXME: this probably isn't unicode-clean */
	gchar **tokens;
	gchar *escaped, *result, *temp;
	int i;

	if ((topic == NULL) || (strlen (topic) == 0))
		return NULL;

	/* escape out <>&"' so that pango markup doesn't get confused */
	escaped = g_markup_escape_text (topic, strlen (topic));

	/* surround urls with <a> markup so that sexy-url-label can link it */
	tokens = g_strsplit_set (escaped, " \t\n", 0);
	if (url_check_word (tokens[0], strlen (tokens[0])) == WORD_URL)
		result = g_strdup_printf ("<a href=\"%s\">%s</a>", tokens[0], tokens[0]);
	else
		result = g_strdup (tokens[0]);
	for (i = 1; tokens[i]; i++) {
		if (url_check_word (tokens[i], strlen (tokens[i])) == WORD_URL) {
			temp = g_strdup_printf ("%s <a href=\"%s\">%s</a>", result, tokens[i], tokens[i]);
			g_free (result);
			result = temp;
		} else {
			temp = g_strdup_printf ("%s %s", result, tokens[i]);
			g_free (result);
			result = temp;
		}
	}
	g_strfreev (tokens);
	g_free (escaped);
	return result;
#else
	return g_strdup (topic);
#endif
}

void
topic_label_change_current (TopicLabel *label)
{
	GladeXML *xml = NULL;
	GtkWidget *dialog, *entry;
	GtkTextBuffer *buffer;
	gchar *title, *topic;

	if ((label->priv->current == NULL) || (label->priv->current->type != SESS_CHANNEL))
		return;

	if (g_file_test ("../../topic-change.glade", G_FILE_TEST_EXISTS))
		xml = glade_xml_new ("../../topic-change.glade", NULL, NULL);
	else
		xml = glade_xml_new (XCHATSHAREDIR "/topic-change.glade", NULL, NULL);

	dialog = glade_xml_get_widget (xml, "topic change");
        entry  = glade_xml_get_widget (xml, "topic entry box");

	title = g_strdup_printf (_("Changing topic for %s"), label->priv->current->channel);
	gtk_window_set_title (GTK_WINDOW (dialog), title);
	g_free (title);

	topic = g_hash_table_lookup (label->priv->topics, label->priv->current);
	buffer = gtk_text_buffer_new (NULL);
        gtk_text_view_set_buffer (GTK_TEXT_VIEW (entry), buffer);
	if (topic)
        	gtk_text_buffer_set_text (buffer, topic, -1);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
		GtkTextIter start, end;
		gchar *newtopic;

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_widget_hide (dialog);
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (entry), GTK_WRAP_NONE);
		newtopic = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
		label->priv->current->server->p_topic (label->priv->current->server, label->priv->current->channel, newtopic);
		g_free (newtopic);
	}

	gtk_widget_destroy (dialog);
        g_object_unref (xml);
}
