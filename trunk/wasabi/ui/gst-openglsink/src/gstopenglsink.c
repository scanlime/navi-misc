/*
 *
 * GStreamer
 * Copyright (C) 1999-2001 Erik Walthinsen <omega@cse.ogi.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <gst/gst.h>

/* include this header if you want to use dynamic parameters
#include <gst/control/control.h>
*/

#include "gstopenglsink.h"

static GstElementDetails plugin_details = {
  "OpenGLSink",
  "Sink/Video/OpenGLSink",
  "LGPL",
  "OpenGL Texture Sink",
  VERSION,
  "David Trowbridge <trowbrds@cs.colorado.edu>\n"
  "Thomas Vander Stichele <thomas@apestaart.org>",
  "(C) 2002, 2003"
};

/* Filter signals and args */
enum {
  /* FILL ME */
  LAST_SIGNAL
};

enum {
  ARG_0,
  ARG_SILENT
};

GST_PAD_TEMPLATE_FACTORY (gst_openglsink_sink_factory,
  "sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  NULL		/* no caps */
);

static void	gst_openglsink_class_init(		GstOpenGLSink *klass);
static void	gst_openglsink_init(			GstOpenGLSink *filter);

static void	gst_openglsink_set_property(		GObject *object, guint prop_id,
							const GValue *value,
							GParamSpec *pspec);
static void	gst_openglsink_get_property(		GObject *object, guint prop_id,
							GValue *value,
							GParamSpec *pspec);
static void	gst_openglsink_update_plugin(		const GValue *value,
							gpointer data);
static void	gst_openglsink_update_mute(		const GValue *value,
							gpointer data);

static void	gst_openglsink_chain(			GstPad *pad, GstBuffer *buf);

static GstElementClass *parent_class = NULL;

/* this function handles the link with other plug-ins */
static GstPadLinkReturn
gst_openglsink_link (GstPad *pad, GstCaps *caps)
{
  GstOpenGLSink *filter;
  GstPad *otherpad;

  filter = GST_OPENGLSINK (gst_pad_get_parent (pad));
  g_return_val_if_fail (filter != NULL, GST_PAD_LINK_REFUSED);
  g_return_val_if_fail (GST_IS_OPENGLSINK (filter),
                        GST_PAD_LINK_REFUSED);
  otherpad = (pad == filter->srcpad ? filter->sinkpad : filter->srcpad);

  if (GST_CAPS_IS_FIXED (caps))
  {
    /* caps are not fixed, so try to link on the other side and see if
     * that works */

    if (!gst_pad_try_set_caps (otherpad, caps))
      return GST_PAD_LINK_REFUSED;

    /* caps on other side were accepted, so we're ok */
    return GST_PAD_LINK_OK;
  }
  /* not enough information yet, delay negotation */
  return GST_PAD_LINK_DELAYED;
}

GType
gst_gst_openglsink_get_type (void)
{
  static GType plugin_type = 0;

  if (!plugin_type)
  {
    static const GTypeInfo plugin_info =
    {
      sizeof (GstOpenGLSink),
      NULL,
      NULL,
      (GClassInitFunc) gst_openglsink_class_init,
      NULL,
      NULL,
      sizeof (GstOpenGLSink),
      0,
      (GInstanceInitFunc) gst_openglsink_init,
    };
    plugin_type = g_type_register_static (GST_TYPE_ELEMENT,
	                                  "GstOpenGLSink",
	                                  &plugin_info, 0);
  }
  return plugin_type;
}

/* initialize the plugin's class */
static void
gst_openglsink_class_init (GstOpenGLSink *klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass*) klass;
  gstelement_class = (GstElementClass*) klass;

  parent_class = g_type_class_ref (GST_TYPE_ELEMENT);

  g_object_class_install_property (gobject_class, ARG_SILENT,
    g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
                          FALSE, G_PARAM_READWRITE));

  gobject_class->set_property = gst_openglsink_set_property;
  gobject_class->get_property = gst_openglsink_get_property;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set functions
 * initialize structure
 */
static void
gst_openglsink_init (GstOpenGLSink *filter)
{
  filter->sinkpad = gst_pad_new_from_template (gst_openglsink_sink_factory (),
                                               "sink");
  gst_pad_set_link_function (filter->sinkpad, gst_openglsink_link);
  filter->srcpad = gst_pad_new_from_template (gst_openglsink_src_factory (),
                                              "src");
  gst_pad_set_link_function (filter->srcpad, gst_openglsink_link);

  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
  gst_pad_set_chain_function (filter->sinkpad, gst_openglsink_chain);
  filter->silent = FALSE;

}

/* chain function
 * this function does the actual processing
 */

static void
gst_openglsink_chain (GstPad *pad, GstBuffer *buf)
{
  GstOpenGLSink *filter;
  GstBuffer *out_buf;
  gfloat *data;
  gint i, num_samples;

  g_return_if_fail (GST_IS_PAD (pad));
  g_return_if_fail (buf != NULL);

  filter = GST_OPENGLSINK (GST_OBJECT_PARENT (pad));
  g_return_if_fail (GST_IS_OPENGLSINK (filter));

  if (filter->silent == FALSE)
    g_print ("I'm plugged, therefore I'm in.\n");

  /* just push out the incoming buffer without touching it */
  gst_pad_push (filter->srcpad, buf);
}

static void
gst_openglsink_set_property (GObject *object, guint prop_id,
                                  const GValue *value, GParamSpec *pspec)
{
  GstOpenGLSink *filter;

  g_return_if_fail (GST_IS_OPENGLSINK (object));
  filter = GST_OPENGLSINK (object);

  switch (prop_id)
  {
  case ARG_SILENT:
    filter->silent = g_value_get_boolean (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gst_openglsink_get_property (GObject *object, guint prop_id,
                                  GValue *value, GParamSpec *pspec)
{
  GstOpenGLSink *filter;

  g_return_if_fail (GST_IS_OPENGLSINK (object));
  filter = GST_OPENGLSINK (object);

  switch (prop_id) {
  case ARG_SILENT:
    g_value_set_boolean (value, filter->silent);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and pad templates
 * register the features
 */
static gboolean
plugin_init (GModule *module, GstPlugin *plugin)
{
  GstElementFactory *factory;

  factory = gst_element_factory_new ("plugin", GST_TYPE_OPENGLSINK,
                                     &plugin_details);
  g_return_val_if_fail (factory != NULL, FALSE);

  gst_element_factory_add_pad_template (factory,
                                        gst_openglsink_src_factory ());
  gst_element_factory_add_pad_template (factory,
                                        gst_openglsink_sink_factory ());

  gst_plugin_add_feature (plugin, GST_PLUGIN_FEATURE (factory));

  /* plugin initialisation succeeded */
  return TRUE;
}

/* this is the structure that gst-register looks for
 * so keep the name plugin_desc, or you cannot get your plug-in registered */
GstPluginDesc plugin_desc = {
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  "plugin",
  plugin_init
};
