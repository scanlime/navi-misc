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

static void       paint                  (GtkWidget *widget, GdkRectangle *area);
static void       draw_bg                (XText2 *xtext, int x, int y, int width, int height);
static textentry* nth                    (XText2 *xtext, int line, int *subline);
static textentry* find_char              (XText2 *xtext, int x, int y, int *offset, gboolean *out_of_bounds);
static int        find_x                 (XText2 *xtext, int x, textentry *ent, int subline, int line, gboolean *out_of_bounds);
static void       set_fg                 (XText2 *xtext, GdkGC *gc, int index);
static void       set_bg                 (XText2 *xtext, GdkGC *gc, int index);

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

struct _XText2Private
{
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

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);
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
    gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
  }
}

static gboolean
xtext2_expose (GtkWidget *widget, GdkEventExpose *event)
{
  paint (widget, &event->area);
  return FALSE;
}

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
draw_bg (XText2 *xtext, int x, int y, int width, int height)
{
  gdk_draw_rectangle (xtext->priv->draw_buffer, xtext->priv->bgc, 1, x, y, width, height);
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

  for (i = 0; i < 19; i++)
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
    set_fg (xtext, xtext->priv->fgc, 19);
  }
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
