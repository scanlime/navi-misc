#include "xtextbuffer.h"

static void xtext_buffer_class_init (XTextBufferClass *klass);
static void xtext_buffer_init       (XTextBuffer *buffer);

GType
xtext_buffer_get_type (void)
{
  static GType xtext_buffer_type = 0;
  if (!xtext_buffer_type)
  {
    static const GTypeInfo xtext_buffer_info =
    {
      sizeof (XTextBufferClass),
      NULL,               /* base init */
      NULL,               /* base finalize */
      (GClassInitFunc)    xtext_buffer_class_init,
      NULL,               /* class finalize */
      NULL,               /* class data */
      sizeof (XTextBuffer),
      0,                  /* n preallocs */
      (GInstanceInitFunc) xtext_buffer_init,
    };

    xtext_buffer_type = g_type_register_static (GTK_TYPE_WIDGET, "XTextBuffer", &xtext_buffer_info, 0);
  }

  return xtext_buffer_type;
}

static void
xtext_buffer_class_init (XTextBufferClass *klass)
{
}

static void
xtext_buffer_init (XTextBuffer *buffer)
{
}

XTextBuffer*
xtext_buffer_new (void)
{
  return XTEXT_BUFFER (g_object_new (xtext_buffer_get_type (), NULL));
}
