#include "xtextbuffer.h"
#include <time.h>

static void xtext_buffer_class_init (XTextBufferClass *klass);
static void xtext_buffer_init       (XTextBuffer *buffer);
static void xtext_buffer_finalize   (GObject *object);

struct _textentry
{
  textentry *next;
  textentry *prev;
  unsigned char *str;
  time_t stamp;
  gint16 str_width;
  gint16 str_len;
  gint16 mark_start;
  gint16 mark_end;
  gint16 indent;
  gint16 left_len;
  gint16 lines_taken;
  gboolean multibyte: TRUE;
};

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
  GObjectClass *object_class;

  object_class = (GObjectClass*) klass;

  object_class->finalize = xtext_buffer_finalize;
}

static void
xtext_buffer_init (XTextBuffer *buffer)
{
  buffer->text_first = NULL;
  buffer->text_last = NULL;
}

static void
xtext_buffer_finalize (GObject *object)
{
  XTextBuffer *buffer = XTEXT_BUFFER (object);
  textentry *ent, *next;

  ent = buffer->text_first;
  while (ent)
  {
    next = ent->next;
    g_free (ent);
    ent = next;
  }
}

XTextBuffer*
xtext_buffer_new (void)
{
  return XTEXT_BUFFER (g_object_new (xtext_buffer_get_type (), NULL));
}

void
xtext_buffer_append (XTextBuffer *buffer, unsigned char *text, int len)
{
}

void
xtext_buffer_append_indent (XTextBuffer *buffer, unsigned char *left, int llen, unsigned char *right, int rlen)
{
}

void
xtext_buffer_clear (XTextBuffer *buffer)
{
}

gboolean
xtext_buffer_is_empty (XTextBuffer *buffer)
{
}

void
xtext_buffer_foreach (XTextBuffer *buffer, XTextBufferForeachFunc func, gpointer user_data)
{
}
