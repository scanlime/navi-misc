/* 
 * gstopenglsink.h: video sink that draws to an opengl texture
 */

#ifndef __GST_PLUGIN_OPENGLSINK_H__
#define __GST_PLUGIN_OPENGLSINK_H__

#include <gst/gst.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* #define's don't like whitespacey bits */
#define GST_TYPE_PLUGIN_OPENGLSINK \
  (gst_gst_plugin_openglsink_get_type())
#define GST_PLUGIN_OPENGLSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PLUGIN_OPENGLSINK,GstPluginTemplate))
#define GST_PLUGIN_OPENGLSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PLUGIN_OPENGLSINK,GstPluginTemplate))
#define GST_IS_PLUGIN_OPENGLSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PLUGIN_OPENGLSINK))
#define GST_IS_PLUGIN_OPENGLSINK_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PLUGIN_OPENGLSINK))

typedef struct _GstPluginOpenGLSink      GstPluginOpenGLSink;
typedef struct _GstPluginOpenGLSinkClass GstPluginOpenGLSinkClass;

struct _GstPluginOpenGLSink
{
  GstElement element;

  GstPad *sinkpad;

  gboolean silent;
};

struct _GstPluginOpenGLSinkClass
{
  GstElementClass parent_class;
};

GType gst_gst_plugin_openglsink_get_type (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GST_PLUGIN_OPENGLSINK_H__ */
