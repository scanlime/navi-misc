/* 
 * gstplugin.h: sample header file for plug-in
 */

#ifndef __GST_PLUGIN_TEMPLATE_H__
#define __GST_PLUGIN_TEMPLATE_H__

#include <gst/gst.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* #define's don't like whitespacey bits */
#define GST_TYPE_PLUGIN_TEMPLATE \
  (gst_gst_plugin_template_get_type())
#define GST_PLUGIN_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PLUGIN_TEMPLATE,GstPluginTemplate))
#define GST_PLUGIN_TEMPLATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PLUGIN_TEMPLATE,GstPluginTemplate))
#define GST_IS_PLUGIN_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PLUGIN_TEMPLATE))
#define GST_IS_PLUGIN_TEMPLATE_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PLUGIN_TEMPLATE))

typedef struct _GstPluginTemplate      GstPluginTemplate;
typedef struct _GstPluginTemplateClass GstPluginTemplateClass;

struct _GstPluginTemplate
{
  GstElement element;

  GstPad *sinkpad, *srcpad;

  gboolean silent;
};

struct _GstPluginTemplateClass 
{
  GstElementClass parent_class;
};

GType gst_gst_plugin_template_get_type (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GST_PLUGIN_TEMPLATE_H__ */
