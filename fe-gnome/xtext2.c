#include "xtext2.h"
#include <string.h>
#include <ctype.h>

typedef struct _XTextFormat XTextFormat;

static void           xtext2_class_init      (XText2Class *klass);
static void           xtext2_init            (XText2 *xtext);

static void           xtext2_dispose         (GObject *object);
static void           xtext2_finalize        (GObject *object);
static void           xtext2_set_property    (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);
static void           xtext2_get_property    (GObject *object, guint param_id, GValue *value, GParamSpec *pspec);

static void           xtext2_realize         (GtkWidget *widget);
static void           xtext2_unrealize       (GtkWidget *widget);
static void           xtext2_size_request    (GtkWidget *widget, GtkRequisition *requisition);
static void           xtext2_size_allocate   (GtkWidget *widget, GtkAllocation *allocation);
static gboolean       xtext2_expose          (GtkWidget *widget, GdkEventExpose *event);
static gboolean       xtext2_scroll          (GtkWidget *widget, GdkEventScroll *event);
static gboolean       xtext2_button_press    (GtkWidget *widget, GdkEventButton *event);
static gboolean       xtext2_button_release  (GtkWidget *widget, GdkEventButton *event);
static gboolean       xtext2_motion_notify   (GtkWidget *widget, GdkEventCrossing *event);

static void           backend_init           (XText2 *xtext);
static void           backend_deinit         (XText2 *xtext);
inline static int     backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret);
static void           backend_font_open      (XText2 *xtext, char *name);
static void           backend_font_close     (XText2 *xtext);
static int            backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte);
static void           backend_draw_text      (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte);

static void           paint                  (GtkWidget *widget, GdkRectangle *area);
static void           render_page            (XText2 *xtext);
static int            render_page_timeout    (XText2 *xtext);
static int            render_line            (XText2 *xtext, XTextFormat *f, textentry *ent, int line, int lines_max, int subline, int win_width);
static int            render_str             (XText2 *xtext, XTextFormat *f, int y, textentry *ent, unsigned char *str, int len, int win_width, int indent, int line, int left_only);
static int            render_flush           (XText2 *xtext, int x, int y, unsigned char *str, int len, GdkGC *gc, gboolean multibyte);
static int            render_ents            (XText2 *xtext, textentry *start, textentry *stop);
static void           draw_bg                (XText2 *xtext, int x, int y, int width, int height);
static void           draw_sep               (XText2 *xtext, int y);
static textentry*     nth                    (XText2 *xtext, int line, int *subline);
static textentry*     find_char              (XText2 *xtext, int x, int y, int *offset, gboolean *out_of_bounds);
static int            find_x                 (XText2 *xtext, int x, textentry *ent, int subline, int line, gboolean *out_of_bounds);
static int            find_subline           (XText2 *xtext, textentry *ent, int line);
static void           set_fg                 (XText2 *xtext, GdkGC *gc, int index);
static void           set_bg                 (XText2 *xtext, GdkGC *gc, int index);
static int            text_width             (XText2 *xtext, unsigned char *str, int len, gboolean *mb_ret);
static int            text_width_8bit        (XText2 *xtext, unsigned char *str, int len);
static unsigned char* strip_color            (unsigned char *text, int len, unsigned char *outbuf, int *newlen, gboolean *mb_ret);

static int            count_lines_taken      (XText2 *xtext, textentry *ent);
static int            find_next_wrap         (XText2 *xtext, textentry *ent, unsigned char *str, int win_width, int indent);
static void           recalc_widths          (XText2 *xtext, gboolean do_str_width);
static void           calc_lines             (XText2 *xtext, gboolean fire_signal);
static void           fix_indent             (XText2 *xtext);
static void           reset                  (XText2 *xtext, gboolean mark, gboolean attribs);
static void           adjustment_set         (XText2 *xtext, gboolean fire_signal);
static void           adjustment_changed     (GtkAdjustment *adj, XText2 *xtext);
static gint           adjustment_timeout     (XText2 *xtext);

static gpointer parent_class;

#define XCHAT
#define MOTION_MONITOR       /* URL hilights */
#define SMOOTH_SCROLL        /* line-by-line or pixel scroll? */
#define SCROLL_HACK          /* use XCopyArea scroll or full redraw? */
#define GDK_MULTITHREAD_SAFE /* */
#define USE_DB               /* double-buffer */

#ifdef SCROLL_HACK
#define dontscroll(xtext) ((xtext)->priv->last_pixel_pos = 2147483647)
#else
#define dontscroll(xtext)
#endif

#define MARGIN 2 /* left margin (in pixels) */
#define WORDWRAP_LIMIT 24
#define REFRESH_TIMEOUT 20

/* is delimiter */
#define is_del(c) \
	(c == ' ' || c == '\n' || c == ')' || c == '(' || \
	 c == '>' || c == '<'  || c == ATTR_RESET || c == ATTR_BOLD || c == '\0')

#define charlen(str) g_utf8_skip[*(guchar *)(str)]

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

struct _XText2Private
{
  /* This structure contains all the private data of the XText.
     Most of this is internal state needed to draw */

  /* state associated with rendering specific buffers */
  GHashTable  *buffer_info;          /* stores an XTextFormat for each buffer we observe */
  XTextBuffer *current_buffer;
  XTextBuffer *original_buffer;
  XTextBuffer *selection_buffer;

  /* view settings */
  gboolean     indent;               /* indent text? */
  gboolean     show_separator;       /* show the separator bar? */
  gboolean     thinline;             /* thin separator bar? */
  GdkColor     tint;                 /* tint color */
  gboolean     word_wrap;            /* wrap words? */
  int          fontsize;             /* width in pixels of the space ' ' character */
  int          max_auto_indent;      /* maximum allowed indent */
  gulong       palette[20];          /* color palette */
  gboolean     overdraw: TRUE;       /* draw twice */
  gboolean     bold: TRUE;           /* draw in bold? */
  gboolean     time_stamp: TRUE;     /* show timestamp? */
  GdkPixmap   *pixmap;               /* background pixmap (NULL = palette[19]) */

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

  /* state data */
  int          pixel_offset;         /* number of pixels the top line is chopped by */
  int          last_win_x;           /* previous X */
  int          last_win_y;           /* previous Y */
  int          last_pixel_pos;       /* where the window was scrolled to */
  int          jump_in_offset;       /* "" start rendering */
  int          jump_out_offset;      /* "" stop rendering */
  int          stamp_width;          /* timestamp width */
  int          clip_x;
  int          clip_x2;              /* clipping (x) */
  int          clip_y;
  int          clip_y2;              /* clipping (y) */
  int          ts_x;                 /* ts origin for bgc GC */
  int          ts_y;
  gboolean     dont_render;
  gboolean     dont_render2;
  gboolean     in_hilight;
  gboolean     un_hilight;
  gboolean     skip_border_fills;
  gboolean     underline;
  gboolean     backcolor;            /* non-default backcolor? */
  gboolean     button_down;          /* is a button currently down? */
  gboolean     parsing_color;        /* currently parsing a foreground color */
  gboolean     parsing_backcolor;    /* currently parsing a background color */
  gboolean     render_hilights_only;
  gboolean     indent_changed;       /* whether the indent is dirty */
  gboolean     moving_separator;     /* moving the separator bar? */
  gboolean     auto_indent;          /* automatically change indent */
  gboolean     skip_stamp;           /* skip timestamp */
  gboolean     add_io_tag;           /* "" when adding new text */
  gint         io_tag;               /* for delayed refresh */
  char         num[8];               /* for parsing mIRC color codes */
  int          nc;                   /* offset into xtext->priv->num */
  int          col_fore;             /* current foreground color */
  int          col_back;             /* current background color */
  gulong       vc_signal_tag;        /* signal handler for adj->"value changed" */
  int          hilight_start;
  int          hilight_end;
  textentry   *hilight_ent;

  int          select_start_adj;
  int          select_start_x;
  int          select_start_y;
  int          select_end_x;
  int          select_end_y;

  char         scratch_buffer[4096];

  XTextFormat *current_format;

  /* backend state */
  guint16 fontwidth[128];            /* pixel width for the ASCII chars */
  int     spacewidth;                /* pixel width of the space ' ' character */
#ifdef USE_XFT
  XftColor     color[20];
  XftColor    *xft_fg;               /* both of these point into */
  XftColor    *xft_bg;               /* the color[20] array */
  XftDraw     *xft_draw;
  XftFont     *xft_font;
#else
  struct
  {
    PangoFontDescription *font;
    int ascent;
    int descent;
  } *font, pango_font;
  PangoLayout *layout;
#endif
};

struct _XTextFormat
{
  int        window_width;   /* window size when */
  int        window_height;  /* last rendered */
  int        indent;         /* indent value */
  guint16    grid_offset[256];
  gboolean   grid_dirty;
  gboolean   scrollbar_down; /* is scrollbar at the bottom? */
  int        old_adj;        /* last known adjustment value */
  int        num_lines;

  textentry *wrapped_first;
  textentry *wrapped_last;
  textentry *pagetop;
  int        pagetop_subline;
  int        pagetop_line;

  /* handlers */
  guint      append_handler;
  guint      clear_handler;
  guint      remove_handler;
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
  widget_class->expose_event          = xtext2_expose;
  widget_class->scroll_event          = xtext2_scroll;
  widget_class->button_press_event    = xtext2_button_press;
  widget_class->button_release_event  = xtext2_button_release;
  widget_class->motion_notify_event   = xtext2_motion_notify;

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

  xtext->priv->buffer_info = g_hash_table_new (g_direct_hash, g_direct_equal);

  xtext->adj = GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 1, 1, 1, 1));
  g_object_ref (G_OBJECT (xtext->adj));
  gtk_object_sink (GTK_OBJECT (xtext->adj));

  /* each XText owns an empty buffer */
  xtext->priv->original_buffer = xtext_buffer_new ();
  xtext2_show_buffer (xtext, xtext->priv->original_buffer);

  gtk_widget_set_double_buffered (GTK_WIDGET (xtext), FALSE);

  xtext->priv->indent = TRUE;
  xtext->priv->auto_indent = TRUE;
  xtext->priv->max_auto_indent = 1000;
  xtext->priv->time_stamp = TRUE;
  xtext->priv->show_separator = TRUE;
  xtext->priv->word_wrap = TRUE;
  xtext->priv->pixel_offset = 0;
  xtext->priv->thinline = TRUE;
  xtext->priv->clip_x = 0;
  xtext->priv->clip_x2 = 1000000;
  xtext->priv->clip_y = 0;
  xtext->priv->clip_y2 = 1000000;
  xtext->priv->ts_x = 0;
  xtext->priv->ts_y = 0;

  xtext->priv->vc_signal_tag = g_signal_connect (G_OBJECT (xtext->adj), "value-changed", G_CALLBACK (adjustment_changed), xtext);
}

GtkWidget*
xtext2_new ()
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
      xtext->priv->indent = indent;
      break;
    }
    case PROP_SHOW_SEPARATOR:
    {
      gboolean show = g_value_get_boolean (value);
      xtext->priv->show_separator = show;
      break;
    }
    case PROP_TINT_COLOR:
    {
      GdkColor *color = g_value_get_boxed (value);
      xtext->priv->tint = *color;
      break;
    }
    case PROP_WORD_WRAP:
    {
      gboolean wrap = g_value_get_boolean (value);
      xtext->priv->word_wrap = wrap;
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

  xtext->priv->bgc      = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
  xtext->priv->fgc      = gdk_gc_new_with_values (widget->window, &val, GDK_GC_EXPOSURES | GDK_GC_SUBWINDOW);
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

  /* set the foreground & background colors. if the user has initialized
   * the palette, this will do colors. At init the colors are all set to 0 */
  set_fg (xtext, xtext->priv->fgc, 18);
  set_bg (xtext, xtext->priv->fgc, 19);
  set_fg (xtext, xtext->priv->bgc, 19);

  /* draw_directly to window */
  xtext->priv->draw_buffer = widget->window;

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  backend_init (xtext);
  xtext2_refresh (xtext);
}

static void
xtext2_unrealize (GtkWidget *widget)
{
  XText2 *xtext = XTEXT2 (widget);
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
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (allocation->width == f->window_width)
    height_only = TRUE;

  /*
  if (allocation->x == widget->allocation.x &&
      allocation->y == widget->allocation.y &&
      xtext->priv->avoid_trans)
    transparent = FALSE;
  */

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
  {
    f->window_width = allocation->width;
    f->window_height = allocation->height;
    gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
    dontscroll (xtext);
    if (!height_only)
    {
      calc_lines (xtext, FALSE);
    }
    else
    {
      f->pagetop = NULL;
      adjustment_set (xtext, FALSE);
    }
#if defined(USE_XLIB) || defined(WIN32)
    if (transparent && xtext->priv->transparent && xtext->priv->shaded)
    {
      /*
      free_trans (xtext);
      load_trans (xtext);
      */
    }
#endif

    if (f->scrollbar_down)
      gtk_adjustment_set_value (xtext->adj, xtext->adj->upper - xtext->adj->page_size);
  }
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
    new_value = xtext->adj->value - (xtext->adj->page_increment / 10);
    if (new_value < xtext->adj->lower)
      new_value = xtext->adj->lower;
    gtk_adjustment_set_value (xtext->adj, new_value);
  }
  else if (event->direction == GDK_SCROLL_DOWN)
  {
    new_value = xtext->adj->value + (xtext->adj->page_increment / 10);
    if (new_value > (xtext->adj->upper - xtext->adj->page_size))
      new_value = xtext->adj->upper - xtext->adj->page_size;
    gtk_adjustment_set_value (xtext->adj, new_value);
  }
  return FALSE;
}

static gboolean
xtext2_button_release (GtkWidget *widget, GdkEventButton *event)
{
  XText2 *xtext = XTEXT2 (widget);
  unsigned char *word;
  int old;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (xtext->priv->moving_separator)
  {
    xtext->priv->moving_separator = FALSE;
    old = f->indent;
    if (event->x < (4 * widget->allocation.width) / 5 && event->x > 15)
      f->indent = event->x;
    fix_indent (xtext);
    if (f->indent != old)
    {
      recalc_widths (xtext, FALSE);
      adjustment_set (xtext, TRUE);
      render_page (xtext);
    }
    else
    {
      draw_sep (xtext, -1);
    }
    return FALSE;
  }

  /* FIXME */

  return FALSE;
}

static gboolean
xtext2_button_press (GtkWidget *widget, GdkEventButton *event)
{
  XText2 *xtext = XTEXT2 (widget);
  textentry *ent;
  unsigned char *word;
  int line_x, x, y, offset, len;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  gdk_window_get_pointer (widget->window, &x, &y, 0);

  /* FIXME */

  if (event->button == 3)
  {
    /* right click */
    return FALSE;
  }

  if (event->button == 2)
  {
    /* middle click */
    return FALSE;
  }

  /* we only want the left button now */
  if (event->button != 1)
    return FALSE;

  if (event->type == GDK_2BUTTON_PRESS)
  {
    /* double-click, select word */
  }

  if (event->type == GDK_3BUTTON_PRESS)
  {
    /* triple-click, select line */
  }

  /* check if it was a separator bar click */
  if (xtext->priv->show_separator && f->indent)
  {
    line_x = f->indent - ((xtext->priv->spacewidth + 1) / 2);
    if (line_x == x || line_x == x + 1 || line_x == x - 1)
    {
      xtext->priv->moving_separator = TRUE;
      /* draw the separator line */
      draw_sep (xtext, -1);
      return FALSE;
    }
  }

  xtext->priv->button_down = TRUE;
  xtext->priv->select_start_x = x;
  xtext->priv->select_start_y = y;
  xtext->priv->select_start_adj = xtext->adj->value;

  return FALSE;
}

static gboolean
xtext2_motion_notify (GtkWidget *widget, GdkEventCrossing *event)
{
  XText2 *xtext = XTEXT2 (widget);
  int tmp, x, y, offset, len;
  unsigned char *word;
  textentry *word_ent;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  gdk_window_get_pointer (widget->window, &x, &y, 0);

  if (xtext->priv->moving_separator)
  {
    if (x < (3 * widget->allocation.width) / 5 && x > 15)
    {
      tmp = f->indent;
      f->indent = x;
      fix_indent (xtext);
      if (tmp != f->indent)
      {
	recalc_widths (xtext, FALSE);
	if (f->scrollbar_down)
	  gtk_adjustment_set_value (xtext->adj, xtext->adj->upper - xtext->adj->page_size);
	if (!xtext->priv->io_tag)
	  xtext->priv->io_tag = g_timeout_add (REFRESH_TIMEOUT, (GSourceFunc) adjustment_timeout, xtext);
      }
    }
    return FALSE;
  }

  if (xtext->priv->button_down)
  {
    gtk_grab_add (widget);
    xtext->priv->select_end_x = x;
    xtext->priv->select_end_y = y;
    /*
    selection_update (xtext, event, y);
    */
    return FALSE;
  }

#ifdef MOTION_MONITOR
#endif /* MOTION_MONITOR */

  /* FIXME */
  return FALSE;
}

#ifdef USE_XFT
/* ========================================= */
/* ========== XFT 1 and 2 BACKEND ========== */
/* ========================================= */
static void
backend_init (XText2 *xtext)
{
  if (xtext->priv->xft_draw == NULL)
  {
    xtext->priv->xft_draw = XftDrawCreate (
      GDK_WINDOW_XDISPLAY    (xtext->priv->draw_buffer),
      GDK_WINDOW_XWINDOW     (xtext->priv->draw_buffer),
      GDK_VISUAL_XVISUAL     (gdk_drawable_get_visual (xtext->priv->draw_buffer)),
      GDK_COLORMAP_XCOLORMAP (gdk_drawable_get_colormap (xtext->priv->draw_buffer))
    );
    XftDrawSetSubwindowMode (xtext->priv->xft_draw, IncludeInferiors);
  }
}

static void
backend_deinit (XText2 *xtext)
{
  if (xtext->priv->xft_draw)
  {
    XftDrawDestroy (xtext->priv->xft_draw);
    xtext->priv->xft_draw = NULL;
  }
}

inline static int
backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret)
{
  XGlyphInfo ext;

  if (*str < 128)
  {
    *mbl_ret = 1;
    return xtext->priv->fontwidth[*str];
  }
  mbl_ret = charlen (str);
  XftTextExtentsUtf8 (GDK_WINDOW_XDISPLAY (xtext->priv->draw_buffer), xtext->priv->font, str, *mbl_ret, &ext);

  return ext.xOff;
}

static void
backend_font_open (XText2 *xtext, char *name)
{
  XftFont *font = NULL;
  PangoFontDescription *fontd;
  Display *xdisplay = GDK_WINDOW_XDISPLAY (xtext->priv->draw_buffer);
  int weight, slant, screen = DefaultScreen (xdisplay);

  if (name[0] == '-')
  {
    xtext->priv->font = XftFontOpenXlfd (xdisplay, screen, name);
    if (xtext->priv->font)
      return;
  }

  fontd = pango_font_description_from_string (name);
  if (pango_font_description_get_size (fontd) != 0)
  {
    /* FIXME */
  }
  pango_font_description_free (fontd);

  if (font == NULL)
  {
    font = XftFontOpenName (xdisplay, screen, name);
    if (font == NULL)
      font = XftFontOpenName (xdisplay, screen, "sans-11");
  }
  xtext->priv->font->font = font;
}

static void
backend_font_close (XText2 *xtext)
{
  XftFontClose (GDK_WINDOW_XDISPLAY (xtext->priv->draw_buffer), xtext->priv->font);
  xtext->priv->font = NULL;
}

static int
backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte)
{
}

static void
backend_draw_text (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte)
{
}

#else
/* ======================================= */
/* ============ PANGO BACKEND ============ */
/* ======================================= */
static void
backend_init (XText2 *xtext)
{
  if (xtext->priv->layout == NULL)
  {
    xtext->priv->layout = gtk_widget_create_pango_layout (GTK_WIDGET (xtext), 0);
    if (xtext->priv->font)
      pango_layout_set_font_description (xtext->priv->layout, xtext->priv->font->font);
  }
}

static void
backend_deinit (XText2 *xtext)
{
  if (xtext->priv->layout)
  {
    g_object_unref (xtext->priv->layout);
    xtext->priv->layout = NULL;
  }
}

inline static int
backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret)
{
  int width;
  if (*str < 128)
  {
    *mbl_ret = 1;
    return xtext->priv->fontwidth[*str];
  }

  *mbl_ret = charlen (str);
  pango_layout_set_text (xtext->priv->layout, str, *mbl_ret);
  pango_layout_get_pixel_size (xtext->priv->layout, &width, NULL);

  return width;
}

static void
backend_font_open (XText2 *xtext, char *name)
{
  PangoLanguage *lang;
  PangoContext *context;
  PangoFontMetrics *metrics;

  xtext->priv->font = &xtext->priv->pango_font;

  xtext->priv->font->font = pango_font_description_from_string (name);
  if (!xtext->priv->font->font)
    xtext->priv->font->font = pango_font_description_from_string ("sans 11");

  if (xtext->priv->font->font)
  {
    if (pango_font_description_get_size (xtext->priv->font->font) == 0)
    {
      pango_font_description_free (xtext->priv->font->font);
      xtext->priv->font->font = pango_font_description_from_string ("sans 11");
    }
  }
  else
  {
    xtext->priv->font = NULL;
    return;
  }

  backend_init (xtext);
  pango_layout_set_font_description (xtext->priv->layout, xtext->priv->font->font);

  /* vte does it this way */
  context = gtk_widget_get_pango_context (GTK_WIDGET (xtext));
  lang = pango_context_get_language (context);
  metrics = pango_context_get_metrics (context, xtext->priv->font->font, lang);
  xtext->priv->font->ascent  = pango_font_metrics_get_ascent  (metrics) / PANGO_SCALE;
  xtext->priv->font->descent = pango_font_metrics_get_descent (metrics) / PANGO_SCALE;
  pango_font_metrics_unref (metrics);
}

static void
backend_font_close (XText2 *xtext)
{
  pango_font_description_free (xtext->priv->font->font);
  xtext->priv->font->font = NULL;
}

static int
backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte)
{
  int width;

  if (!multibyte)
    return text_width_8bit (xtext, str, len);

  if (str[0] == '\0')
    return 0;

  pango_layout_set_text (xtext->priv->layout, str, len);
  pango_layout_get_pixel_size (xtext->priv->layout, &width, NULL);

  return width;
}

static void
backend_draw_text (XText2 *xtext, gboolean fill, GdkGC *gc, int x, int y, char *str, int len, int width, gboolean multibyte)
{
  GdkGCValues val;
  GdkColor col;
  PangoLayoutLine *line;

  pango_layout_set_text (xtext->priv->layout, str, len);

  if (fill)
  {
#ifdef WIN32
    if (xtext->priv->transparent && !xtext->priv->backcolor)
    {
      win32_draw_bg (xtext, x, y - xtext->priv->font->ascent, width, xtext->priv->fontsize);
    }
    else
#endif
    {
      gdk_gc_get_values (gc, &val);
      col.pixel = val.background.pixel;
      gdk_gc_set_foreground (gc, &col);
      gdk_draw_rectangle (xtext->priv->draw_buffer, gc, 1, x, y - xtext->priv->font->ascent, width, xtext->priv->fontsize);
      col.pixel = val.foreground.pixel;
      gdk_gc_set_foreground (gc, &col);
    }
  }

  line = pango_layout_get_lines (xtext->priv->layout)->data;
  gdk_draw_layout_line_with_colors (xtext->priv->draw_buffer, gc, x, y, line, 0, 0);

  if (xtext->priv->overdraw)
    gdk_draw_layout_line_with_colors (xtext->priv->draw_buffer, gc, x, y, line, 0, 0);

  if (xtext->priv->bold)
    gdk_draw_layout_line_with_colors (xtext->priv->draw_buffer, gc, x + 1, y, line, 0, 0);
}

#endif

static void
paint (GtkWidget *widget, GdkRectangle *area)
{
  XText2 *xtext = XTEXT2 (widget);
  XTextFormat *f;
  textentry *ent_start, *ent_end;
  int x, y;

#if defined(USE_XLIB) || defined (WIN32)
  if (xtext->priv->transparent)
  {
    gdk_window_get_origin (widget->window, &x, &y);
    /* update transparency only if it moved */
    if (xtext->priv->last_win_x != x || xtext->priv->last_win_y != y)
    {
      xtext->priv->last_win_x = x;
      xtext->priv->last_win_y = y;
#if !defined(USE_SHM) && !defined(WIN32)
      if (xtext->priv->shaded)
      {
	/* FIXME
	xtext->priv->recycle = TRUE;
	load_trans (xtext);
	xtext->priv->recycle = FALSE;
	*/
      }
      else
#endif /* !USE_SHM && !WIN32 */
      {
	/* FIXME
	free_trans (xtext);
	load_trans (xtext);
	*/
      }
    }
  }
#endif /* USE_XLIB || WIN32 */

  if (area->x == 0 && area->y == 0 &&
      area->height == widget->allocation.height &&
      area->width  == widget->allocation.width)
  {
    dontscroll (xtext); /* force scrolling off */
    render_page (xtext);
    return;
  }

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  ent_start = find_char (xtext, area->x, area->y, NULL, NULL);
  if (!ent_start)
  {
    draw_bg (xtext, area->x, area->y, area->width, area->height);
    goto xit;
  }
  ent_end = find_char (xtext, area->x + area->width, area->y + area->height, NULL, NULL);
  if (!ent_end)
    ent_end = f->wrapped_last;

  /* can't set a clip here, because fgc/bgc are used to draw the DB too */
  xtext->priv->clip_x = area->x;
  xtext->priv->clip_x2 = area->x + area->width;
  xtext->priv->clip_y = area->y;
  xtext->priv->clip_y2 = area->y + area->height;

  /* y is the last pixel y location it rendered text at */
  y = render_ents (xtext, ent_start, ent_end);

  if (y && y < widget->allocation.height && !ent_end->next)
  {
    GdkRectangle rect;

    rect.x = 0;
    rect.y = y;
    rect.width = widget->allocation.width;
    rect.height = widget->allocation.height;

    /* fill any space below the last line that also intersects with the exposure rectangle */
    if (gdk_rectangle_intersect (area, &rect, &rect))
      draw_bg (xtext, rect.x, rect.y, rect.width, rect.height);
  }

  xtext->priv->clip_x = 0;
  xtext->priv->clip_x2 = 1000000;
  xtext->priv->clip_y = 0;
  xtext->priv->clip_y2 = 1000000;

xit:
  x = f->indent - ((xtext->priv->spacewidth + 1) / 2);
  if (area->x <= x)
    draw_sep (xtext, -1);
}

static void
render_page (XText2 *xtext)
{
  textentry *ent;
  int line = 0;
  int lines_max;
  int width;
  int height;
  int subline;
  int startline = xtext->adj->value;
  XTextFormat *f;

  if (!GTK_WIDGET_REALIZED (xtext))
    return;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (f->indent < MARGIN)
    f->indent = MARGIN;

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);

  if (width < 34 || height < xtext->priv->fontsize || width < f->indent + 32)
    return;

#ifdef SMOOTH_SCROLL
  xtext->priv->pixel_offset = (xtext->adj->value - startline) * xtext->priv->fontsize;
#else
  xtext->priv->pixel_offset = 0;
#endif

  subline = line = 0;
  ent = f->wrapped_first;

  if (startline > 0)
    ent = nth (xtext, startline, &subline);

  f->pagetop = ent;
  f->pagetop_subline = subline;
  f->pagetop_line = startline;

#ifdef SCROLL_HACK
  {
    int pos, overlap;
    GdkRectangle area;

    if (f->num_lines <= xtext->adj->page_size)
      dontscroll (xtext);

#ifdef SMOOTH_SCROLL
    pos = xtext->adj->value * xtext->priv->fontsize;
#else
    pos = startline * xtext->priv->fontsize;
#endif /* SMOOTH_SCROLL */

    overlap = xtext->priv->last_pixel_pos - pos;
    xtext->priv->last_pixel_pos = pos;

#ifdef USE_DB
#ifdef WIN32
    if (!xtext->priv->transparent && !xtext->priv->pixmap && abs (overlap) < height)
#else
    if (!xtext->priv->pixmap && abs (overlap) < height)
#endif /* WIN32 */
#else /* USE_DB */
#ifdef WIN32
    if (!xtext->priv->transparent && !xtext->priv->pixmap && abs (overlap) < height)
#else
    if (!xtext->priv->pixmap && abs (overlap) < height - (3 * xtext->priv->fontsize))
#endif /* WIN32 */
#endif /* USE_DB */
    {
      /* so the obscured regions are exposed */
      gdk_gc_set_exposures (xtext->priv->fgc, TRUE);
      if (overlap < 1)
      {
	int remainder;
	gdk_draw_drawable (xtext->priv->draw_buffer, xtext->priv->fgc, xtext->priv->draw_buffer, 0, -overlap, 0, 0, width, height + overlap);
	remainder = ((height - xtext->priv->font->descent) % xtext->priv->fontsize) + xtext->priv->font->descent;
	area.y = (height + overlap) - remainder;
	area.height = remainder - overlap;
      }
      else
      {
	gdk_draw_drawable (xtext->priv->draw_buffer, xtext->priv->fgc, xtext->priv->draw_buffer, 0, 0, 0, overlap, width, height - overlap);
	area.y = 0;
	area.height = overlap;
      }
      gdk_gc_set_exposures (xtext->priv->fgc, FALSE);

      g_print ("scroll hacking!\n");

      if (area.height > 0)
      {
	area.x = 0;
	area.width = width;
	g_print ("painting area: (%d, %d) -> (%d, %d)\n", area.x, area.y, area.width, area.height);
	paint (xtext, &area);
      }
      f->grid_dirty = TRUE;
      return;
    }
  }
#endif /* SCROLL_HACK */

  f->grid_dirty = FALSE;
  width -= MARGIN;
  lines_max = ((height + xtext->priv->pixel_offset) / xtext->priv->fontsize) + 1;

  while (ent)
  {
    reset (xtext, FALSE, TRUE);
    line += render_line (xtext, f, ent, line, lines_max, subline, width);
    subline = 0;

    if (line >= lines_max)
      break;

    ent = ent->next;
  }

  line = (xtext->priv->fontsize * line) - xtext->priv->pixel_offset;

  /* fill any space below the last line with our background GC */
  draw_bg (xtext, 0, line, width + MARGIN, height - line);

  /* draw the separator line */
  draw_sep (xtext, -1);
}

static int
render_page_timeout (XText2 *xtext)
{
  GtkAdjustment *adj = xtext->adj;
  XTextFormat *f;

  xtext->priv->add_io_tag = FALSE;
  xtext->priv->io_tag = 0;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  /* less than a complete page? */
  if (f->num_lines <= adj->page_size)
  {
    f->old_adj = 0;
    adj->value = 0;
    render_page (xtext);
  }
  else if (f->scrollbar_down)
  {
    g_signal_handler_block (xtext->adj, xtext->priv->vc_signal_tag);
    adjustment_set (xtext, FALSE);
    gtk_adjustment_set_value (adj, adj->upper - adj->page_size);
    g_signal_handler_unblock (xtext->adj, xtext->priv->vc_signal_tag);
    f->old_adj = adj->value;
    render_page (xtext);
  }
  else
  {
    adjustment_set (xtext, TRUE);
    if (xtext->priv->indent_changed)
    {
      xtext->priv->indent_changed = FALSE;
      render_page (xtext);
    }
  }

  return 0;
}

static int
render_line (XText2 *xtext, XTextFormat *f, textentry *ent, int line, int lines_max, int subline, int win_width)
{
  unsigned char *str;
  int indent, taken, entline, len, y;

  entline = taken = 0;
  str = ent->str;
  indent = ent->indent;

#ifdef XCHAT
  /* draw the timestamp */
  if (xtext->priv->auto_indent && xtext->priv->time_stamp && !xtext->priv->skip_stamp)
  {
    char *time_str;
    int stamp_size = get_stamp_str ("[%H:%M:%S] ", ent->stamp, &time_str);
    int tmp = ent->multibyte;
    y = (xtext->priv->fontsize * line) + xtext->priv->font->ascent - xtext->priv->pixel_offset;
    ent->multibyte = TRUE;
    render_str (xtext, f, y, ent, time_str, stamp_size, win_width, 2, line, TRUE);
    ent->multibyte = tmp;
    g_free (time_str);
  }
#endif

  /* draw each line one-by-one */
  do
  {
    /* if it's one of the first 4 wraps, we don't need to recalc it here. */
    if (entline < RECORD_WRAPS)
    {
      if (ent->lines_taken < 2)
      {
	len = ent->str_len;
      }
      else
      {
	if (entline > 0)
	  len = ent->wrap_offset[entline] - ent->wrap_offset[entline - 1];
	else
	  len = ent->wrap_offset[0];
      }
    }
    else
    {
      len = find_next_wrap (xtext, ent, str, win_width, indent);
    }
    entline++;
    y = (xtext->priv->fontsize * line) + xtext->priv->font->ascent + xtext->priv->pixel_offset;

    if (!subline)
    {
      if (!render_str (xtext, f, y, ent, str, len, win_width, indent, line, FALSE))
      {
	/* small optimization */
	return ent->lines_taken - subline;
      }
    }
    else
    {
      xtext->priv->dont_render = TRUE;
      render_str (xtext, f, y, ent, str, len, win_width, indent, line, FALSE);
      xtext->priv->dont_render = FALSE;
      subline--;
      line--;
      taken--;
    }

    indent = f->indent;
    line++;
    taken++;
    str += len;

    if (line >= lines_max)
      break;
  } while (str < ent->str + ent->str_len);

  return taken;
}

static int
render_str (XText2 *xtext, XTextFormat *f, int y, textentry *ent, unsigned char *str, int len, int win_width, int indent, int line, int left_only)
{
  GdkGC *gc;
  int i = 0, x = indent, j = 0;
  unsigned char *pstr = str;
  int col_num, tmp;
  int offset;
  gboolean mark = FALSE;
  int ret = 1;

  xtext->priv->in_hilight = FALSE;

  offset = str - ent->str;

  if (line >= 0 && line < 255)
    f->grid_offset[line] = offset;

  gc = xtext->priv->fgc;

  if (ent->mark_start != -1 &&
      ent->mark_start <= i + offset &&
      ent->mark_end > i + offset)
  {
    set_bg (xtext, gc, 16);
    set_fg (xtext, gc, 17);
    xtext->priv->backcolor = TRUE;
    mark = TRUE;
  }
#ifdef MOTION_MONITOR
  if (xtext->priv->hilight_start <= i + offset && xtext->priv->hilight_end > i + offset)
  {
    if (!xtext->priv->un_hilight)
    {
#ifdef COLOR_HILIGHT
      set_bg (xtext, gc, 2);
#else
      xtext->priv->underline = TRUE;
#endif /* COLOR_HILIGHT */
    }
    xtext->priv->in_hilight = TRUE;
  }
#endif /* MOTION_MONITOR */

  if (!xtext->priv->skip_border_fills && !xtext->priv->dont_render)
  {
    /* draw background to the left of the text */
    if (str == ent->str && indent > MARGIN && xtext->priv->time_stamp)
    {
      /* don't overwrite the timestamp */
      if (indent > xtext->priv->stamp_width)
	draw_bg (xtext, xtext->priv->stamp_width, y - xtext->priv->font->ascent, indent - xtext->priv->stamp_width, xtext->priv->fontsize);
    }
    else
    {
      /* fill the indent area with background gc */
      if (indent >= xtext->priv->clip_x)
	draw_bg (xtext, 0, y - xtext->priv->font->ascent, MIN (indent, xtext->priv->clip_x2), xtext->priv->fontsize);
    }
  }

  if (xtext->priv->jump_in_offset > 0 && offset < xtext->priv->jump_in_offset)
    xtext->priv->dont_render2 = TRUE;

  while (i < len)
  {
#ifdef MOTION_MONITOR
    if (xtext->priv->hilight_ent == ent && xtext->priv->hilight_start == (i + offset))
    {
      x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      pstr += j;
      j = 0;
      if (!xtext->priv->un_hilight)
      {
#ifdef COLOR_HILIGHT
	set_bg (xtext, gc, 2);
#else
	xtext->priv->underline = TRUE;
#endif /* COLOR_HILIGHT */
      }
      xtext->priv->in_hilight = TRUE;
    }
#endif /* MOTION_MONITOR */

    if ((xtext->priv->parsing_color && isdigit (str[i]) && xtext->priv->nc < 2) ||
        (xtext->priv->parsing_color && str[i] == ',' && isdigit (str[i+1]) && xtext->priv->nc < 3))
    {
      pstr++;
      if (str[i] == ',')
      {
	xtext->priv->parsing_backcolor = TRUE;
	if (xtext->priv->nc)
	{
	  xtext->priv->num[xtext->priv->nc] = 0;
	  xtext->priv->nc = 0;
	  col_num = atoi (xtext->priv->num);
	  if (col_num == 99) /* mIRC lameness */
	    col_num = 18;
	  else
	    col_num = col_num % 16;
	  xtext->priv->col_fore = col_num;
	  if (!mark)
	    set_fg (xtext, gc, col_num);
	}
      }
      else
      {
	xtext->priv->num[xtext->priv->nc] = str[i];
	if (xtext->priv->nc < 7)
	  xtext->priv->nc++;
      }
    }
    else
    {
      if (xtext->priv->parsing_color)
      {
	xtext->priv->parsing_color = FALSE;
	if (xtext->priv->nc)
	{
	  xtext->priv->num[xtext->priv->nc] = 0;
	  xtext->priv->nc = 0;
	  col_num = atoi (xtext->priv->num);
	  if (xtext->priv->parsing_backcolor)
	  {
	    if (col_num == 99) /* mIRC lameness */
	      col_num = 19;
	    else
	      col_num = col_num % 16;
	    if (col_num == 19)
	      xtext->priv->backcolor = FALSE;
	    else
	      xtext->priv->backcolor = TRUE;
	    if (!mark)
	      set_bg (xtext, gc, col_num);
            xtext->priv->col_back = col_num;
	  }
	  else
	  {
	    if (col_num == 99) /* mIRC lameness */
	      col_num = 18;
	    else
	      col_num = col_num % 16;
	    if (!mark)
	      set_fg (xtext, gc, col_num);
	    xtext->priv->col_fore = col_num;
	  }
	  xtext->priv->parsing_backcolor = FALSE;
	}
	else
	{
	  /* got a \003<non-digit>... - reset colors */
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  pstr += j;
	  j = 0;
	  reset (xtext, mark, FALSE);
	}
      }

      switch (str[i])
      {
	case '\n':
	  break;
	case ATTR_REVERSE:
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  pstr += j + 1;
	  j = 0;
	  tmp = xtext->priv->col_fore;
	  xtext->priv->col_fore = xtext->priv->col_back;
	  xtext->priv->col_back = tmp;
	  if (!mark)
	  {
	    set_fg (xtext, gc, xtext->priv->col_fore);
	    set_bg (xtext, gc, xtext->priv->col_fore);
	  }
	  if (xtext->priv->col_back != 19)
	    xtext->priv->backcolor = TRUE;
          else
	    xtext->priv->backcolor = FALSE;
	  break;
	case ATTR_BOLD:
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  xtext->priv->bold = !xtext->priv->bold;
	  pstr += j + 1;
	  j = 0;
	  break;
	case ATTR_UNDERLINE:
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  xtext->priv->underline = !xtext->priv->underline;
	  pstr += j + 1;
	  j = 0;
	  break;
	case ATTR_RESET:
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  pstr += j + 1;
	  j = 0;
	  reset (xtext, mark, !xtext->priv->in_hilight);
	  break;
	case ATTR_COLOR:
	  x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
	  xtext->priv->parsing_color = TRUE;
	  pstr += j + 1;
	  j = 0;
	  break;
	default:
	  tmp = charlen (str + i);
	  /* invalid utf8 safeguard */
	  if (tmp + i > len)
	    tmp = len - i;
	  j += tmp; /* move to the next utf8 character */
      }
    }
    i += charlen (str + i); /* move to the next utf8 char */
    /* invalid utf8 safeguard */
    if (i > len)
      i = len;

    /* have we been told to stop rendering at this point? */
    if (xtext->priv->jump_out_offset > 0 && xtext->priv->jump_out_offset <= (i + offset))
    {
      render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      ret = 0; /* skip the rest of the lines, we're done */
      j = 0;
      break;
    }

    if (xtext->priv->jump_in_offset > 0 && xtext->priv->jump_in_offset == (i + offset))
    {
      x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      pstr += j;
      j = 0;
      xtext->priv->dont_render2 = FALSE;
    }

#ifdef MOTION_MONITOR
    if (xtext->priv->hilight_ent == ent && xtext->priv->hilight_end == (i + offset))
    {
      x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      pstr += j;
      j = 0;
#ifdef COLOR_HILIGHT
      if (mark)
      {
	set_bg (xtext, gc, 16);
	xtext->priv->backcolor = TRUE;
      }
      else
      {
	set_bg (xtext, gc, xtext->priv->col_back);
	xtext->priv->backcolor = (xtext->col_back != 19)
      }
#else
      xtext->priv->underline = FALSE;
#endif /* COLOR_HILIGHT */
      xtext->priv->in_hilight = FALSE;
      if (xtext->priv->render_hilights_only)
      {
	/* stop drawing this ent */
	ret = 0;
	break;
      }
    }
#endif /* MOTION_MONITOR */
    if (!mark && ent->mark_start == (i + offset))
    {
      x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      pstr += j;
      j = 0;
      set_bg (xtext, gc, 16);
      set_fg (xtext, gc, 17);
      xtext->priv->backcolor = TRUE;
      mark = TRUE;
    }

    if (mark && ent->mark_end == (i + offset))
    {
      x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);
      pstr += j;
      j = 0;
      set_bg (xtext, gc, xtext->priv->col_back);
      set_fg (xtext, gc, xtext->priv->col_fore);
      xtext->priv->backcolor = (xtext->priv->col_back != 19);
      mark = FALSE;
    }
  }

  if (j)
    x += render_flush (xtext, x, y, pstr, j, gc, ent->multibyte);

  if (mark)
  {
    set_bg (xtext, gc, xtext->priv->col_back);
    set_fg (xtext, gc, xtext->priv->col_fore);
    xtext->priv->backcolor = (xtext->priv->col_back != 19);
  }

  /* draw background to the right of the text */
  if (!left_only && !xtext->priv->dont_render)
  {
    /* draw separator now so it doesn't appear to flicker */
    draw_sep (xtext, y - xtext->priv->font->ascent);
    if (!xtext->priv->skip_border_fills && xtext->priv->clip_x2 >= x)
    {
      int xx = MAX (x, xtext->priv->clip_x);
      int yy = MIN (xtext->priv->clip_x2 - xx, (win_width + MARGIN) - xx);
      draw_bg (xtext, xx, y - xtext->priv->font->ascent, yy, xtext->priv->fontsize);
    }
  }

  xtext->priv->dont_render2 = FALSE;
  return ret;
}

static int
render_flush (XText2 *xtext, int x, int y, unsigned char *str, int len, GdkGC *gc, gboolean multibyte)
{
  int str_width;
  gboolean fill;
  GdkDrawable *pix = NULL;
  int dest_x, dest_y;

  if (xtext->priv->dont_render || len < 1)
    return 0;

  str_width = backend_get_text_width (xtext, str, len, multibyte);

  if (xtext->priv->dont_render2)
    return str_width;

  /* roll-your-own clipping (avoiding XftDrawString is always good!) */
  if (x > xtext->priv->clip_x2 || x + str_width < xtext->priv->clip_x)
    return str_width;
  if (y - xtext->priv->font->ascent > xtext->priv->clip_y2 || (y - xtext->priv->font->ascent) + xtext->priv->fontsize < xtext->priv->clip_y)
    return str_width;

  if (xtext->priv->render_hilights_only)
  {
    if (!xtext->priv->in_hilight)
      return str_width;
#ifndef COLOR_HILIGHT
    if (!xtext->priv->un_hilight)
      goto dounder;
#endif /* COLOR_HILIGHT */
  }

#ifdef USE_DB
#ifdef WIN32
  if (!xtext->priv->transparent)
#endif /* WIN32 */
  {
    pix = gdk_pixmap_new (xtext->priv->draw_buffer, str_width, xtext->priv->fontsize, xtext->priv->depth);
    if (pix)
    {
#ifdef USE_XFT
      XftDrawChange (xtext->priv->xftdraw, GDK_WINDOW_XWINDOW (pix));
#endif /* USE_XFT */
      dest_x = x;
      dest_y = y - xtext->priv->font->ascent;
      gdk_gc_set_ts_origin (xtext->priv->bgc, xtext->priv->ts_x - x, xtext->priv->ts_y - dest_y);
      x = 0;
      y = xtext->priv->font->ascent;
      xtext->priv->draw_buffer = pix;
    }
  }
#endif /* USE_DB */

  fill = TRUE;

  /* backcolor is always handled by XDrawImageString */
  if (!xtext->priv->backcolor && xtext->priv->pixmap)
  {
    /* draw the background pixmap behind the text - CAUSES FLICKER HERE!! */
    draw_bg (xtext, x, y - xtext->priv->font->ascent, str_width, xtext->priv->fontsize);
    fill = FALSE; /* already drawn the background */
  }
  backend_draw_text (xtext, fill, gc, x, y, str, len, str_width, multibyte);

#ifdef USE_DB
  if (pix)
  {
    GdkRectangle clip;
    GdkRectangle dest;
    gdk_gc_set_ts_origin (xtext->priv->bgc, xtext->priv->ts_x, xtext->priv->ts_y);
    xtext->priv->draw_buffer = GTK_WIDGET (xtext)->window;
#ifdef USE_XFT
#endif /* USE_XFT */
    clip.x = xtext->priv->clip_x;
    clip.y = xtext->priv->clip_y;
    clip.width = xtext->priv->clip_x2 - xtext->priv->clip_x;
    clip.height = xtext->priv->clip_y2 - xtext->priv->clip_x;

    dest.x = dest_x;
    dest.y = dest_y;
    dest.width = str_width;
    dest.height = xtext->priv->fontsize;

    /* dump the buffer to window, within the clipping rect */
    if (gdk_rectangle_intersect (&clip, &dest, &dest))
      gdk_draw_drawable (xtext->priv->draw_buffer, xtext->priv->bgc, pix, dest.x - dest_x, dest.y - dest_y, dest.x, dest.y, dest.width, dest.height);
    g_object_unref (pix);
  }
#endif /* USE_DB */

  if (xtext->priv->underline)
  {
#ifdef USE_XFT
    GdkColor col;
#endif /* USE_XFT */

#ifndef COLOR_HILIGHT
dounder:
#endif /* COLOR_HILIGHT */
#ifdef USE_XFT
    col.pixel = xtext->priv->xft_fg->pixel;
    gdk_gc_set_foreground (gc, &col);
#endif /* USE_XFT */
    if (pix)
      y = dest_y + xtext->priv->font->ascent + 1;
    else
    {
      y++;
      dest_x = x;
    }
    /* draw directly to window, it's out of the range of our DB */
    gdk_draw_line (xtext->priv->draw_buffer, gc, dest_x, y, dest_x + str_width - 1, y);
  }
  return str_width;
}

static int
render_ents (XText2 *xtext, textentry *start, textentry *stop)
{
  textentry *ent, *orig_ent, *tmp_ent;
  int line;
  int lines_max;
  int width;
  int height;
  int subline;
  gboolean drawing = FALSE;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (f->indent < MARGIN)
    f->indent = MARGIN;

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);
  width -= MARGIN;

  if (width < 32 || height < xtext->priv->fontsize || width < f->indent + 30)
    return 0;

  lines_max = ((height + xtext->priv->pixel_offset) / xtext->priv->fontsize) + 1;
  line = 0;
  orig_ent = f->pagetop;
  subline = f->pagetop_subline;

  /* used before a complete page is in buffer */
  if (orig_ent == NULL)
    orig_ent = f->wrapped_first;

  /* check if start is before the start of this page */
  if (stop)
  {
    tmp_ent = orig_ent;
    while (tmp_ent)
    {
      if (tmp_ent == start)
	break;
      if (tmp_ent == stop)
      {
	drawing = TRUE;
	break;
      }
      tmp_ent = tmp_ent->next;
    }
  }

  ent = orig_ent;
  while (ent)
  {
    if (stop && ent == start)
      drawing = TRUE;

    if (drawing || ent == stop || ent == start)
    {
      reset (xtext, FALSE, TRUE);
      line += render_line (xtext, f, ent, line, lines_max, subline, width);
      subline = 0;
      xtext->priv->jump_in_offset = 0;
    }
    else
    {
      if (ent == orig_ent)
      {
	line -= subline;
	subline = 0;
      }
      line += ent->lines_taken;
    }

    if (ent == stop)
      break;

    if (line >= lines_max)
      break;

    ent = ent->next;
  }

  /* space below last line */
  return (xtext->priv->fontsize * line) - xtext->priv->pixel_offset;
}

static void
draw_bg (XText2 *xtext, int x, int y, int width, int height)
{
  gdk_draw_rectangle (xtext->priv->draw_buffer, xtext->priv->bgc, 1, x, y, width, height);
}

static void
draw_sep (XText2 *xtext, int y)
{
  int x, height;
  GdkGC *light, *dark;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (y == -1)
  {
    y = 0;
    height = GTK_WIDGET (xtext)->allocation.height;
  }
  else
  {
    height = xtext->priv->fontsize;
  }

  /* draw the separator line */
  if (xtext->priv->show_separator && f->indent)
  {
    light = xtext->priv->light_gc;
    dark  = xtext->priv->dark_gc;

     x = f->indent - ((xtext->priv->spacewidth + 1) / 2);
     if (x < 1)
       return;
     if (xtext->priv->thinline)
     {
       if (xtext->priv->moving_separator)
	 gdk_draw_line (xtext->priv->draw_buffer, light, x, y, x, y + height);
       else
	 gdk_draw_line (xtext->priv->draw_buffer, xtext->priv->thin_gc, x, y, x, y + height);
     }
     else
     {
       if (xtext->priv->moving_separator)
       {
	 gdk_draw_line (xtext->priv->draw_buffer, light, x - 1, y, x - 1, y + height);
	 gdk_draw_line (xtext->priv->draw_buffer, dark, x, y, x, y + height);
       }
       else
       {
	 gdk_draw_line (xtext->priv->draw_buffer, dark, x - 1, y, x - 1, y + height);
	 gdk_draw_line (xtext->priv->draw_buffer, light, x, y, x, y + height);
       }
     }
  }
}

static textentry*
nth (XText2 *xtext, int line, int *subline)
{
  int lines = 0;
  textentry *ent;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  ent = f->wrapped_first;

  /* --optimization-- try to make a short-cut using the pagetop ent */
  if (f->pagetop)
  {
    if (line == f->pagetop_line)
    {
      *subline = f->pagetop_subline;
      return f->pagetop;
    }
    else if (line > f->pagetop_line)
    {
      /* start from the pagetop instead of the absolute beginning */
      ent = f->pagetop;
      lines = f->pagetop_line - f->pagetop_subline;
    }
    else if (line > f->pagetop_line - line)
    {
      /* move backwards from pagetop */
      ent = f->pagetop;
      lines = f->pagetop_line - f->pagetop_subline;

      while (1)
      {
	if (line <= lines)
	{
	  *subline = line - lines;
	  return ent;
	}
	ent = ent->prev;
	if (!ent)
	  break;
	lines -= ent->lines_taken;
      }
      return NULL;
    }
  }

  while (ent)
  {
    lines += ent->lines_taken;
    if (lines > line)
    {
      *subline = ent->lines_taken - (lines - line);
      return ent;
    }
    ent = ent->next;
  }

  return NULL;
}

static textentry*
find_char (XText2 *xtext, int x, int y, int *offset, gboolean *out_of_bounds)
{
  textentry *ent;
  int line;
  int subline;

  line = (y + xtext->priv->pixel_offset) / xtext->priv->fontsize;
  ent = nth (xtext, line + (int) xtext->adj->value, &subline);
  if (!ent)
    return NULL;
  if (offset)
    *offset = find_x (xtext, x, ent, subline, line, out_of_bounds);
  return ent;
}

static int
find_x (XText2 *xtext, int x, textentry *ent, int subline, int line, gboolean *out_of_bounds)
{
  int indent;
  unsigned char *str;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (subline < 1)
    indent = ent->indent;
  else
    indent = f->indent;

  if (line > xtext->adj->page_size || line < 0)
    return 0;

  if (f->grid_dirty || line > 255)
  {
    str = ent->str + find_subline (xtext, ent, subline);
    if (str >= ent->str + ent->str_len)
      return 0;
  }
  else
  {
    if (f->grid_offset[line] > ent->str_len)
      return 0;
    str = ent->str + f->grid_offset[line];
  }

  if (x < indent)
  {
    *out_of_bounds = TRUE;
    return (str - ent->str);
  }

  *out_of_bounds = FALSE;

  {
    int xx = indent;
    int i = 0;
    gboolean col = FALSE;
    int nc = 0;
    unsigned char *orig = str;
    unsigned char *text = str;
    int mbl;

    while (*text)
    {
      mbl = 1;
      if ((col && isdigit (*text) && nc < 2) ||
	  (col && text[0] == ',' && isdigit (*(text+1)) && nc < 3))
      {
	nc++;
	if (*text == ',')
	  nc = 0;
	text++;
      }
      else
      {
	col = FALSE;
	switch (*text)
	{
	  case ATTR_COLOR:
	    col = TRUE;
	    nc = 0;
	  case ATTR_BEEP:
	  case ATTR_RESET:
	  case ATTR_REVERSE:
	  case ATTR_BOLD:
	  case ATTR_UNDERLINE:
	    text++;
	    break;
	  default:
	    xx += backend_get_char_width (xtext, text, &mbl);
	    text += mbl;
	    if (xx >= x)
	      return i + (orig - ent->str);
	}
      }
      i += mbl;
      if (text - orig >= ent->str_len)
	return ent->str_len;
    }
  }
  return ent->str_len;
}

static int
find_subline (XText2 *xtext, textentry *ent, int line)
{
  int win_width;
  unsigned char *str;
  int indent, str_pos, line_pos, len;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  if (ent->lines_taken < 2 || line < 1)
    return 0;

  /* we record the first 4 lines' wraps, so take a shortcut */
  if (line <= RECORD_WRAPS)
    return ent->wrap_offset[line - 1];

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &win_width, NULL);
  win_width -= MARGIN;

  /* start from the last recorded wrap and move forward */
  indent = f->indent;
  str_pos = ent->wrap_offset[RECORD_WRAPS - 1];
  str = str_pos + ent->str;
  line_pos = RECORD_WRAPS;

  do
  {
    len = find_next_wrap (xtext, ent, str, win_width, indent);
    indent = f->indent;
    str += len;
    str_pos += len;
    line_pos++;
    if (line_pos >= line)
      return str_pos;
  } while (str < ent->str + ent->str_len);

  return 0;
}

void
xtext2_set_palette (XText2 *xtext, GdkColor palette[])
{
  int i;

  for (i = 0; i <= 19; i++)
  {
#ifdef USE_XFT
    xtext->priv->color[i].color.red   = palette[i].red;
    xtext->priv->color[i].color.green = palette[i].green;
    xtext->priv->color[i].color.blue  = palette[i].blue;
    xtext->priv->color[i].color.alpha = 0xffff;
    xtext->priv->color[i].color.pixel = palette[i].pixel;
#endif
    xtext->priv->palette[i] = palette[i].pixel;
  }

  if (GTK_WIDGET_REALIZED (xtext))
  {
    set_fg (xtext, xtext->priv->fgc, 18);
    set_bg (xtext, xtext->priv->fgc, 19);
    set_fg (xtext, xtext->priv->bgc, 19);
  }

  xtext2_refresh (xtext);
}

static void
set_fg (XText2 *xtext, GdkGC *gc, int index)
{
  GdkColor col;
  col.pixel = xtext->priv->palette[index];
  gdk_gc_set_foreground (gc, &col);

#ifdef USE_XFT
  if (gc == xtext->priv->fgc)
    xtext->xft_fg = &xtext->color[index];
  else
    xtext->xft_bg = &xtext->color[index];
#endif
}

static void
set_bg (XText2 *xtext, GdkGC *gc, int index)
{
  GdkColor col;
  col.pixel = xtext->priv->palette[index];
  gdk_gc_set_background (gc, &col);
#ifdef USE_XFT
  xtext->xft_bg = &xtext->color[index];
#endif
}

gboolean
xtext2_set_font (XText2 *xtext, char *name)
{
  int i;
  unsigned char c;
  if (xtext->priv->font)
    backend_font_close (xtext);

  /* realize so that the font has an open display */
  gtk_widget_realize (GTK_WIDGET (xtext));

  backend_font_open (xtext, name);
  if (xtext->priv->font == NULL)
    return FALSE;

  /* measure the width of ASCII chars for fast access */
  for (i = 0; i < sizeof (xtext->priv->fontwidth) / sizeof (xtext->priv->fontwidth[0]); i++)
  {
    c = i;
    xtext->priv->fontwidth[i] = backend_get_text_width (xtext, &c, 1, TRUE);
  }
  xtext->priv->spacewidth = xtext->priv->fontwidth[' '];
  xtext->priv->fontsize = xtext->priv->font->ascent + xtext->priv->font->descent;

#ifdef XCHAT
  {
    char *time_str;
    int stamp_size = get_stamp_str ("[%H:%M:%S] ", time (NULL), &time_str);
    xtext->priv->stamp_width = backend_get_text_width (xtext, time_str, stamp_size, FALSE);
    g_free (time_str);
  }
#endif
  fix_indent (xtext);
  recalc_widths (xtext, TRUE);

  return TRUE;
}

static void
buffer_destruction_notify (XText2 *xtext, XTextBuffer *buffer)
{
  XTextFormat *f;

  if (xtext->priv->current_buffer == buffer)
    xtext2_show_buffer (xtext, xtext->priv->original_buffer);
  if (xtext->priv->selection_buffer == buffer)
    xtext->priv->selection_buffer = NULL;

  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);
  g_hash_table_remove (xtext->priv->buffer_info, buffer);

  g_signal_handler_disconnect (G_OBJECT (buffer), f->append_handler);
  g_signal_handler_disconnect (G_OBJECT (buffer), f->clear_handler);
  g_signal_handler_disconnect (G_OBJECT (buffer), f->remove_handler);

  g_free (f);
}

static void
buffer_append (XTextBuffer *buffer, textentry *ent, XText2 *xtext)
{
  XTextFormat *f;
  textentry *newent;
  int space;
  int left_width;

  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);

  newent = g_memdup (ent, sizeof(textentry));
  /* append to the linked list */
  if (f->wrapped_last)
    f->wrapped_last->next = newent;
  else
    f->wrapped_first = newent;
  newent->prev = f->wrapped_last;
  f->wrapped_last = newent;

  newent->str_width = text_width (xtext, newent->str, newent->str_len, NULL);

  newent->lines_taken = count_lines_taken (xtext, newent);
  f->num_lines += newent->lines_taken;

  if (xtext->priv->current_buffer == buffer)
  {
#ifdef SCROLL_HACK
    /* this could be improved */
    if ((f->num_lines - 1) <= xtext->adj->page_size)
      dontscroll (xtext);
#endif /* SCROLL_HACK */
      if (!xtext->priv->add_io_tag)
      {
	/* remove scrolling events */
	if (xtext->priv->io_tag)
	{
	  g_source_remove (xtext->priv->io_tag);
	  xtext->priv->io_tag = 0;
	}
	xtext->priv->io_tag = g_timeout_add (REFRESH_TIMEOUT * 2, (GSourceFunc) render_page_timeout, xtext);
      }
  }
  else if (f->scrollbar_down)
  {
    f->old_adj = f->num_lines - xtext->adj->page_size;
    if (f->old_adj < 0)
      f->old_adj = 0;
  }

  /* if we're not indented, we're done */
  if (newent->left_len <= 0)
  {
    newent->indent = 0;
    return;
  }

  left_width = text_width (xtext, newent->str, newent->left_len, NULL);

  newent->indent = (f->indent - left_width) - xtext->priv->spacewidth;
  if (xtext->priv->time_stamp)
    space = xtext->priv->stamp_width;
  else
    space = 0;

  /* do we need to auto-adjust the separator position? */
  if (xtext->priv->auto_indent && newent->indent < MARGIN + space)
  {
    int tempindent = MARGIN + space + xtext->priv->spacewidth + left_width;

    if (tempindent > f->indent)
      f->indent = tempindent;
    if (f->indent > xtext->priv->max_auto_indent)
      f->indent = xtext->priv->max_auto_indent;

    fix_indent (xtext);

    newent->indent = (f->indent - left_width) - xtext->priv->spacewidth;
    xtext->priv->indent_changed = TRUE;
    recalc_widths (xtext, FALSE);
  }
}

static void
buffer_clear (XTextBuffer *buffer, XText2 *xtext)
{
  XTextFormat *f;
  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);
  f->wrapped_first = buffer->text_first;
  f->wrapped_last  = buffer->text_last;
}

static void
buffer_remove (XTextBuffer *buffer, XText2 *xtext)
{
  XTextFormat *f;
  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);
  f->wrapped_first = buffer->text_first;
  f->wrapped_last  = buffer->text_last;
}

static XTextFormat*
allocate_buffer (XText2 *xtext, XTextBuffer *buffer)
{
  XTextFormat *f = g_new0 (XTextFormat, 1);
  g_hash_table_insert (xtext->priv->buffer_info, buffer, f);
  /* FIXME: should copy existing lines & set wraps here */
  f->old_adj = -1;
  f->wrapped_first = NULL;
  f->wrapped_last  = NULL;
  f->scrollbar_down = TRUE;
  f->indent = xtext->priv->spacewidth * 2;
  f->append_handler = g_signal_connect (G_OBJECT (buffer), "append", G_CALLBACK (buffer_append), (gpointer) xtext);
  f->clear_handler  = g_signal_connect (G_OBJECT (buffer), "clear",  G_CALLBACK (buffer_clear),  (gpointer) xtext);
  f->remove_handler = g_signal_connect (G_OBJECT (buffer), "remove", G_CALLBACK (buffer_remove), (gpointer) xtext);
  g_object_weak_ref (G_OBJECT (buffer), (GWeakNotify) buffer_destruction_notify, xtext);
  return f;
}

void
xtext2_show_buffer (XText2 *xtext, XTextBuffer *buffer)
{
  XTextFormat *f;
  int w, h;

  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);

  if (xtext->priv->current_buffer == buffer)
    return;

  if (xtext->priv->add_io_tag)
  {
    xtext->priv->add_io_tag = 0;
  }
  if (xtext->priv->io_tag)
  {
    g_source_remove (xtext->priv->io_tag);
    xtext->priv->io_tag = 0;
  }

  if (!GTK_WIDGET_REALIZED (GTK_WIDGET (xtext)))
    gtk_widget_realize (GTK_WIDGET (xtext));

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &w, &h);

  if (f == NULL)
  {
    /* this isn't a buffer we've seen before */
    f = allocate_buffer (xtext, buffer);
  }

  /* FIXME: recalc stuff! */
  xtext->priv->current_buffer = buffer;
  xtext->adj->value = f->old_adj;
  xtext->adj->upper = f->num_lines;
  if (xtext->adj->upper == 0)
    xtext->adj->upper = 1;
  else if (xtext->adj->value > xtext->adj->upper - xtext->adj->page_size)
  {
    xtext->adj->value = xtext->adj->upper - xtext->adj->page_size;
    if (xtext->adj->value < 0)
      xtext->adj->value = 0;
  }

  if (f->window_width != w)
  {
    f->window_width = w;
    calc_lines (xtext, FALSE);
    if (f->scrollbar_down)
      gtk_adjustment_set_value (xtext->adj, xtext->adj->upper - xtext->adj->page_size);
  } else if (f->window_height != h)
  {
    f->window_height = h;
    f->pagetop = NULL;
    adjustment_set (xtext, FALSE);
  }

  render_page (xtext);
  gtk_adjustment_changed (xtext->adj);
}

void
xtext2_refresh (XText2 *xtext)
{
  if (GTK_WIDGET_REALIZED (GTK_WIDGET (xtext)))
  {
#if defined(USE_XLIB) || defined(WIN32)
    if (xtext->priv->transparent)
    {
      /* FIXME
      free_trans (xtext);
      load_trans (xtext);
      */
    }
#endif
    render_page (xtext);
  }
}

static int
text_width (XText2 *xtext, unsigned char *str, int len, gboolean *mb_ret)
{
  unsigned char *new_buf;
  int new_len, mb;
  new_buf = strip_color (str, len, xtext->priv->scratch_buffer, &new_len, &mb);

  if (mb_ret)
    *mb_ret = mb;

  return backend_get_text_width (xtext, new_buf, new_len, mb);
}

static int
text_width_8bit (XText2 *xtext, unsigned char *str, int len)
{
  /* gives width of an 8 bit string - with no mIRC codes in it */
  int width = 0;

  while (len)
  {
    width += xtext->priv->fontwidth[*str];
    str++;
    len--;
  }
  return width;
}

static unsigned char*
strip_color (unsigned char *text, int len, unsigned char *outbuf, int *newlen, gboolean *mb_ret)
{
  int nc = 0;
  int i = 0;
  gboolean col = FALSE;
  unsigned char *new_str;
  int mbl;
  gboolean mb = FALSE;

  if (outbuf == NULL)
    new_str = malloc (len + 2);
  else
    new_str = outbuf;

  while (len > 0)
  {
    if (text[0] >= 128)
      mb = TRUE;
    if ((col && isdigit (text[0]) && nc < 2) ||
        (col && text[0] == ',' && isdigit (*(text+1)) && nc < 3))
    {
      nc++;
      if (text[0] == ',')
	nc = 0;
    }
    else
    {
      col = FALSE;
      switch (text[0])
      {
	case ATTR_COLOR:
	  col = TRUE;
	  nc = 0;
	  break;
	case ATTR_BEEP:
	case ATTR_RESET:
	case ATTR_REVERSE:
	case ATTR_BOLD:
	case ATTR_UNDERLINE:
	  break;
	default:
	  mbl = charlen (text);
	  if (mbl == 1)
	  {
	    new_str[i] = text[0];
	    i++;
	    text++;
	    len--;
	  }
	  else
	  {
	    mb = TRUE;
	    len -= mbl;
	    /* safeguard against invalid utf8 */
	    /* avoid memcpy beyond buffer */
	    if (len < 0)
	      mbl += len;
	    memcpy (&new_str[i], text, mbl);
	    i += mbl;
	    text += mbl;
	  }
	  continue;
      }
    }
    text++;
    len--;
  }
  new_str[i] = 0;
  if (newlen != NULL)
    *newlen = i;
  if (mb_ret != NULL)
    *mb_ret = mb;
  return new_str;
}

static int
count_lines_taken (XText2 *xtext, textentry *ent)
{
  /* count how many lines 'ent' will take (with wraps) */
  unsigned char *str;
  int indent, taken, len;
  int win_width;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &win_width, NULL);
  win_width -= MARGIN;

  if (ent->str_width + ent->indent < win_width)
    return 1;

  indent = ent->indent;
  str = ent->str;
  taken = 0;

  do
  {
    len = find_next_wrap (xtext, ent, str, win_width, indent);
    if (taken < RECORD_WRAPS)
      ent->wrap_offset[taken] = (str + len) - ent->str;
    indent = f->indent;
    taken++;
    str += len;
  } while (str < ent->str + ent->str_len);
  return taken;
}

static int
find_next_wrap (XText2 *xtext, textentry *ent, unsigned char *str, int win_width, int indent)
{
  unsigned char *last_space = str;
  unsigned char *orig_str = str;
  int str_width = indent;
  gboolean col = FALSE;
  int nc = 0;
  int mbl;
  int ret;
  int limit_offset = 0;

  /* single liners */
  if (win_width > ent->str_width + ent->indent)
    return ent->str_len;

  /* it does happen! */
  if (win_width < 1)
  {
    ret = ent->str_len - (str - ent->str);
    goto done;
  }

  while (1)
  {
    if ((col && isdigit (*str) && nc < 2) ||
        (col && *str == ',' && isdigit (*(str+1)) && nc < 3))
    {
      nc++;
      if (*str == ',')
	nc = 0;
      limit_offset++;
      str++;
    }
    else
    {
      col = FALSE;
      switch (*str)
      {
	case ATTR_COLOR:
	  col = TRUE;
	  nc = 0;
	case ATTR_BEEP:
	case ATTR_RESET:
	case ATTR_REVERSE:
	case ATTR_BOLD:
	case ATTR_UNDERLINE:
	  limit_offset++;
	  str++;
	  break;
	default:
	  str_width += backend_get_char_width (xtext, str, &mbl);
	  if (str_width > win_width)
	  {
	    if (xtext->priv->word_wrap)
	    {
	      if (str - last_space > WORDWRAP_LIMIT + limit_offset)
	      {
		/* fall back to character wrap */
		ret = str - orig_str;
	      }
	      else
	      {
		if (*last_space == ' ')
		  last_space++;
		ret = last_space - orig_str;
		if (ret == 0) /* fall back to character wrap */
		  ret = str - orig_str;
	      }
	      goto done;
	    }
	    ret = str - orig_str;
	    goto done;
	  }

	  /* keep a record of the last space, for word wrapping */
	  if (is_del (*str))
	  {
	    last_space = str;
	    limit_offset = 0;
	  }

	  /* progress to the next character */
	  str += mbl;
      }
    }

    if (str >= ent->str + ent->str_len)
    {
      ret = str - orig_str;
      goto done;
    }
  }

done:
  /* must make progress */
  if (ret < 1)
    ret = 1;

  return ret;
}

static void
recalc_widths (XText2 *xtext, gboolean do_str_width)
{
  textentry *ent;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  /* since we have a new font, we have to recalc the text widths */
  ent = f->wrapped_first;
  while (ent)
  {
    if (do_str_width)
    {
      ent->str_width = text_width (xtext, ent->str, ent->str_len, NULL);
    }
    if (ent->left_len != -1)
    {
      ent->indent = (f->indent - text_width (xtext, ent->str, ent->left_len, NULL)) - xtext->priv->spacewidth;
      if (ent->indent < MARGIN)
	ent->indent = MARGIN;
    }
    ent = ent->next;
  }

  calc_lines (xtext, FALSE);
}

static void
calc_lines (XText2 *xtext, gboolean fire_signal)
{
  textentry *ent;
  int width;
  int height;
  int lines;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);
  width -= MARGIN;

  if (width < 30 || height < xtext->priv->fontsize || width < f->indent + 30)
    return;

  lines = 0;
  ent = f->wrapped_first;
  while (ent)
  {
    ent->lines_taken = count_lines_taken (xtext, ent);
    lines += ent->lines_taken;
    ent = ent->next;
  }

  f->pagetop = NULL;
  f->num_lines = lines;
  adjustment_set (xtext, fire_signal);
}

static void
fix_indent (XText2 *xtext)
{
  int j;
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

  /* make indent a multiple of the space width */
  if (f->indent && xtext->priv->spacewidth)
  {
    j = 0;
    while (j < f->indent)
    {
      j += xtext->priv->spacewidth;
    }
    f->indent = j;
  }

  /* force scrolling off */
  dontscroll (xtext);
}

static void
reset (XText2 *xtext, gboolean mark, gboolean attribs)
{
  if (attribs)
  {
    xtext->priv->underline = FALSE;
    xtext->priv->bold = FALSE;
  }
  if (!mark)
  {
    xtext->priv->backcolor = FALSE;
    if (xtext->priv->col_fore != 18)
      set_fg (xtext, xtext->priv->fgc, 18);
    if (xtext->priv->col_back != 19)
      set_bg (xtext, xtext->priv->fgc, 19);
  }
  xtext->priv->col_fore = 18;
  xtext->priv->col_back = 19;
  xtext->priv->parsing_color = FALSE;
  xtext->priv->parsing_backcolor = FALSE;
  xtext->priv->nc = 0;
}

static void
adjustment_set (XText2 *xtext, gboolean fire_signal)
{
  GtkAdjustment *adj = xtext->adj;
  XTextFormat *f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);
  adj->lower = 0;
  adj->upper = f->num_lines;

  if (adj->upper == 0)
    adj->upper = 1;

  adj->page_size = (GTK_WIDGET (xtext)->allocation.height - xtext->priv->font->descent) / xtext->priv->fontsize;
  adj->page_increment = adj->page_size;

  if (adj->value > adj->upper - adj->page_size)
    adj->value = adj->upper - adj->page_size;

  if (adj->value < 0)
    adj->value = 0;

  if (fire_signal)
    gtk_adjustment_changed (adj);
}

static void
adjustment_changed (GtkAdjustment *adj, XText2 *xtext)
{
  XTextFormat *f;
  f = g_hash_table_lookup (xtext->priv->buffer_info, xtext->priv->current_buffer);

#ifdef SMOOTH_SCROLL
  if (f->old_adj != adj->value)
#else
  if ((int) f->old_adj != (int) adj->value)
#endif /* SMOOTH_SCROLL */
  {
    f->scrollbar_down = (adj->value >= adj->upper - adj->page_size);

    if (adj->value + 1 == f->old_adj || adj->value - 1 == f->old_adj)
    {
      /* clicked an arrow? */
      if (xtext->priv->io_tag)
      {
	g_source_remove (xtext->priv->io_tag);
	xtext->priv->io_tag = 0;
      }
      render_page (xtext);
    }
    else
    {
      if (!xtext->priv->io_tag)
	xtext->priv->io_tag = g_timeout_add (REFRESH_TIMEOUT, (GSourceFunc) adjustment_timeout, xtext);
    }
  }
  f->old_adj = adj->value;
}

static gint
adjustment_timeout (XText2 *xtext)
{
  render_page (xtext);
  xtext->priv->io_tag = 0;
  return 0;
}
