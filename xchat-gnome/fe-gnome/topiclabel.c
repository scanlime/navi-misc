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
  PROP_LABEL,
  PROP_CURSOR_POSITION,
  PROP_SELECTION_BOUND,
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
static void topic_label_draw_cursor         (TopicLabel       *label,
                                             gint              xoffset,
                                             gint              yoffset);
static void get_text_callback               (GtkClipboard     *clipboard,
                                             GtkSelectionData *selection_data,
                                             guint             info,
                                             gpointer          user_data_or_owner);
static void clear_text_callback             (GtkClipboard     *clipboard,
                                             gpointer          user_data_or_owner);
static void get_layout_location             (TopicLabel       *label,
                                             gint             *xp,
                                             gint             *yp);
static void draw_insertion_cursor           (TopicLabel       *label,
                                             GdkRectangle     *cursor_location,
                                             gboolean          is_primary,
                                             PangoDirection    direction,
                                             gboolean          draw_arrow);
static PangoDirection get_cursor_direction  (TopicLabel       *label);

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

  g_object_class_install_property (gobject_class, PROP_LABEL, g_param_spec_string ("label", "Label", "The text of the label", NULL, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_CURSOR_POSITION, g_param_spec_int ("cursor_position", "Cursor Position", "The current position of the insertion cursor in chars", 0, G_MAXINT, 0, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_SELECTION_BOUND, g_param_spec_int ("selection_bound", "Selection Bound", "The position of the opposite end of the selection from the cursor in chars", 0, G_MAXINT, 0, G_PARAM_READWRITE));
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

//  requisition->width = width;
  requisition->width = label->misc.xpad * 2;
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

  topic_label_ensure_layout (label);

  if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_MAPPED (widget) && label->text && (label->text[0] != '\0'))
  {
    get_layout_location (label, &x, &y);
    gtk_paint_layout (widget->style, widget->window, GTK_WIDGET_STATE (widget), FALSE, &event->area, widget, "label", x, y, label->layout);

    if (label->select_info && (label->select_info->selection_anchor != label->select_info->selection_end))
    {
      gint range[2];
      GdkRegion *clip;
      GtkStateType state;

      range[0] = label->select_info->selection_anchor;
      range[1] = label->select_info->selection_end;

      if (range[0] > range[1])
      {
	gint tmp = range[0];
	range[0] = range[1];
	range[1] = tmp;
      }

      clip = gdk_pango_layout_get_clip_region (label->layout, x, y, range, 1);

      gdk_gc_set_clip_region (widget->style->black_gc, clip);

      state = GTK_STATE_SELECTED;
      if (!GTK_WIDGET_HAS_FOCUS (widget))
        state = GTK_STATE_ACTIVE;

      gdk_draw_layout_with_colors (widget->window, widget->style->black_gc, x, y, label->layout, &widget->style->text[state], &widget->style->base[state]);

      gdk_gc_set_clip_region (widget->style->black_gc, NULL);
      gdk_region_destroy (clip);
    }
    else if (label->select_info && GTK_WIDGET_HAS_FOCUS (widget))
    {
      topic_label_draw_cursor (label, x, y);
    }
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

  if (label->select_info)
    topic_label_destroy_window (label);

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
  if (label->text && label->select_info)
  {
    gint start, end;
    gint len;

    start = MIN (label->select_info->selection_anchor, label->select_info->selection_end);
    end   = MAX (label->select_info->selection_anchor, label->select_info->selection_end);

    len = strlen (label->text);

    if (end > len)
      end = len;
    if (start > len)
      start = len;

    if (start != end)
      gtk_clipboard_set_text (gtk_widget_get_clipboard (GTK_WIDGET (label), GDK_SELECTION_CLIPBOARD), label->text + start, end - start);
  }
}

static void
topic_label_select_all (TopicLabel *label)
{
  topic_label_select_region_index (label, 0, strlen (label->text));
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
topic_label_draw_cursor (TopicLabel *label, gint xoffset, gint yoffset)
{
  if (label->select_info == NULL)
    return;

  if (GTK_WIDGET_DRAWABLE (label))
  {
    GtkWidget *widget = GTK_WIDGET (label);

    PangoDirection keymap_direction;
    PangoDirection cursor_direction;
    PangoRectangle strong_pos, weak_pos;
    gboolean split_cursor;
    PangoRectangle *cursor1 = NULL;
    PangoRectangle *cursor2 = NULL;
    GdkRectangle cursor_location;
    PangoDirection dir1 = PANGO_DIRECTION_NEUTRAL;
    PangoDirection dir2 = PANGO_DIRECTION_NEUTRAL;

    keymap_direction = gdk_keymap_get_direction (gdk_keymap_get_for_display (gtk_widget_get_display (widget)));
    cursor_direction = get_cursor_direction (label);

    topic_label_ensure_layout (label);

    pango_layout_get_cursor_pos (label->layout, label->select_info->selection_end, &strong_pos, &weak_pos);

    g_object_get (gtk_widget_get_settings (widget), "gtk-split-cursor", &split_cursor, NULL);

    dir1 = cursor_direction;

    if (split_cursor)
    {
      cursor1 = &strong_pos;

      if (strong_pos.x != weak_pos.x || strong_pos.y != weak_pos.y)
      {
        dir2 = (cursor_direction == PANGO_DIRECTION_LTR) ? PANGO_DIRECTION_RTL : PANGO_DIRECTION_LTR;
        cursor2 = &weak_pos;
      }
    }
    else
    {
      if (keymap_direction == cursor_direction)
        cursor1 = &strong_pos;
      else
        cursor1 = &weak_pos;
    }

    cursor_location.x = xoffset + PANGO_PIXELS (cursor1->x);
    cursor_location.y = yoffset + PANGO_PIXELS (cursor1->y);
    cursor_location.width = 0;
    cursor_location.height = PANGO_PIXELS (cursor1->height);

    draw_insertion_cursor (label, &cursor_location, TRUE, dir1, dir2 != PANGO_DIRECTION_NEUTRAL);

    if (dir2 != PANGO_DIRECTION_NEUTRAL)
    {
      cursor_location.x = xoffset + PANGO_PIXELS (cursor2->x);
      cursor_location.y = yoffset + PANGO_PIXELS (cursor2->y);
      cursor_location.width = 0;
      cursor_location.height = PANGO_PIXELS (cursor2->height);

      draw_insertion_cursor (label, &cursor_location, FALSE, dir2, TRUE);
    }
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

static void
get_layout_location (TopicLabel *label, gint *xp, gint *yp)
{
  GtkMisc *misc;
  GtkWidget *widget;
  gfloat xalign;
  gint x, y;

  misc = GTK_MISC (label);
  widget = GTK_WIDGET (label);

  if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
    xalign = misc->xalign;
  else
    xalign = 1.0 - misc->xalign;

  x = floor (widget->allocation.x + (gint)misc->xpad + xalign * (widget->allocation.width - widget->requisition.width) + 0.5);

  if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR)
    x = MAX (x, widget->allocation.x + misc->xpad);
  else
    x = MIN (x, widget->allocation.x + widget->allocation.width - widget->requisition.width - misc->xpad);

  y = floor (widget->allocation.y + (gint)misc->ypad + MAX (((widget->allocation.height - widget->requisition.height) * misc->yalign) + 0.5, 0));

  if (xp)
    *xp = x;
  if (yp)
    *yp = y;
}

static void
draw_insertion_cursor (TopicLabel *label, GdkRectangle *cursor_location, gboolean is_primary, PangoDirection direction, gboolean draw_arrow)
{
  GtkWidget *widget = GTK_WIDGET (label);
  GtkTextDirection text_dir;

  if (direction == PANGO_DIRECTION_LTR)
    text_dir = GTK_TEXT_DIR_LTR;
  else
    text_dir = GTK_TEXT_DIR_RTL;

  gtk_draw_insertion_cursor (widget, widget->window, NULL, cursor_location, is_primary, text_dir, draw_arrow);
}

static PangoDirection
get_cursor_direction (TopicLabel *label)
{
  GSList *l;

  g_assert (label->select_info);

  topic_label_ensure_layout (label);

  for (l = pango_layout_get_lines (label->layout); l; l = l->next)
  {
    PangoLayoutLine *line = l->data;

    /* If label->select_info->selection_end is at the very end of
     * the line, we don't know if the cursor is on this line or
     * the next without looking ahead at the next line. (End
     * of paragraph is different from line break.) But it's
     * definitely in this paragraph, which is good enough
     * to figure out the resolved direction.
     */
    if (line->start_index + line->length >= label->select_info->selection_end)
      return line->resolved_dir;
  }

  return PANGO_DIRECTION_LTR;
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
