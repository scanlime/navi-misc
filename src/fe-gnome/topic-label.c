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
#include <gtk/gtk.h>
#include <string.h>
#ifdef HAVE_LIBSEXY
#include <libsexy/sexy-url-label.h>
#endif
#include "topic-label.h"
#include "../common/fe.h"
#include "../common/url.h"

static void topic_label_class_init      (TopicLabelClass *klass);
static void topic_label_init            (TopicLabel      *label);
static void topic_label_finalize        (GObject         *object);
static void topic_label_expand_activate (GtkExpander     *expander,
                                         TopicLabel      *label);
static void topic_label_url_activated   (GtkWidget       *url_label,
                                         const char      *url,
                                         gpointer         data);

struct _TopicLabelPriv
{
	GtkWidget *expander;
	GtkWidget *label;
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

	gtk_box_pack_start (GTK_BOX (label), label->priv->expander, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (label), label->priv->label,    TRUE,  TRUE, 0);
	gtk_box_set_spacing (GTK_BOX (label), 6);

	g_signal_connect (G_OBJECT (label->priv->expander), "activate",      G_CALLBACK (topic_label_expand_activate), (gpointer) label);
	g_signal_connect (G_OBJECT (label->priv->label),    "url_activated", G_CALLBACK (topic_label_url_activated),   NULL);
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

static void
topic_label_url_activated (GtkWidget *url_label, const char *url, gpointer data)
{
	fe_open_url (url);
}

GtkWidget *
topic_label_new (void)
{
	return GTK_WIDGET (g_object_new (topic_label_get_type (), NULL));
}

void
topic_label_set_topic (TopicLabel *label, const char *topic)
{
#ifdef HAVE_LIBSEXY
	sexy_url_label_set_markup (SEXY_URL_LABEL (label->priv->label), topic);
#else
	gtk_label_set_text (GTK_LABEL (label->priv->label), topic);
#endif
}

char *
topic_label_get_topic_string (const char *topic)
{
#ifdef HAVE_LIBSEXY
	/* FIXME: this probably isn't unicode-clean */
	gchar **tokens;
	gchar *escaped, *result, *temp;
	int i;

	if (strlen (topic) == 0)
		return g_strdup (topic);

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
