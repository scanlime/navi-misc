/*
 * topiclabel.c - special widget for the topic label
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

#include "topiclabel.h"

struct _TopicLabelSelectionInfo
{
  GdkWindow *window;
  gint       selection_anchor;
  gint       selection_end;
  GtkWidget *popup_menu;
};

enum {
  MOVE_CURSOR,
  COPY_CLIPBOARD,
  POPULATE_POPUP,
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_JUSTIFY,
  PROP_DISPLAY,
};

static guint signals[LAST_SIGNAL] = { 0 };

static void      topic_label_class_init    (TopicLabelClass *klass);
static void      topic_label_init          (TopicLabel      *label);
static void      topic_label_set_property  (GObject         *object,
                                            guint            prop_id,
                                            const GValue    *value,
                                            GParamSpec      *pspec);
static void      topic_label_get_property  (GObject         *object,
                                            guint            prop_id,
                                            GValue          *value,
                                            GParamSpec      *pspec);
static void      topic_label_destroy       (GtkObject       *object);
static void      topic_label_finalize      (GObject         *object);
static void      topic_label_size_request  (GtkWidget       *widget,
                                            GtkRequisition  *requisition);
static void      topic_label_size_allocate (GtkWidget       *widget,
                                            GtkRequisition  *requisition);
/* ... */
static void     topic_label_expose         (GtkWidget       *widget,
                                            GdkEventExpose  *event);
static void     topic_label_realize        (GtkWidget       *widget);
static void     topic_label_unrealize      (GtkWidget       *widget);
static void     topic_label_map            (GtkWidget       *widget);
static void     topic_label_unmap          (GtkWidget       *widget);
static gboolean topic_label_button_press   (GtkWidget       *widget,
                                            GdkEventButton  *event);
static gboolean topic_label_button_release (GtkWidget       *widget,
                                            GdkEventButton  *event);
static gboolean topic_label_motion         (GtkWidget       *widget,
                                            GdkEventMotion  *event);
/* ... */
/* for selectability */
static void topic_label_move_cursor        (TopicLabel      *label,
                                            GtkMovementStep  step,
                                            gint             count,
                                            gboolean         extend_selection);
static void topic_label_copy_clipboard     (TopicLabel      *label);
static void topic_label_select_all         (TopicLabel      *label);
static void topic_label_do_popup           (TopicLabel      *label,
                                            GdkEventButton  *event);
static void topic_label_move_forward_word  (TopicLabel      *label,
                                            gint             start);
static void topic_label_move_backward_word (TopicLabel      *label,
                                            gint             start);

static GtkMiscClass *parent_class = NULL;

GType
topic_label_get_type (void)
{
  static GType topic_label_type = 0;

  if (!topic_label_type)
  {
    static const GTypeInfo topic_label_info =
    {
      sizeof (TopicLabelClass),
      NULL,               /* base_init */
      NULL,               /* base_finalize */
      (GClassInitFunc)    topic_label_class_init,
      NULL,               /* class_finalize */
      NULL,               /* class_data */
      sizeof (TopicLabel),
      0,                  /* n_preallocs */
      (GInstanceInitFunc) topic_label_init,
    };
  }

  return topic_label_type;
}

static void
topic_label_class_init (TopicLabelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  parent_class = g_type_class_peek_parent (class);

  gobject_class->set_property = topic_label_set_property;
  gobject_class->get_property = topic_label_get_property;
}

static void
topic_label_init (TopicLabel *label)
{
  GTK_WIDGET_SET_FLAGS (label, GTK_NO_WINDOW);

  label->label = NULL;

  label->jtype = GTK_JUSTIFY_FILL;
  label->display = TOPIC_LABEL_DISPLAY_FULL;

  label->layout = NULL;
  label->text = NULL;
  label->attrs = NULL;

  topic_label_set_text (label, "");
}

static void
topic_label_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
}

static void
topic_label_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
}

static void
topic_label_destroy (GtkObject *object)
{
}

static void
topic_label_finalize (GObject *object)
{
}

static void
topic_label_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
}

static void
topic_label_size_allocate (GtkWidget *widget, GtkRequisition *requisition)
{
}

static void
topic_label_expose (GtkWidget *widget, GdkEventExpose  *event)
{
}

static void
topic_label_realize (GtkWidget *widget)
{
}

static void
topic_label_unrealize (GtkWidget *widget)
{
}

static void
topic_label_map (GtkWidget *widget)
{
}

static void
topic_label_unmap (GtkWidget *widget)
{
}

static gboolean
topic_label_button_press (GtkWidget *widget, GdkEventButton *event)
{
}

static gboolean
topic_label_button_release (GtkWidget *widget, GdkEventButton *event)
{
}

static gboolean
topic_label_motion (GtkWidget *widget, GdkEventMotion  *event)
{
}

static void
topic_label_move_cursor (TopicLabel *label, GtkMovementStep step, gint count, gboolean extend_selection)
{
}

static void
topic_label_copy_clipboard (TopicLabel *label)
{
}

static void
topic_label_select_all (TopicLabel *label)
{
}

static void
topic_label_do_popup (TopicLabel *label, GdkEventButton *event)
{
}

static void
topic_label_move_forward_word (TopicLabel *label, gint start)
{
}

static void
topic_label_move_backward_word (TopicLabel *label, gint start)
{
}
