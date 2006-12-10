/*
 * topic-label.c - Widget encapsulating the topic label
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
#include <glade/glade.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif
#include "gui.h"
#include "util.h"
#include "text-entry.h"
#include "topic-label.h"
#include "../common/fe.h"
#include "../common/url.h"
#include "../common/outbound.h"

static void  topic_label_class_init       (TopicLabelClass *klass);
static void  topic_label_init             (TopicLabel      *label);
static void  topic_label_finalize         (GObject         *object);
static void  topic_label_expand_activate  (GtkExpander     *expander,
                                           TopicLabel      *label);
static char *topic_label_get_topic_string (const char      *topic);
static void  topic_label_size_allocate    (GtkWidget       *widget,
                                           GtkAllocation   *allocation);
#ifdef HAVE_LIBSEXY
static void  topic_label_url_activated    (GtkWidget       *url_label,
                                           const char      *url,
                                           TopicLabel      *label);
#endif
static void  topic_entry_activate         (GtkTextBuffer   *textbuffer,
                                           GtkTextIter     *arg1,
                                           gchar           *text,
                                           gint             len,
                                           GtkDialog       *dialog);

#define TOPIC_LABEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TOPIC_LABEL_TYPE, TopicLabelPriv))

struct _TopicLabelPriv
{
	GtkWidget      *expander;
	GtkWidget      *label;
	GtkWidget      *sizing_label;

	GHashTable     *topics;
	struct session *current;
};

G_DEFINE_TYPE (TopicLabel, topic_label, GTK_TYPE_HBOX);

static void
topic_label_class_init (TopicLabelClass *klass)
{
	GObjectClass   *gobject_class;
	GtkWidgetClass *widget_class;

	gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->finalize = topic_label_finalize;

	widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->size_allocate = topic_label_size_allocate;

	g_type_class_add_private (gobject_class, sizeof (TopicLabelPriv));
}

static void
topic_label_init (TopicLabel *label)
{
	TopicLabelPriv *priv;

	priv = label->priv = TOPIC_LABEL_GET_PRIVATE (label);
	priv->expander = gtk_expander_new (NULL);
#ifdef HAVE_LIBSEXY
	priv->label = sexy_url_label_new ();
	priv->sizing_label = sexy_url_label_new ();
#else
	priv->label = gtk_label_new (NULL);
	priv->sizing_label = gtk_label_new (NULL);
#endif
	gtk_expander_set_expanded (GTK_EXPANDER (priv->expander), FALSE);
	gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
	gtk_label_set_selectable (GTK_LABEL (priv->label), TRUE);
	gtk_misc_set_alignment (GTK_MISC (priv->label), 0.0, 0.0);

	gtk_widget_show (priv->expander);
	gtk_widget_show (priv->label);
	gtk_widget_show (GTK_WIDGET (label));

	gtk_box_pack_start (GTK_BOX (label), priv->expander, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (label), priv->label,    TRUE,  TRUE, 0);
	gtk_box_set_spacing (GTK_BOX (label), 6);

	g_signal_connect (G_OBJECT (priv->expander), "activate", G_CALLBACK (topic_label_expand_activate), (gpointer) label);
#ifdef HAVE_LIBSEXY
	g_signal_connect (G_OBJECT (priv->label), "url_activated", G_CALLBACK (topic_label_url_activated), (gpointer) label);
#endif

	priv->topics = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, g_free);
}

static void
topic_label_finalize (GObject *object)
{
	TopicLabel *label = TOPIC_LABEL (object);
	TopicLabelPriv *priv = label->priv;

	g_hash_table_destroy (priv->topics);

	G_OBJECT_CLASS (topic_label_parent_class)->finalize (object);
}

static void
topic_label_expand_activate (GtkExpander *expander, TopicLabel *label)
{
	TopicLabelPriv *priv = label->priv;

	if (gtk_expander_get_expanded (expander)) {
		gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_END);
		gtk_label_set_line_wrap (GTK_LABEL (priv->label), FALSE);
	} else {
		gtk_label_set_ellipsize (GTK_LABEL (priv->label), PANGO_ELLIPSIZE_NONE);
		gtk_label_set_line_wrap (GTK_LABEL (priv->label), TRUE);
	}
}

#ifdef HAVE_LIBSEXY
static void
topic_label_url_activated (GtkWidget *url_label, const char *url, TopicLabel *label)
{
	TopicLabelPriv *priv = label->priv;

	char *command;
	command = g_strdup_printf ("URL %s", url);
	handle_command (priv->current, command, 1);
	g_free (command);
}
#endif

GtkWidget *
topic_label_new (void)
{
	return g_object_new (TOPIC_LABEL_TYPE, NULL);
}

void
topic_label_set_topic (TopicLabel *label, struct session *sess, const char *topic)
{
	TopicLabelPriv *priv = label->priv;
	gchar *escaped;

	escaped = topic_label_get_topic_string (topic);

	g_hash_table_insert (priv->topics, sess, escaped);

	if (sess == priv->current) {
		topic_label_set_current (label, sess);
	}
}

void
topic_label_remove_session (TopicLabel *label, struct session *sess)
{
	TopicLabelPriv *priv = label->priv;

	g_hash_table_remove (priv->topics, sess);
	if (sess == priv->current) {
		gtk_label_set_text (GTK_LABEL (priv->label), "");
	}
}

void
topic_label_set_current (TopicLabel *label, struct session *sess)
{
	TopicLabelPriv *priv = label->priv;
	gchar *topic;

	if (sess) {
		topic = g_hash_table_lookup (priv->topics, sess);
	} else {
		topic = NULL;
	}


#ifdef HAVE_LIBSEXY
	if (topic) {
		sexy_url_label_set_markup (SEXY_URL_LABEL (priv->label), topic);
		sexy_url_label_set_markup (SEXY_URL_LABEL (priv->sizing_label), topic);
	} else {
		gtk_label_set_text (GTK_LABEL (priv->label), "");
		gtk_label_set_text (GTK_LABEL (priv->sizing_label), "");
	}
#else
	if (topic) {
		gtk_label_set_text (GTK_LABEL (priv->label), topic);
		gtk_label_set_text (GTK_LABEL (priv->sizing_label), topic);
	} else {
		gtk_label_set_text (GTK_LABEL (priv->label), "");
		gtk_label_set_text (GTK_LABEL (priv->sizing_label), "");
	}
#endif

	if (sess == NULL || sess->type == SESS_SERVER) {
		gtk_widget_hide (gui.topic_hbox);
	} else {
		gtk_widget_show (gui.topic_hbox);
	}

	priv->current = sess;
}

static char *
topic_label_get_topic_string (const char *topic)
{
#ifdef HAVE_LIBSEXY
	/* FIXME: this probably isn't unicode-clean */
	gchar **tokens;
	gchar *escaped, *result, *temp;
	int i;

	if ((topic == NULL) || (strlen (topic) == 0)) {
		return NULL;
	}

	/* escape out <>&"' so that pango markup doesn't get confused */
	escaped = g_markup_escape_text (topic, strlen (topic));

	/* surround urls with <a> markup so that sexy-url-label can link it */
	tokens = g_strsplit_set (escaped, " \t\n", 0);
	if (url_check_word (tokens[0], strlen (tokens[0])) == WORD_URL) {
		result = g_strdup_printf ("<a href=\"%s\">%s</a>", tokens[0], tokens[0]);
	} else {
		result = g_strdup (tokens[0]);
	}

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
	TopicLabelPriv *priv = label->priv;
	GladeXML *xml = NULL;
	GtkWidget *dialog, *entry;
	GtkTextBuffer *buffer;
	gchar *title, *topic, *path;

	if ((priv->current == NULL) || (priv->current->type != SESS_CHANNEL)) {
		return;
	}

	path = locate_data_file ("topic-change.glade");
	xml = glade_xml_new (path, NULL, NULL);
	g_free (path);

	dialog = glade_xml_get_widget (xml, "topic change");
        entry  = glade_xml_get_widget (xml, "topic entry box");

	title = g_strdup_printf (_("Changing topic for %s"), priv->current->channel);
	gtk_window_set_title (GTK_WINDOW (dialog), title);
	g_free (title);

	topic = priv->current->topic;
	buffer = gtk_text_buffer_new (NULL);
        gtk_text_view_set_buffer (GTK_TEXT_VIEW (entry), buffer);
	g_signal_connect (G_OBJECT (buffer), "insert-text", G_CALLBACK (topic_entry_activate), dialog);
	if (topic) {
        	gtk_text_buffer_set_text (buffer, topic, -1);
	}

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
		GtkTextIter start, end;
		gchar *newtopic;

		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_widget_hide (dialog);
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (entry), GTK_WRAP_NONE);
		newtopic = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
		priv->current->server->p_topic (priv->current->server, priv->current->channel, newtopic);
		g_free (newtopic);
	}

	gtk_widget_destroy (dialog);
        g_object_unref (xml);
}

static void
topic_label_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	TopicLabel     *label;
	TopicLabelPriv *priv;
	GtkRequisition  request;

	GTK_WIDGET_CLASS (topic_label_parent_class)->size_allocate (widget, allocation);

	label = TOPIC_LABEL (widget);
	priv = label->priv;

	gtk_widget_size_request (priv->sizing_label, &request);

	if (request.width < allocation->width - 6) {
		gtk_widget_hide (priv->expander);
	} else {
		gtk_widget_show (priv->expander);
	}
}

static void
topic_entry_activate (GtkTextBuffer *textbuffer, GtkTextIter *arg1, gchar *text, gint len, GtkDialog *dialog)
{
	if (strncmp (text, "\n", len) == 0) {
		gtk_dialog_response (dialog, GTK_RESPONSE_OK);
	}
}
