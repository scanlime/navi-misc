#include "gldrawingarea.h"

static void gl_drawing_area_class_init (GLDrawingAreaClass *klass);
static void gl_drawing_area_init       (GLDrawingArea      *darea);
static void gl_drawing_area_realize    (GtkWidget          *widget);
static void gl_drawing_area_finalize   (GObject            *object);

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
  darea->drawable = NULL;
}

static void
gl_drawing_area_realize (GtkWidget *widget)
{
  GLDrawingArea *darea = GL_DRAWING_AREA(widget);

  darea->context = gtk_widget_get_gl_context(widget);
  darea->drawable = gtk_widget_get_gl_drawable(widget);
}

GtkWidget*
gl_drawing_area_new (GdkGLConfig *config)
{
  GtkWidget *r = GTK_WIDGET(g_object_new(GL_DRAWING_AREA_TYPE, NULL));
  GLDrawingArea *darea = GL_DRAWING_AREA(r);
  gtk_widget_set_gl_capability(r, config, NULL, TRUE, GDK_GL_RGBA_TYPE);
  darea->config = g_object_ref(config);
  return r;
}

GtkWidget*
gl_drawing_area_new_shared (GLDrawingArea *base)
{
  GtkWidget *r = GTK_WIDGET(g_object_new(GL_DRAWING_AREA_TYPE, NULL));
  GLDrawingArea *darea = GL_DRAWING_AREA(r);
  gtk_widget_set_gl_capability(r, base->config, base->context, TRUE, GDK_GL_RGBA_TYPE);
  darea->config = g_object_ref(base->config);
  return r;
}

void gl_drawing_area_make_current (GLDrawingArea *glarea)
{
}

static void
gl_drawing_area_finalize (GObject *object)
{
  GLDrawingArea *darea = GL_DRAWING_AREA(object);
  g_object_unref(darea->config);
}
