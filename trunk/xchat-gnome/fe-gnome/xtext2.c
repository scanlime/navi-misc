#include "xtext2.h"
#include <string.h>

static void     xtext2_class_init      (XText2Class *klass);
static void     xtext2_init            (XText2 *xtext);

static void     xtext2_dispose         (GObject *object);
static void     xtext2_finalize        (GObject *object);
static void     xtext2_set_property    (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);
static void     xtext2_get_property    (GObject *object, guint param_id, GValue *value, GParamSpec *pspec);

static void     xtext2_realize         (GtkWidget *widget);
static void     xtext2_unrealize       (GtkWidget *widget);
static void     xtext2_size_request    (GtkWidget *widget, GtkRequisition *requisition);
static void     xtext2_size_allocate   (GtkWidget *widget, GtkAllocation *allocation);
static gboolean xtext2_button_press    (GtkWidget *widget, GdkEventButton *event);
static gboolean xtext2_button_release  (GtkWidget *widget, GdkEventButton *event);
static gboolean xtext2_motion_notify   (GtkWidget *widget, GdkEventMotion *event);
static gboolean xtext2_selection_clear (GtkWidget *widget, GdkEventSelection *event);
static void     xtext2_selection_get   (GtkWidget *widget, GtkSelectionData *sel_data, guint info, guint time);
static gboolean xtext2_expose          (GtkWidget *widget, GdkEventExpose *event);
static gboolean xtext2_scroll          (GtkWidget *widget, GdkEventScroll *event);

static void     backend_init           (XText2 *xtext);
static void     backend_deinit         (XText2 *xtext);
static void     backend_font_open      (XText2 *xtext, char *name);
static int      backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret);
static int      backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte);
static int      backend_draw_text      (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte);

static void     paint                  (GtkWidget *widget, GdkRectangle *area);
static char*    selection_get_text     (XText2 *xtext, int *len);
static void     set_indent             (XText2 *xtext, gboolean indent);
static void     set_show_separator     (XText2 *xtext, gboolean show);
static void     set_tint_color         (XText2 *xtext, GdkColor *color);
static void     set_word_wrap          (XText2 *xtext, gboolean wrap);

static gpointer parent_class;

/* properties */
enum
{
  PROP_0,
  PROP_FONT,
  PROP_INDENT,
  PROP_SHOW_SEPARATOR,
  PROP_TINT_COLOR,
  PROP_WORD_WRAP,
};

/* signals */
enum
{
  WORD_CLICK,
  LAST_SIGNAL,
};

static gint xtext_signals[LAST_SIGNAL];

/* selection values */
enum
{
  TARGET_UTF8_STRING,
  TARGET_STRING,
  TARGET_TEXT,
  TARGET_COMPOUND_TEXT,
};

struct _XText2Private
{
  /* view settings */
  GdkColor     tint;                 /* color to tint the background */
  gboolean     indent:         TRUE; /* whether the widget can indent */
  gboolean     show_separator: TRUE; /* whether the separator bar is drawn */
  gboolean     word_wrap:      TRUE; /* wrap words or not */
  gboolean     avoid_trans:    TRUE; /* avoid transparency? */

  /* drawing data */
  gint         depth;                /* gdk window depth */
  GdkGC       *bgc;                  /* background color */
  GdkGC       *fgc;                  /* foreground color */
  GdkGC       *light_gc;             /* separator bar (light) */
  GdkGC       *dark_gc;              /* separator bar (dark) */
  GdkGC       *thin_gc;              /* separator bar (thin) */
  GdkDrawable *draw_buffer;          /* buffer to draw into */
  GdkPixmap   *background_pm;        /* background image */
  GdkCursor   *hand_cursor;          /* hand cursor (for urls, etc) */

#ifdef USE_XFT
  /* XFT backend */
  XftColor  color[20];
  XftColor *color_fg;
  XftColor *color_bg;
  XftDraw  *xftdraw;
  XftFont  *font;
#else
  /* Pango backend */
  struct
  {
    PangoFontDescription *font;
    int ascent, descent;
  } *font, pango_font;
  PangoLayout *layout;
#endif

  /* selection information */
  gboolean button_down:        TRUE;
  int      select_start_x;
  int      select_start_y;
  int      select_start_adj;         /* adjustment value when the selection started */
  int      select_end_x;
  int      select_end_y;

  /* general state */
  gboolean moving_separator:   TRUE; /* currently moving the separator bar? */
  gboolean word_or_line_select:TRUE; /* selecting a word or line? */
};

GType
xtext2_get_type (void)
{
  static GType xtext2_type = 0;
  if (!xtext2_type)
  {
    static const GTypeInfo xtext2_info =
    {
      sizeof (XText2Class),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    xtext2_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (XText2),
      0,                  /* n preallocs */
      (GInstanceInitFunc) xtext2_init,
    };

    xtext2_type = g_type_register_static (GTK_TYPE_WIDGET, "XText2", &xtext2_info, 0);
  }

  return xtext2_type;
}

static void
xtext2_class_init (XText2Class *klass)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GParamSpec *pspec;

  object_class = (GObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;

  parent_class = g_type_class_ref (GTK_TYPE_WIDGET);

  object_class->dispose               = xtext2_dispose;
  object_class->finalize              = xtext2_finalize;
  object_class->set_property          = xtext2_set_property;
  object_class->get_property          = xtext2_get_property;

  widget_class->realize               = xtext2_realize;
  widget_class->unrealize             = xtext2_unrealize;
  widget_class->size_request          = xtext2_size_request;
  widget_class->size_allocate         = xtext2_size_allocate;
  widget_class->button_press_event    = xtext2_button_press;
  widget_class->button_release_event  = xtext2_button_release;
  widget_class->motion_notify_event   = xtext2_motion_notify;
  widget_class->selection_clear_event = xtext2_selection_clear;
  widget_class->selection_get         = xtext2_selection_get;
  widget_class->expose_event          = xtext2_expose;
  widget_class->scroll_event          = xtext2_scroll;

  pspec = g_param_spec_string ("font",
                               "Font",
			       "Font name for text rendering",
			       "",
			       G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_FONT, pspec);

  pspec = g_param_spec_boolean ("indent",
                                "Indent",
				"Whether the XText has an indent",
				TRUE,
				G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (object_class, PROP_INDENT, pspec);

  pspec = g_param_spec_boolean ("show-separator",
                                "Show Separator",
				"Show the separator",
				TRUE,
				G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (object_class, PROP_SHOW_SEPARATOR, pspec);

  pspec = g_param_spec_boxed ("tint-gdk",
                              "Tint Color",
			      "Color to tint the background",
			      GDK_TYPE_COLOR,
			      G_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_TINT_COLOR, pspec);

  pspec = g_param_spec_boolean ("word-wrap",
                                "Word Wrap",
				"Wrap words in the widget",
				TRUE,
				G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
  g_object_class_install_property (object_class, PROP_WORD_WRAP, pspec);

  xtext_signals[WORD_CLICK] = g_signal_new ("word-click",
    G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
    G_STRUCT_OFFSET (XText2Class, word_click), NULL, NULL,
    gtk_marshal_VOID__POINTER_POINTER, G_TYPE_NONE,
    2, G_TYPE_POINTER, G_TYPE_POINTER);
}

static void
xtext2_init (XText2 *xtext)
{
  xtext->priv = g_new0 (XText2Private, 1);
  gtk_widget_set_double_buffered (GTK_WIDGET (xtext), FALSE);
}

GtkWidget*
xtext2_new (GdkColor palette[], gboolean separator)
{
  return GTK_WIDGET (g_object_new (xtext2_get_type (), NULL));
}

static void
xtext2_set_property (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec)
{
  XText2 *xtext = XTEXT2(object);
  switch (param_id)
  {
    case PROP_FONT:
      break;
    case PROP_INDENT:
    {
      gboolean indent = g_value_get_boolean (value);
      set_indent (xtext, indent);
      break;
    }
    case PROP_SHOW_SEPARATOR:
    {
      gboolean show = g_value_get_boolean (value);
      set_show_separator (xtext, show);
      break;
    }
    case PROP_TINT_COLOR:
    {
      GdkColor *color = g_value_get_boxed (value);
      set_tint_color (xtext, color);
      break;
    }
    case PROP_WORD_WRAP:
    {
      gboolean wrap = g_value_get_boolean (value);
      set_word_wrap (xtext, wrap);
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  }
}

static void
xtext2_get_property (GObject *object, guint param_id, GValue *value, GParamSpec *pspec)
{
  XText2 *xtext = XTEXT2(object);
  switch (param_id)
  {
    case PROP_FONT:
      break;
    case PROP_INDENT:
      g_value_set_boolean (value, xtext->priv->indent);
      break;
    case PROP_SHOW_SEPARATOR:
      g_value_set_boolean (value, xtext->priv->show_separator);
      break;
    case PROP_TINT_COLOR:
      g_value_set_boxed (value, &xtext->priv->tint);
      break;
    case PROP_WORD_WRAP:
      g_value_set_boolean (value, xtext->priv->word_wrap);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  }
}

static void
xtext2_dispose (GObject *object)
{
}

static void
xtext2_finalize (GObject *object)
{
  XText2 *xtext = (XText2*) object;

  g_free (xtext->priv);
  xtext->priv = NULL;
}

static void
xtext2_realize (GtkWidget *widget)
{
  XText2 *xtext;
  GdkWindowAttr attributes;
  guint event_mask;
  GdkColormap *cmap;
  GdkGCValues val;
  GdkColor color;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  xtext = XTEXT2 (widget);

  event_mask = GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
#ifdef MOTION_MONITOR
               GDK_LEAVE_NOTIFY_MASK |
#endif
	       GDK_POINTER_MOTION_MASK;

  cmap = gtk_widget_get_colormap (widget);

  attributes.x           = widget->allocation.x;
  attributes.y           = widget->allocation.y;
  attributes.width       = widget->allocation.width;
  attributes.height      = widget->allocation.height;
  attributes.wclass      = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask  = gtk_widget_get_events (widget) | event_mask;
  attributes.colormap    = cmap;
  attributes.visual      = gtk_widget_get_visual (widget);

  widget->window = gdk_window_new (widget->parent->window, &attributes,
                                   GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP);
  gdk_window_set_user_data (widget->window, widget);

  xtext->priv->depth = gdk_drawable_get_visual (widget->window)->depth;

  val.subwindow_mode = GDK_INCLUDE_INFERIORS;
  val.graphics_exposures = 0;

  xtext->priv->bgc       = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
  xtext->priv->fgc       = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
  xtext->priv->light_gc = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
  xtext->priv->dark_gc  = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
  xtext->priv->thin_gc  = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);

  /* light separator bar */
  color.red = 0xffff; color.green = 0xffff; color.blue = 0xffff;
  gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE);
  gdk_gc_set_foreground (xtext->priv->light_gc, &color);

  /* dark separator bar */
  color.red = 0x1111; color.green = 0x1111; color.blue = 0x1111;
  gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE);
  gdk_gc_set_foreground (xtext->priv->dark_gc, &color);

  /* thin separator bar */
  color.red = 0x8e38; color.green = 0x8e38; color.blue = 0x9f38;
  gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE);
  gdk_gc_set_foreground (xtext->priv->thin_gc, &color);

  /* FIXME: set fg & bg colors */

  /* draw_directly to window */
  xtext->priv->draw_buffer = widget->window;

#if defined(USE_XLIB) || defined(WIN32)
  /* FIXME: transparency */
#endif
  if (xtext->priv->background_pm)
  {
    gdk_gc_set_tile (xtext->priv->bgc, xtext->priv->background_pm);
    gdk_gc_set_ts_origin (xtext->priv->bgc, 0, 0);
    /* FIXME: ts? */
    gdk_gc_set_fill (xtext->priv->bgc, GDK_TILED);
  }

#if (GTK_MAJOR_VERSION == 2) && (GTK_MINOR_VERSION == 0)
  xtext->priv->hand_cursor = gdk_cursor_new (GDK_HAND1);
#else
  xtext->priv->hand_cursor = gdk_cursor_new_for_display (gdk_drawable_get_display (widget->window), GDK_HAND1);
#endif

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  backend_init (xtext);
}

static void
xtext2_unrealize (GtkWidget *widget)
{
  XText2 *xtext = XTEXT2 (widget);
  backend_deinit (xtext);
  if (((GtkWidgetClass*)parent_class)->unrealize)
    ((GtkWidgetClass*)parent_class)->unrealize (widget);
}

static void
xtext2_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  requisition->width = 200;
  requisition->height = 90;
}

static void
xtext2_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  XText2 *xtext = XTEXT2 (widget);
  gboolean height_only = FALSE;
  gboolean transparent = TRUE;

  /* FIXME: buffer madness */
  if (allocation->x == widget->allocation.x &&
      allocation->y == widget->allocation.y &&
      xtext->priv->avoid_trans)
    transparent = FALSE;

  xtext->priv->avoid_trans = FALSE;

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
  {
    /* FIXME: stuff! */
  }
}

static gboolean
xtext2_button_press (GtkWidget *widget, GdkEventButton *event)
{
  XText2 *xtext = XTEXT2 (widget);
  int x, y;

  gdk_window_get_pointer (widget->window, &x, &y, 0);

  /* right click */
  if (event->button == 3)
  {
    return FALSE;
  }

  if (event->button == 2)
  {
    return FALSE;
  }

  /* only want left click at this point */
  if (event->button != 1)
    return FALSE;

  /* double click, select word */
  if (event->type == GDK_2BUTTON_PRESS)
  {
    return FALSE;
  }

  /* triple click, select line */
  if (event->type == GDK_3BUTTON_PRESS)
  {
    return FALSE;
  }

  /* check if it was a separator bar click */
  /* FIXME: depends on buffer */

  xtext->priv->button_down = TRUE;
  xtext->priv->select_start_x = x;
  xtext->priv->select_start_y = y;
  xtext->priv->select_start_adj = xtext->adj->value;
  return FALSE;
}

static gboolean
xtext2_button_release (GtkWidget *widget, GdkEventButton *event)
{
  XText2 *xtext = XTEXT2 (widget);
  unsigned char *word;

  if (xtext->priv->moving_separator)
  {
    xtext->priv->moving_separator = FALSE;
    /* FIXME: stuff */
    return FALSE;
  }

  if (xtext->priv->word_or_line_select)
  {
    xtext->priv->word_or_line_select = FALSE;
    xtext->priv->button_down = FALSE;
    return FALSE;
  }

  if (event->button == 1)
  {
    /* FIXME: stuff */
  }

  return FALSE;
}

static gboolean
xtext2_motion_notify (GtkWidget *widget, GdkEventMotion *event)
{
  /* FIXME !!! */
  return FALSE;
}

static gboolean
xtext2_selection_clear (GtkWidget *widget, GdkEventSelection *event)
{
  /* FIXME !!! */
  return TRUE;
}

static void
xtext2_selection_get (GtkWidget *widget, GtkSelectionData *sel_data, guint info, guint time)
{
  XText2 *xtext = XTEXT2 (widget);
  char *stripped;
  guchar *new_text;
  int len;
  gsize glen;

  stripped = selection_get_text (xtext, &len);
  if (!stripped)
    return;

  switch (info)
  {
    case TARGET_UTF8_STRING:
      /* it's already in utf8 */
      gtk_selection_data_set_text (sel_data, stripped, len);
      break;
    case TARGET_TEXT:
    case TARGET_COMPOUND_TEXT:
    {
      GdkAtom encoding;
      gint format;
      gint new_length;

#if (GTK_MAJOR_VERSION == 2) && (GTK_MINOR_VERSION == 0)
      gdk_string_to_compound_text (stripped, &encoding, &format, &new_text, &new_length);
#else
      gdk_string_to_compound_text_for_display (gdk_drawable_get_display (widget->window),
                                   stripped, &encoding, &format, &new_text, &new_length);
#endif
      gtk_selection_data_set (sel_data, encoding, format, new_text, new_length);
      gdk_free_compound_text (new_text);
      break;
    }
    default:
      new_text = g_locale_from_utf8 (stripped, len, NULL, &glen, NULL);
      gtk_selection_data_set (sel_data, GDK_SELECTION_TYPE_STRING, 8, new_text, glen);
      g_free (new_text);
  };
  g_free (stripped);
}

static gboolean
xtext2_expose (GtkWidget *widget, GdkEventExpose *event)
{
  paint (widget, &event->area);
  return FALSE;
}

static gboolean
xtext2_scroll (GtkWidget *widget, GdkEventScroll *event)
{
  XText2 *xtext = XTEXT2 (widget);
  gfloat new_value;


  if (event->direction == GDK_SCROLL_UP)
  {
    /* mousewheel/pgup */
    new_value = xtext->adj->value - (xtext->adj->page_increment / 10);
    if (new_value < xtext->adj->lower)
      new_value = xtext->adj->lower;
    gtk_adjustment_set_value (xtext->adj, new_value);
  }
  else if (event->direction == GDK_SCROLL_DOWN)
  {
    /* mousewheel/pgdn */
    new_value = xtext->adj->value + (xtext->adj->page_increment / 10);
    if (new_value > (xtext->adj->upper - xtext->adj->page_size))
      new_value = xtext->adj->upper -xtext->adj->page_size;
    gtk_adjustment_set_value (xtext->adj, new_value);
  }
  return FALSE;
}

#ifdef USE_XFT
/* ========================================= */
/* ========== XFT 1 and 2 BACKEND ========== */
/* ========================================= */

static void
backend_init (XText2 *xtext)
{
  if (xtext->priv->xftdraw == NULL)
  {
    xtext->priv->xftdraw = XftDrawCreate (
      GDK_WINDOW_XDISPLAY (xtext->priv->draw_buffer),
      GDK_WINDOW_XWINDOW (xtext->priv->draw_buffer),
      GDK_VISUAL_XVISUAL (gdk_drawable_get_visual (xtext->priv->draw_buffer)),
      GDK_COLORMAP_XCOLORMAP (gdk_drawable_get_colormap (xtext->priv->draw_buffer)));
    XftDrawSetSubwindowMode (xtext->priv->xftdraw, IncludeInferiors);
  }
}

static void
backend_deinit (XText2 *xtext)
{
  if (xtext->priv->xftdraw)
  {
    XftDrawDestroy (xtext->priv->xftdraw);
    xtext->priv->xftdraw = NULL;
  }
}

static void
backend_font_open (XText2 *xtext, char *name)
{
}

inline static int
backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret)
{
}

static int
backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte)
{
}

static int
backend_draw_text (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte)
{
}
#else /* !USE_XFT */
/* ======================================= */
/* ============ PANGO BACKEND ============ */
/* ======================================= */

#define backend_font_close(d,f) pango_font_description_free(f->font)
static void
backend_init (XText2 *xtext)
{
}

static void
backend_deinit (XText2 *xtext)
{
}

static void
backend_font_open (XText2 *xtext, char *name)
{
}

static int
backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret)
{
}

static int
backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte)
{
}

static int
backend_draw_text (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte)
{
}
#endif

static void
paint (GtkWidget *widget, GdkRectangle *area)
{
  /* FIXME !!! */
}

static char*
selection_get_text (XText2 *xtext, int *len)
{
  /* FIXME !!! */
}

static void
set_indent (XText2 *xtext, gboolean indent)
{
  xtext->priv->indent = indent;
}

static void
set_show_separator (XText2 *xtext, gboolean show)
{
  xtext->priv->show_separator = show;
}

static void
set_tint_color (XText2 *xtext, GdkColor *color)
{
  memcpy (&xtext->priv->tint, color, sizeof (GdkColor));
}

static void
set_word_wrap (XText2 *xtext, gboolean wrap)
{
  xtext->priv->word_wrap = wrap;
}
