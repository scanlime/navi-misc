#include "xtext2.h"
#include <string.h>

static void       xtext2_class_init      (XText2Class *klass);
static void       xtext2_init            (XText2 *xtext);

static void       xtext2_dispose         (GObject *object);
static void       xtext2_finalize        (GObject *object);
static void       xtext2_set_property    (GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);
static void       xtext2_get_property    (GObject *object, guint param_id, GValue *value, GParamSpec *pspec);

static void       xtext2_realize         (GtkWidget *widget);
static void       xtext2_unrealize       (GtkWidget *widget);
static void       xtext2_size_request    (GtkWidget *widget, GtkRequisition *requisition);
static void       xtext2_size_allocate   (GtkWidget *widget, GtkAllocation *allocation);
static gboolean   xtext2_expose          (GtkWidget *widget, GdkEventExpose *event);

static void       backend_init           (XText2 *xtext);
static void       backend_deinit         (XText2 *xtext);
inline static int backend_get_char_width (XText2 *xtext, unsigned char *str, int *mbl_ret);
static void       backend_font_open      (XText2 *xtext, char *name);
static void       backend_font_close     (XText2 *xtext);
static int        backend_get_text_width (XText2 *xtext, char *str, int len, gboolean multibyte);

static void       paint                  (GtkWidget *widget, GdkRectangle *area);
static void       render_page            (XText2 *xtext);
static void       draw_bg                (XText2 *xtext, int x, int y, int width, int height);
static void       draw_sep               (XText2 *xtext, int y);
static textentry* nth                    (XText2 *xtext, int line, int *subline);
static textentry* find_char              (XText2 *xtext, int x, int y, int *offset, gboolean *out_of_bounds);
static int        find_x                 (XText2 *xtext, int x, textentry *ent, int subline, int line, gboolean *out_of_bounds);
static void       set_fg                 (XText2 *xtext, GdkGC *gc, int index);
static void       set_bg                 (XText2 *xtext, GdkGC *gc, int index);
static int        text_width_8bit        (XText2 *xtext, unsigned char *str, int len);

static int        count_lines_taken      (XText2 *xtext, textentry *ent);
static int        find_next_wrap         (XText2 *xtext, textentry *ent, unsigned char *str, int win_width, int indent);
static void       recalc_widths          (XText2 *xtext, gboolean do_str_width);
static void       fix_indent             (XText2 *xtext);

static gpointer parent_class;

#define MARGIN 2 /* left margin (in pixels) */
#define WORDWRAP_LIMIT 24

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

  /* view settings */
  gboolean     indent;               /* indent text? */
  gboolean     show_separator;       /* show the separator bar? */
  GdkColor     tint;                 /* tint color */
  gboolean     word_wrap;            /* wrap words? */
  int          fontsize;             /* width in pixels of the space ' ' character */
  gulong       palette[20];          /* color palette */

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
  int          window_width;         /* width of the window when last rendered */
  int          window_height;        /* height of the window when last rendered */

  /* state associated with rendering specific buffers */
  GHashTable  *buffer_info;          /* stores an XTextFormat for each buffer we observe */
  XTextBuffer *current_buffer;
  XTextBuffer *original_buffer;
  XTextBuffer *selection_buffer;

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

typedef struct _XTextFormat XTextFormat;
struct _XTextFormat
{
  int   window_width;  /* window size when */
  int   window_height; /* last rendered */
  int   indent;        /* indent value */

  textentry *wrapped_first;
  textentry *wrapped_last;

  /* handlers */
  guint append_handler;
  guint clear_handler;
  guint remove_handler;
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
  g_print ("xtext2_init() - buffer_info is 0x%x\n", xtext->priv->buffer_info);

  gtk_widget_set_double_buffered (GTK_WIDGET (xtext), FALSE);
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

  xtext->priv = g_new0 (XText2Private, 1);

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
  set_fg (xtext, xtext->priv->fgc, 19);

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

  /* FIXME: buffer madness */

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
  {
    /* FIXME: stuff! */
    xtext->priv->window_width = allocation->width;
    xtext->priv->window_height = allocation->height;
    gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
  }
}

static gboolean
xtext2_expose (GtkWidget *widget, GdkEventExpose *event)
{
  paint (widget, &event->area);
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
      font = XftFontOpenName (xdisplay, screen, "sans-12");
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

#endif

static void
paint (GtkWidget *widget, GdkRectangle *area)
{
  XText2 *xtext = XTEXT2 (widget);
  textentry *ent_start, *ent_end;
  int x, y;

  ent_start = find_char (xtext, area->x, area->y, NULL, NULL);
  if (!ent_start)
  {
    draw_bg (xtext, area->x, area->y, area->width, area->height);
    goto xit;
  }
  ent_end = find_char (xtext, area->x + area->width, area->y + area->height, NULL, NULL);

xit:
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

  if (!GTK_WIDGET_REALIZED (xtext))
    return;

  /* indent */

  gdk_drawable_get_size (GTK_WIDGET (xtext)->window, &width, &height);

  if (width < 34 || height < xtext->priv->fontsize /* || indent */)
    return;

  ent = NULL; /* FIXME */
  while (ent)
  {
  }

  /* fill any space below the last line with our background GC */
  draw_bg (xtext, 0, line, width + MARGIN, height - line);

  /* draw the separator line */
  draw_sep (xtext, -1);
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
  if (xtext->priv->show_separator /* && indent */)
  {
    light = xtext->priv->light_gc;
    dark  = xtext->priv->dark_gc;

    /* x = indent */
  }
}

static textentry*
nth (XText2 *xtext, int line, int *subline)
{
  /* FIXME */
  return NULL;
}

static textentry*
find_char (XText2 *xtext, int x, int y, int *offset, gboolean *out_of_bounds)
{
  textentry *ent;
  int line;
  int subline;

  /* uninitialized font */
  if (xtext->priv->fontsize == 0)
    return NULL;

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
    int stamp_size = get_stamp_str (time (NULL), &time_str);
    xtext->priv->stamp_width = text_width (xtext, time_str, stamp_size, NULL);
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
  g_print ("appending to buffer\n");
  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);
  f->wrapped_first = buffer->text_first;
  f->wrapped_last  = buffer->text_last;
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
  f->wrapped_first = buffer->text_first;
  f->wrapped_last  = buffer->text_last;
  f->append_handler = g_signal_connect (G_OBJECT (buffer), "append", G_CALLBACK (buffer_append), (gpointer) xtext);
  f->clear_handler  = g_signal_connect (G_OBJECT (buffer), "clear",  G_CALLBACK (buffer_clear),  (gpointer) xtext);
  f->remove_handler = g_signal_connect (G_OBJECT (buffer), "remove", G_CALLBACK (buffer_remove), (gpointer) xtext);
  g_object_weak_ref (G_OBJECT (buffer), (GWeakNotify) buffer_destruction_notify, xtext);
}

void
xtext2_show_buffer (XText2 *xtext, XTextBuffer *buffer)
{
  XTextFormat *f;

  f = g_hash_table_lookup (xtext->priv->buffer_info, buffer);
  g_print ("xtext2_show_buffer() - buffer_info is 0x%x\n", xtext->priv->buffer_info);
  if (f == NULL)
  {
    /* this isn't a buffer we've seen before */
    f = allocate_buffer (xtext, buffer);
  }
}

void
xtext2_refresh (XText2 *xtext)
{
  if (GTK_WIDGET_REALIZED (GTK_WIDGET (xtext)))
  {
#if defined(USE_XLIB) || defined(WIN32)
    if (xtext->priv->transparent)
    {
    }
#endif
    render_page (xtext);
  }
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

static int
count_lines_taken (XText2 *xtext, textentry *ent)
{
  /* count how many lines 'ent' will take (with wraps) */
  unsigned char *str;
  int indent, taken, len;
  int win_width;

  win_width = xtext->priv->window_width - MARGIN;

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
    indent = xtext->priv->indent;
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
}

static void
fix_indent (XText2 *xtext)
{
}
