#ifndef __XTEXT_BUFFER_H__
#define __XTEXT_BUFFER_H__

#include <gtk/gtk.h>

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
