#ifndef __GL_DRAWING_AREA_H__
#define __GL_DRAWING_AREA_H__

#include <gtk/gtkdrawingarea.h>
#include <gtk/gtkgl.h>

#define GL_DRAWING_AREA_TYPE		(gl_drawing_area_get_type ())
#define GL_DRAWING_AREA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GL_DRAWING_AREA_TYPE, GLDrawingArea))
#define GL_DRAWING_AREA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GL_DRAWING_AREA_TYPE, GLDrawingAreaClass))
#define IS_GL_DRAWING_AREA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GL_DRAWING_AREA_TYPE))
#define IS_GL_DRAWING_AREA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GL_DRAWING_AREA_TYPE))
#define GL_DRAWING_AREA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GL_DRAWING_AREA_TYPE, GLDrawingAreaClass))

typedef struct _GLDrawingArea		GLDrawingArea;
typedef struct _GLDrawingAreaClass	GLDrawingAreaClass;

struct _GLDrawingArea
{
  GtkDrawingArea widget;

  GdkGLConfig *config;
  GdkGLContext *context;
  GdkGLDrawable *gldrawable;
};

struct _GLDrawingAreaClass
{
  GtkDrawingAreaClass parent_class;
};

GType      gl_drawing_area_get_type     (void) G_GNUC_CONST;
GtkWidget* gl_drawing_area_new          (GdkGLConfig *config);
GtkWidget* gl_drawing_area_new_shared   (GLDrawingArea *base);
void       gl_drawing_area_make_current (GLDrawingArea *glarea);
void       gl_drawing_area_swap_buffers (GLDrawingArea *glarea);

#endif /* __GL_DRAWING_AREA_H__ */
