#ifndef __XTEXT2_H__
#define __XTEXT2_H__

#include <gtk/gtk.h>
#include "xtextbuffer.h"

G_BEGIN_DECLS

#define XTEXT2_TYPE            (xtext2_get_type ())
#define XTEXT2(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XTEXT2_TYPE, XText2))
#define XTEXT2_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XTEXT2_TYPE, XText2Class))
#define IS_XTEXT2(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XTEXT2_TYPE))
#define IS_XTEXT2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XTEXT2_TYPE))

typedef struct _XText2        XText2;
typedef struct _XText2Class   XText2Class;
typedef struct _XText2Private XText2Private;

struct _XText2
{
  GtkWidget parent;

  XText2Private *priv;
  GtkAdjustment *adj;
};

struct _XText2Class
{
  GtkWidgetClass parent_class;

  /* signals */
  void (*word_click) (XText2 * xtext, char *word, GdkEventButton *event);
};

GType      xtext2_get_type    (void) G_GNUC_CONST;
GtkWidget* xtext2_new         (void);
void       xtext2_set_palette (XText2 *xtext, GdkColor palette[]);
gboolean   xtext2_set_font    (XText2 *xtext, char *name);
void       xtext2_show_buffer (XText2 *xtext, XTextBuffer *buffer);
void       xtext2_refresh     (XText2 *xtext);

#define ATTR_BOLD      '\002'
#define ATTR_COLOR     '\003'
#define ATTR_BEEP      '\007'
#define ATTR_RESET     '\017'
#define ATTR_REVERSE   '\026'
#define ATTR_UNDERLINE '\037'

G_END_DECLS

#endif
