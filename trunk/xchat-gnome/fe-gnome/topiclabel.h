/*
 * topiclabel.h - special widget for the topic label
 *
 * Copyright (C) 2004 David Trowbridge
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

#ifndef __XCHAT_GNOME_TOPIC_LABEL_H__
#define __XCHAT_GNOME_TOPIC_LABEL_H__

#include <gtk/gtk.h>

#define TOPIC_LABEL_TYPE            (topic_label_get_type ())
#define TOPIC_LABEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TOPIC_LABEL_TYPE, TopicLabel))
#define TOPIC_LABEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TOPIC_LABEL_TYPE, TopicLabelClass))
#define IS_TOPIC_LABEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TOPIC_LABEL_TYPE))
#define IS_TOPIC_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TOPIC_LABEL_TYPE))
#define TOPIC_LABEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TOPIC_LABEL_TYPE, TopicLabelClass))

typedef struct _TopicLabel      TopicLabel;
typedef struct _TopicLabelClass TopicLabelClass;

typedef struct _TopicLabelSelectionInfo TopicLabelSelectionInfo;

typedef enum
{
  TOPIC_LABEL_DISPLAY_FULL,
  TOPIC_LABEL_DISPLAY_ABRIDGED,
} TopicLabelDisplayType;

struct _TopicLabel
{
  GtkMisc        misc;

  /* private */
  gchar         *text;
  gchar         *label;
  guint          jtype : 2;
  gboolean       wrap : TRUE;
  PangoAttrList *attrs;
  PangoAttrList *effective_attrs;
  PangoLayout   *layout;

  TopicLabelSelectionInfo *select_info;
  TopicLabelDisplayType display;
};

struct _TopicLabelClass
{
  GtkMiscClass parent_class;

  void (* move_cursor)    (TopicLabel      *label,
                           GtkMovementStep  step,
                           gint             count,
                           gboolean         extend_selection);
  void (* copy_clipboard) (TopicLabel      *label);
  void (* populate_popup) (TopicLabel      *label,
                           GtkMenu         *menu);
};

GType      topic_label_get_type    (void) G_GNUC_CONST;
GtkWidget* topic_label_new         ();
void       topic_label_set_text    (TopicLabel *label, const gchar *str);
void       topic_label_set_display (TopicLabel *label, TopicLabelDisplayType type);

#endif
