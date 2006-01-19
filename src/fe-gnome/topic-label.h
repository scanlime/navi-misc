/*
 * topic-label.h - Widget encapsulating the topic label
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
#ifndef XCHAT_GNOME_TOPIC_LABEL_H
#define XCHAT_GNOME_TOPIC_LABEL_H

#include <gtk/gtkhbox.h>
#include "../common/xchat.h"

G_BEGIN_DECLS

typedef struct _TopicLabel      TopicLabel;
typedef struct _TopicLabelClass TopicLabelClass;
typedef struct _TopicLabelPriv  TopicLabelPriv;

#define TOPIC_LABEL_TYPE            (topic_label_get_type ())
#define TOPIC_LABEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TOPIC_LABEL_TYPE, TopicLabel))
#define TOPIC_LABEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TOPIC_LABEL_TYPE, TopicLabelClass))
#define IS_TOPIC_LABEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TOPIC_LABEL_TYPE))
#define IS_TOPIC_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TOPIC_LABEL_TYPE))

struct _TopicLabel
{
	GtkHBox parent;

	TopicLabelPriv *priv;
};

struct _TopicLabelClass
{
	GtkHBoxClass parent_class;
};

GType      topic_label_get_type         (void) G_GNUC_CONST;
GtkWidget *topic_label_new              (void);
void       topic_label_set_topic        (TopicLabel *label, struct session *sess, const char *topic);
void       topic_label_remove_session   (TopicLabel *label, struct session *sess);
void       topic_label_set_current      (TopicLabel *label, struct session *sess);
void       topic_label_change_current   (TopicLabel *label);

G_END_DECLS

#endif
