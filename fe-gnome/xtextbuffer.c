#include "xtextbuffer.h"
#include <time.h>
#include <string.h>

#define SCRATCH_BUFFER_SIZE 4096
#define charlen(str) g_utf8_skip[*(guchar *)(str)]

static void xtext_buffer_class_init (XTextBufferClass *klass);
static void xtext_buffer_init       (XTextBuffer *buffer);
static void xtext_buffer_finalize   (GObject *object);

static void append_entry            (XTextBuffer *buffer, textentry *ent);
static int  lines_taken             (XTextBuffer *buffer, textentry *ent);
static void remove_top              (XTextBuffer *buffer);

enum
{
  APPEND,
  CLEAR,
  REMOVE,
  LAST_SIGNAL,
};

static gint buffer_signals[LAST_SIGNAL];

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

  buffer_signals[APPEND] = g_signal_new ("append",
    G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
    G_STRUCT_OFFSET (XTextBufferClass, append), NULL, NULL,
    gtk_marshal_VOID__POINTER, G_TYPE_NONE,
    1, G_TYPE_POINTER);
  buffer_signals[CLEAR] = g_signal_new ("clear",
    G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
    G_STRUCT_OFFSET (XTextBufferClass, clear), NULL, NULL,
    gtk_marshal_VOID__VOID, G_TYPE_NONE,
    0, NULL);
  buffer_signals[REMOVE] = g_signal_new ("remove",
    G_TYPE_FROM_CLASS (object_class), G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
    G_STRUCT_OFFSET (XTextBufferClass, remove), NULL, NULL,
    gtk_marshal_VOID__VOID, G_TYPE_NONE,
    0, NULL);
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
  textentry *ent;

  if (len == -1)
    len = strlen (text);
  if (text[len - 1] == '\n')
    len--;
  if (len >= SCRATCH_BUFFER_SIZE)
    len = SCRATCH_BUFFER_SIZE - 1;

  ent = g_malloc0 (len + 1 + sizeof (textentry));
  ent->str = (unsigned char *) ent + sizeof (textentry);
  ent->str_len = len;
  if (len)
    memcpy (ent->str, text, len);
  ent->indent = 0;
  ent->left_len = -1;

  append_entry (buffer, ent);
}

void
xtext_buffer_append_indent (XTextBuffer *buffer, unsigned char *left, int llen, unsigned char *right, int rlen)
{
  textentry *ent;
  unsigned char *str;
  int space;
  int tempindent;
  int left_width;

  if (llen == -1)
    llen = strlen (left);
  if (rlen == -1)
    rlen = strlen (right);

  if (rlen >= SCRATCH_BUFFER_SIZE)
    rlen = SCRATCH_BUFFER_SIZE - 1;

  if (right[rlen - 1] == '\n')
    rlen--;

  ent = g_malloc0 (llen + rlen + 2 + sizeof (textentry));
  str = (unsigned char*) ent + sizeof (textentry);

  memcpy (str, left, llen);
  str[llen] = ' ';
  memcpy (str + llen + 1, right, rlen);

}

void
xtext_buffer_clear (XTextBuffer *buffer)
{
  textentry *next;

  while (buffer->text_first)
  {
    next = buffer->text_first->next;
    g_free (buffer->text_first);
    buffer->text_first = next;
  }
  buffer->text_last = NULL;

  /* notify listeners */
}

gboolean
xtext_buffer_is_empty (XTextBuffer *buffer)
{
  return (buffer->text_first == NULL);
}

void
xtext_buffer_foreach (XTextBuffer *buffer, XTextBufferForeachFunc func, gpointer user_data)
{
  textentry *ent = buffer->text_first;

  while (ent)
  {
    func (buffer, ent->str, user_data);
    ent = ent->next;
  }
}

static void
append_entry (XTextBuffer *buffer, textentry *ent)
{
  gboolean multibyte;
  int i;

  /* we don't like tabs */
  i = 0;
  while (i < ent->str_len)
  {
    if (ent->str[i] == '\t')
      ent->str[i] = ' ';
      i += charlen (ent->str + i);
  }

  ent->stamp = 0;
//  ent->str_width = text_width ();
  ent->multibyte = multibyte;
  ent->mark_start = -1;
  ent->mark_end = -1;
  ent->next = NULL;

  // margin?

  /* append to our linked list */
  if (buffer->text_last)
    buffer->text_last->next = ent;
  else
    buffer->text_first = ent;
  ent->prev = buffer->text_last;

  /*
  ent->lines_taken = lines_taken (buffer, ent);
  buffer->num_lines += ent->lines_taken;
  */

  if (buffer->max_lines > 2 && buffer->max_lines < buffer->num_lines)
  {
    remove_top (buffer);
  }

  /* notify viewers */
}

static int
lines_taken (XTextBuffer *buffer, textentry *ent)
{
  /* HELP! */
}

static void
remove_top (XTextBuffer *buffer)
{
}
