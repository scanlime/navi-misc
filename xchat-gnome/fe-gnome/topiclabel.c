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

static void      topic_label_class_init     (TopicLabelClass  *klass);
static void      topic_label_init           (TopicLabel       *label);
static void      topic_label_set_property   (GObject          *object,
                                             guint             prop_id,
                                             const GValue     *value,
                                             GParamSpec       *pspec);
static void      topic_label_get_property   (GObject          *object,
                                             guint             prop_id,
                                             GValue           *value,
                                             GParamSpec       *pspec);
static void      topic_label_destroy        (GtkObject        *object);
static void      topic_label_finalize       (GObject          *object);
static void      topic_label_size_request   (GtkWidget        *widget,
                                             GtkRequisition   *requisition);
static void      topic_label_size_allocate  (GtkWidget        *widget,
                                             GtkAllocation    *allocation);
/* ... */
static gint     topic_label_expose          (GtkWidget        *widget,
                                             GdkEventExpose   *event);
static void     topic_label_realize         (GtkWidget        *widget);
static void     topic_label_unrealize       (GtkWidget        *widget);
static void     topic_label_map             (GtkWidget        *widget);
static void     topic_label_unmap           (GtkWidget        *widget);
static gboolean topic_label_button_press    (GtkWidget        *widget,
                                             GdkEventButton   *event);
static gboolean topic_label_button_release  (GtkWidget        *widget,
                                             GdkEventButton   *event);
static gboolean topic_label_motion          (GtkWidget        *widget,
                                             GdkEventMotion   *event);
/* ... */
/* for selectability */
static void topic_label_move_cursor         (TopicLabel       *label,
                                             GtkMovementStep   step,
                                             gint              count,
                                             gboolean          extend_selection);
static void topic_label_copy_clipboard      (TopicLabel       *label);
static void topic_label_select_all          (TopicLabel       *label);
static void topic_label_do_popup            (TopicLabel       *label,
                                             GdkEventButton   *event);
static void topic_label_move_forward_word   (TopicLabel       *label,
                                             gint              start);
static void topic_label_move_backward_word  (TopicLabel       *label,
                                             gint              start);

static void topic_label_create_window       (TopicLabel       *label);
static void topic_label_destroy_window      (TopicLabel       *label);
static void topic_label_clear_layout        (TopicLabel       *label);
static void topic_label_ensure_layout       (TopicLabel       *label);

static void topic_label_recalculate         (TopicLabel       *label);
static void topic_label_set_label_internal  (TopicLabel       *label,
                                             gchar            *str);
static void topic_label_set_text_internal   (TopicLabel       *label,
                                             gchar            *str);
static void topic_label_select_region_index (TopicLabel       *label,
                                             gint              anchor_index,
                                             gint              end_index);
static void get_text_callback               (GtkClipboard     *clipboard,
                                             GtkSelectionData *selection_data,
                                             guint             info,
                                             gpointer          user_data_or_owner);
static void clear_text_callback             (GtkClipboard     *clipboard,
                                             gpointer          user_data_or_owner);

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

    topic_label_type = g_type_register_static (GTK_TYPE_MISC, "TopicLabel", &topic_label_info, 0);
  }

  return topic_label_type;
}

static void
topic_label_class_init (TopicLabelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->set_property = topic_label_set_property;
  gobject_class->get_property = topic_label_get_property;
  gobject_class->finalize = topic_label_finalize;

  object_class->destroy = topic_label_destroy;

  widget_class->size_request = topic_label_size_request;
  widget_class->size_allocate = topic_label_size_allocate;
  widget_class->expose_event = topic_label_expose;
  widget_class->realize = topic_label_realize;
  widget_class->unrealize = topic_label_unrealize;
  widget_class->map = topic_label_map;
  widget_class->unmap = topic_label_unmap;
  widget_class->button_press_event = topic_label_button_press;
  widget_class->button_release_event = topic_label_button_release;
  widget_class->motion_notify_event = topic_label_motion;
}

static void
topic_label_init (TopicLabel *label)
{
  GTK_WIDGET_SET_FLAGS (label, GTK_NO_WINDOW);

  label->text = NULL;

  label->jtype = GTK_JUSTIFY_FILL;
  label->display = TOPIC_LABEL_DISPLAY_FULL;

  label->layout = NULL;
  label->text = NULL;
  label->attrs = NULL;
// FIXME
//  topic_label_set_text (label, "");
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
  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
topic_label_finalize (GObject *object)
{
  TopicLabel *label;
  g_return_if_fail (IS_TOPIC_LABEL (object));
  label = TOPIC_LABEL (object);

  g_free (label->text);

  if (label->layout)
    g_object_unref (label->layout);

  if (label->attrs)
    pango_attr_list_unref (label->attrs);

  if (label->effective_attrs)
    pango_attr_list_unref (label->effective_attrs);

  g_free (label->select_info);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
topic_label_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  TopicLabel *label;
  gint width, height;
  PangoRectangle logical_rect;

  g_return_if_fail (IS_TOPIC_LABEL (widget));
  g_return_if_fail (requisition != NULL);

  label = TOPIC_LABEL (widget);

  /*
   * If display is set to FULL, the height requisition can depend on:
   *   - any width set on the widget via gtk_widget_set_usize()
   *   - the padding of the widget (xpand, set by gtk_misc_set_padding)
   *
   * Instead of trying to detect changes to these quantities, if we are
   * wrapping, we just rewrap for each size request. Since size
   * requisitions are cached by the GTK+ core, this is not expensive.
   */

  if (label->wrap)
    topic_label_clear_layout (label);

  topic_label_ensure_layout (label);

  width = label->misc.xpad * 2;
  height = label->misc.ypad * 2;

  pango_layout_get_extents (label->layout, NULL, &logical_rect);

  width = label->misc.xpad * 2;
  width += PANGO_PIXELS (logical_rect.width);

  height = label->misc.ypad * 2;
  height += PANGO_PIXELS (logical_rect.height);

  requisition->width = width;
  requisition->height = height;
}

static void
topic_label_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  TopicLabel *label;
  g_return_if_fail (IS_TOPIC_LABEL (widget));
  label = TOPIC_LABEL (widget);

  (* GTK_WIDGET_CLASS (parent_class)->size_allocate) (widget, allocation);

  if (label->select_info && label->select_info->window)
  {
    gdk_window_move_resize (label->select_info->window, allocation->x, allocation->y, allocation->width, allocation->height);
  }
}

static gint
topic_label_expose (GtkWidget *widget, GdkEventExpose  *event)
{
  TopicLabel *label;
  gint x, y;

  g_return_val_if_fail (IS_TOPIC_LABEL (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  label = TOPIC_LABEL (widget);

  /* layout? */

  if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_MAPPED (widget) && label->text && (label->text[0] != '\0'))
  {
    /* ... */
  }
  return FALSE;
}

static void
topic_label_realize (GtkWidget *widget)
{
  TopicLabel *label;
  label = TOPIC_LABEL (widget);

  (* GTK_WIDGET_CLASS (parent_class)->realize) (widget);

  if (label->select_info)
    topic_label_create_window (label);
}

static void
topic_label_unrealize (GtkWidget *widget)
{
  TopicLabel *label;
  label = TOPIC_LABEL (widget);

  /*
  if (label->select_info)
    topic_label_destroy_window (label);
  */

  (* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

static void
topic_label_map (GtkWidget *widget)
{
  TopicLabel *label;
  label = TOPIC_LABEL (widget);

  (* GTK_WIDGET_CLASS (parent_class)->map) (widget);

  if (label->select_info)
    gdk_window_show (label->select_info->window);
}

static void
topic_label_unmap (GtkWidget *widget)
{
  TopicLabel *label;
  label = TOPIC_LABEL (widget);

  if (label->select_info)
    gdk_window_hide (label->select_info->window);

  (* GTK_WIDGET_CLASS (parent_class)->unmap) (widget);
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

static void
topic_label_create_window (TopicLabel *label)
{
}

static void
topic_label_destroy_window (TopicLabel *label)
{
}

static void
topic_label_clear_layout (TopicLabel *label)
{
  if (label->layout)
  {
    g_object_unref (label->layout);
    label->layout = NULL;
  }
}

static void
topic_label_ensure_layout (TopicLabel *label)
{
  GtkWidget *widget;
  PangoRectangle logical_rect;
  gint rwidth, rheight;
  gboolean rtl;

  widget = GTK_WIDGET (label);

  rtl = (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL);
  rwidth = label->misc.xpad * 2;
  rheight = label->misc.ypad * 2;

  if (!label->layout)
  {
    PangoAlignment align = PANGO_ALIGN_LEFT;

    label->layout = gtk_widget_create_pango_layout (widget, label->text);

    if (label->effective_attrs)
      pango_layout_set_attributes (label->layout, label->effective_attrs);

    switch (label->jtype)
    {
      case GTK_JUSTIFY_LEFT:
        align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
        break;
      case GTK_JUSTIFY_RIGHT:
        align = rtl ? PANGO_ALIGN_LEFT : PANGO_ALIGN_RIGHT;
        break;
      case GTK_JUSTIFY_CENTER:
        align = PANGO_ALIGN_CENTER;
        break;
      case GTK_JUSTIFY_FILL:
        /* FIXME: this just doesn't work to do this */
        align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
        pango_layout_set_justify (label->layout, TRUE);
        break;
      default:
        g_assert_not_reached();
    }

    if (label->wrap)
    {
      GtkWidgetAuxInfo *aux_info;
      gint longest_paragraph;
      gint width, height;
    }
  }
}

static void
topic_label_recalculate (TopicLabel *label)
{
  topic_label_set_text_internal (label, g_strdup (label->label));
  if (label->attrs)
    pango_attr_list_ref (label->attrs);
  if (label->effective_attrs)
    pango_attr_list_unref (label->effective_attrs);
  label->effective_attrs = label->attrs;

  topic_label_clear_layout (label);
  gtk_widget_queue_resize (GTK_WIDGET (label));
}

static void
topic_label_set_label_internal (TopicLabel *label, gchar *str)
{
  g_free (label->label);

  label->label = str;

  g_object_notify (G_OBJECT (label), "label");
}

static void
topic_label_set_text_internal (TopicLabel *label, gchar *str)
{
  g_free (label->text);

  label->text = str;

  topic_label_select_region_index (label, 0, 0);
}

static void
topic_label_select_region_index (TopicLabel *label, gint anchor_index, gint end_index)
{
  static const GtkTargetEntry targets[] = {
    { "STRING", 0, 0 },
    { "TEXT", 0, 0 },
    { "COMPOUND_TEXT", 0, 0 },
    { "UTF8_STRING", 0, 0 },
  };

  g_return_if_fail (IS_TOPIC_LABEL (label));

  if (label->select_info)
  {
    GtkClipboard *clipboard;

    if (label->select_info->selection_anchor == anchor_index &&
        label->select_info->selection_end == end_index)
      return;

    label->select_info->selection_anchor = anchor_index;
    label->select_info->selection_end = end_index;

    clipboard = gtk_widget_get_clipboard (GTK_WIDGET (label), GDK_SELECTION_PRIMARY);

    if (anchor_index != end_index)
    {
      gtk_clipboard_set_with_owner (clipboard, targets, G_N_ELEMENTS (targets), get_text_callback, clear_text_callback, G_OBJECT (label));
    }
    else
    {
      if (gtk_clipboard_get_owner (clipboard) == G_OBJECT (label))
        gtk_clipboard_clear (clipboard);
    }

    gtk_widget_queue_draw (GTK_WIDGET (label));
    g_object_freeze_notify (G_OBJECT (label));
    g_object_notify (G_OBJECT (label), "cursor_position");
    g_object_notify (G_OBJECT (label), "selection_bound");
    g_object_thaw_notify (G_OBJECT (label));
  }
}

static void
get_text_callback (GtkClipboard *clipboard, GtkSelectionData *selection_data, guint info, gpointer user_data_or_owner)
{
  TopicLabel *label;

  label = TOPIC_LABEL (user_data_or_owner);

  if ((label->select_info->selection_anchor != label->select_info->selection_end) && label->text)
  {
    gint start, end;
    gint len;

    start = MIN (label->select_info->selection_anchor, label->select_info->selection_end);
    end =   MAX (label->select_info->selection_anchor, label->select_info->selection_end);

    len = strlen (label->text);

    if (end > len)
      end = len;
    if (start > len)
      start = len;

    gtk_selection_data_set_text (selection_data, label->text + start, end - start);
  }
}

static void
clear_text_callback (GtkClipboard *clipboard, gpointer user_data_or_owner)
{
  TopicLabel *label;

  label = TOPIC_LABEL (user_data_or_owner);

  if (label->select_info)
  {
    label->select_info->selection_anchor = label->select_info->selection_end;
    gtk_widget_queue_draw (GTK_WIDGET (label));
  }
}

GtkWidget*
topic_label_new ()
{
  return GTK_WIDGET (g_object_new (topic_label_get_type (), NULL));
}

void
topic_label_set_text (TopicLabel *label, const gchar *str)
{
  g_return_if_fail (IS_TOPIC_LABEL (label));

  g_object_freeze_notify (G_OBJECT (label));
  topic_label_set_label_internal (label, g_strdup (str ? str : ""));
  topic_label_recalculate (label);
  g_object_thaw_notify (G_OBJECT (label));
}
