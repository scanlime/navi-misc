#ifndef __XTEXT_BUFFER_H__
#define __XTEXT_BUFFER_H__

#include <gtk/gtk.h>
#include <time.h>

G_BEGIN_DECLS

#define XTEXT_BUFFER_TYPE            (xtext_buffer_get_type ())
#define XTEXT_BUFFER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XTEXT_BUFFER_TYPE, XTextBuffer))
#define XTEXT_BUFFER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XTEXT_BUFFER_TYPE, XTextBufferClass))
#define IS_XTEXT_BUFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XTEXT_BUFFER_TYPE))
#define IS_XTEXT_BUFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), XTEXT_BUFFER_TYPE))

typedef struct _XTextBuffer      XTextBuffer;
typedef struct _XTextBufferClass XTextBufferClass;
typedef struct _textentry        textentry;

typedef void (*XTextBufferForeachFunc) (XTextBuffer *buffer, unsigned char *text, gpointer user_data);

struct _XTextBuffer
{
  GObject parent;

  textentry *text_first;
  textentry *text_last;
  guint num_lines;
  guint max_lines;
};

struct _XTextBufferClass
{
  GObjectClass parent_class;

  /* signals */
  void (*append) (XTextBuffer *buffer, textentry *ent);
  void (*clear)  (XTextBuffer *buffer);
  void (*remove) (XTextBuffer *buffer);
};

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
#define RECORD_WRAPS 4
  guint16 wrap_offset[RECORD_WRAPS];
  gboolean multibyte: TRUE;
};

GType        xtext_buffer_get_type (void) G_GNUC_CONST;
XTextBuffer* xtext_buffer_new      (void);

void         xtext_buffer_append        (XTextBuffer *buffer, unsigned char *text, int len);
void         xtext_buffer_append_indent (XTextBuffer *buffer, unsigned char *left, int llen,
                                                              unsigned char *right, int rlen);
void         xtext_buffer_clear         (XTextBuffer *buffer);
gboolean     xtext_buffer_is_empty      (XTextBuffer *buffer);
void         xtext_buffer_foreach       (XTextBuffer *buffer, XTextBufferForeachFunc func, gpointer user_data);


G_END_DECLS

#endif
