#include "gldrawingarea.h"

static void gl_drawing_area_class_init     (GLDrawingAreaClass *klass);
static void gl_drawing_area_init           (GLDrawingArea      *darea);
static void gl_drawing_area_realize        (GtkWidget          *widget);
static void gl_drawing_area_finalize       (GObject            *object);
static void gl_drawing_area_send_configure (GLDrawingArea      *darea);

GType
gl_drawing_area_get_type (void)
{
  static GType gl_drawing_area_type = 0;

  if(!gl_drawing_area_type)
    {
      static const GTypeInfo gl_drawing_area_info =
      {
	sizeof (GLDrawingAreaClass),
	NULL,               /* base init */
	NULL,               /* base finalize */
	(GClassInitFunc)    gl_drawing_area_class_init,
	NULL,               /* class finalize */
	NULL,               /* class data */
	sizeof (GLDrawingArea),
	0,                  /* n preallocs */
	(GInstanceInitFunc) gl_drawing_area_init,
      };

      gl_drawing_area_type = g_type_register_static (GTK_TYPE_DRAWING_AREA, "GLDrawingArea", &gl_drawing_area_info, 0);
    }

  return gl_drawing_area_type;
}

static void
gl_drawing_area_class_init (GLDrawingAreaClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  widget_class->realize = gl_drawing_area_realize;
  object_class->finalize = gl_drawing_area_finalize;
}

static void
gl_drawing_area_init (GLDrawingArea *darea)
{
  darea->config = NULL;
  darea->context = NULL;
  darea->gldrawable = NULL;
}

static void
gl_drawing_area_realize (GtkWidget *widget)
{
  /* much of this is copied from the GtkDrawingArea source */
  GLDrawingArea *darea;
  GtkDrawingArea *area;
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (IS_GL_DRAWING_AREA (widget));

  area = GTK_DRAWING_AREA (widget);
  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, area);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  darea = GL_DRAWING_AREA (widget);

  darea->context = gtk_widget_get_gl_context(widget);
  darea->gldrawable = gtk_widget_get_gl_drawable(widget);

  gl_drawing_area_send_configure (darea);

  darea->context = gtk_widget_get_gl_context(widget);
  darea->gldrawable = gtk_widget_get_gl_drawable(widget);
}

GtkWidget*
gl_drawing_area_new (GdkGLConfig *config)
{
  GtkWidget *r = GTK_WIDGET(g_object_new(GL_DRAWING_AREA_TYPE, NULL));
  GLDrawingArea *darea = GL_DRAWING_AREA(r);
  gtk_widget_set_gl_capability(r, config, NULL, TRUE, GDK_GL_RGBA_TYPE);
  g_return_val_if_fail(gtk_widget_is_gl_capable(r), NULL);
  darea->config = g_object_ref(config);
  return r;
}

GtkWidget*
gl_drawing_area_new_shared (GLDrawingArea *base)
{
  GtkWidget *r = GTK_WIDGET(g_object_new(GL_DRAWING_AREA_TYPE, NULL));
  GLDrawingArea *darea = GL_DRAWING_AREA(r);
  gtk_widget_set_gl_capability(r, base->config, base->context, TRUE, GDK_GL_RGBA_TYPE);
  g_return_val_if_fail(gtk_widget_is_gl_capable(r), NULL);
  darea->config = g_object_ref(base->config);
  return r;
}

void gl_drawing_area_make_current (GLDrawingArea *glarea)
{
  gdk_gl_drawable_make_current(glarea->gldrawable, glarea->context);
}

void gl_drawing_area_swap_buffers (GLDrawingArea *glarea)
{
  gdk_gl_drawable_swap_buffers(glarea->gldrawable);
}

static void
gl_drawing_area_finalize (GObject *object)
{
  GLDrawingArea *darea = GL_DRAWING_AREA(object);
  g_object_unref(darea->config);
}

static void
gl_drawing_area_send_configure (GLDrawingArea *darea)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (darea);

  event->configure.window = g_object_ref (widget->window);
  event->configure.send_event = TRUE;
  event->configure.x = widget->allocation.x;
  event->configure.y = widget->allocation.y;
  event->configure.width = widget->allocation.width;
  event->configure.height = widget->allocation.height;

  gtk_widget_event (widget, event);
  gdk_event_free (event);
}
