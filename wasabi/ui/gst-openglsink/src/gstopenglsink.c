/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gstopenglsink.c:
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


#include <gstopenglsink.h>


GstElementDetails gst_openglsink_details = {
  "OpenGLSink",
  "Sink/Video",
  "LGPL",
  "Black hole for data",
  VERSION,
  "Erik Walthinsen <omega@cse.ogi.edu>",
  "(C) 1999",
};


/* OpenGLSink signals and args */
enum {
  /* FILL ME */
  SIGNAL_HANDOFF,
  LAST_SIGNAL
};

enum {
  ARG_0,
  ARG_NUM_SINKS,
  ARG_SILENT,
  ARG_DUMP,
  ARG_SYNC,
  ARG_LAST_MESSAGE,
};

GST_PAD_TEMPLATE_FACTORY (openglsink_sink_factory,
  "sink%d",
  GST_PAD_SINK,
  GST_PAD_REQUEST,
  NULL                  /* no caps */
);


static void	gst_openglsink_class_init		(GstOpenGLSinkClass *klass);
static void	gst_openglsink_init		(GstOpenGLSink *openglsink);

static void 	gst_openglsink_set_clock 		(GstElement *element, GstClock *clock);
static GstPad* 	gst_openglsink_request_new_pad 	(GstElement *element, GstPadTemplate *templ, const
                                                 gchar *unused);

static void	gst_openglsink_set_property	(GObject *object, guint prop_id,
						 const GValue *value, GParamSpec *pspec);
static void	gst_openglsink_get_property	(GObject *object, guint prop_id,
						 GValue *value, GParamSpec *pspec);

static void	gst_openglsink_chain		(GstPad *pad, GstBuffer *buf);

static GstElementClass *parent_class = NULL;
static guint gst_openglsink_signals[LAST_SIGNAL] = { 0 };

GType
gst_openglsink_get_type (void)
{
  static GType openglsink_type = 0;

  if (!openglsink_type) {
    static const GTypeInfo openglsink_info = {
      sizeof(GstOpenGLSinkClass),      NULL,
      NULL,
      (GClassInitFunc)gst_openglsink_class_init,
      NULL,
      NULL,
      sizeof(GstOpenGLSink),
      0,
      (GInstanceInitFunc)gst_openglsink_init,
    };
    openglsink_type = g_type_register_static (GST_TYPE_ELEMENT, "GstOpenGLSink", &openglsink_info, 0);
  }
  return openglsink_type;
}

static void
gst_openglsink_class_init (GstOpenGLSinkClass *klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass*)klass;
  gstelement_class = (GstElementClass*)klass;

  parent_class = g_type_class_ref (GST_TYPE_ELEMENT);

  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_NUM_SINKS,
    g_param_spec_int ("num_sinks", "Number of sinks", "The number of sinkpads",
                      1, G_MAXINT, 1, G_PARAM_READABLE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_LAST_MESSAGE,
    g_param_spec_string ("last_message", "Last Message", "The message describing current status",
                         NULL, G_PARAM_READABLE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_SYNC,
    g_param_spec_boolean ("sync", "Sync", "Sync on the clock",
                          FALSE, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_SILENT,
    g_param_spec_boolean ("silent", "Silent", "Don't produce last_message events",
                          FALSE, G_PARAM_READWRITE));
  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_DUMP,
    g_param_spec_boolean ("dump", "Dump", "Dump received bytes to stdout",
                          FALSE, G_PARAM_READWRITE));

  gst_openglsink_signals[SIGNAL_HANDOFF] =
    g_signal_new ("handoff", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
                    G_STRUCT_OFFSET (GstOpenGLSinkClass, handoff), NULL, NULL,
                    g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1,
                    G_TYPE_POINTER);

  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_openglsink_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_openglsink_get_property);

  gstelement_class->request_new_pad = GST_DEBUG_FUNCPTR (gst_openglsink_request_new_pad);
  gstelement_class->set_clock       = GST_DEBUG_FUNCPTR (gst_openglsink_set_clock);
}

static void
gst_openglsink_init (GstOpenGLSink *openglsink)
{
  GstPad *pad;

  pad = gst_pad_new ("sink", GST_PAD_SINK);
  gst_element_add_pad (GST_ELEMENT (openglsink), pad);
  gst_pad_set_chain_function (pad, GST_DEBUG_FUNCPTR (gst_openglsink_chain));

  openglsink->silent = FALSE;
  openglsink->dump = FALSE;
  openglsink->sync = FALSE;
  openglsink->last_message = NULL;

  GST_FLAG_SET (openglsink, GST_ELEMENT_EVENT_AWARE);
}

static void
gst_openglsink_set_clock (GstElement *element, GstClock *clock)
{
  GstOpenGLSink *sink;

  sink = GST_OPENGLSINK (element);

  sink->clock = clock;
}

static GstPad*
gst_openglsink_request_new_pad (GstElement *element, GstPadTemplate *templ, const gchar *unused)
{
  gchar *name;
  GstPad *sinkpad;
  GstOpenGLSink *openglsink;

  g_return_val_if_fail (GST_IS_OPENGLSINK (element), NULL);

  if (templ->direction != GST_PAD_SINK) {
    g_warning ("gstopenglsink: request new pad that is not a SINK pad\n");
    return NULL;
  }

  openglsink = GST_OPENGLSINK (element);

  name = g_strdup_printf ("sink%d", GST_ELEMENT (openglsink)->numsinkpads);

  sinkpad = gst_pad_new_from_template (templ, name);
  g_free (name);

  gst_element_add_pad (GST_ELEMENT (openglsink), sinkpad);

  return sinkpad;
}

static void
gst_openglsink_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  GstOpenGLSink *sink;

  /* it's not null if we got it, but it might not be ours */
  sink = GST_OPENGLSINK (object);

  switch (prop_id) {
    case ARG_SILENT:
      sink->silent = g_value_get_boolean (value);
      break;
    case ARG_DUMP:
      sink->dump = g_value_get_boolean (value);
      break;
    case ARG_SYNC:
      sink->sync = g_value_get_boolean (value);
      break;
    default:
      break;
  }
}

static void
gst_openglsink_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  GstOpenGLSink *sink;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (GST_IS_OPENGLSINK (object));

  sink = GST_OPENGLSINK (object);

  switch (prop_id) {
    case ARG_NUM_SINKS:
      g_value_set_int (value, GST_ELEMENT (sink)->numsinkpads);
      break;
    case ARG_SILENT:
      g_value_set_boolean (value, sink->silent);
      break;
    case ARG_DUMP:
      g_value_set_boolean (value, sink->dump);
      break;
    case ARG_SYNC:
      g_value_set_boolean (value, sink->sync);
      break;
    case ARG_LAST_MESSAGE:
      g_value_set_string (value, sink->last_message);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_openglsink_chain (GstPad *pad, GstBuffer *buf)
{
  GstOpenGLSink *openglsink;

  g_return_if_fail (pad != NULL);
  g_return_if_fail (GST_IS_PAD (pad));
  g_return_if_fail (buf != NULL);

  openglsink = GST_OPENGLSINK (gst_pad_get_parent (pad));

  if (GST_IS_EVENT (buf)) {
    GstEvent *event = GST_EVENT (buf);

    if (!openglsink->silent) {
      g_free (openglsink->last_message);

      openglsink->last_message = g_strdup_printf ("chain   ******* (%s:%s)E (type: %d) %p",
		GST_DEBUG_PAD_NAME (pad), GST_EVENT_TYPE (event), event);

      g_object_notify (G_OBJECT (openglsink), "last_message");
    }

    switch (GST_EVENT_TYPE (event)) {
      case GST_EVENT_DISCONTINUOUS:
        if (openglsink->sync && openglsink->clock) {
          gint64 value = GST_EVENT_DISCONT_OFFSET (event, 0).value;
          gst_clock_handle_discont (openglsink->clock, value);
	}
      default:
	gst_pad_event_default (pad, event);
        break;
    }
    return;
  }

  if (openglsink->sync && openglsink->clock) {
    GstClockID id = gst_clock_new_single_shot_id (openglsink->clock, GST_BUFFER_TIMESTAMP (buf));

    gst_element_clock_wait (GST_ELEMENT (openglsink), id, NULL);
    gst_clock_id_free (id);
  }

  if (!openglsink->silent) {
    g_free (openglsink->last_message);

    openglsink->last_message = g_strdup_printf ("chain   ******* (%s:%s)< (%d bytes, %"
                                              G_GINT64_FORMAT ", %d) %p",
		GST_DEBUG_PAD_NAME (pad), GST_BUFFER_SIZE (buf), GST_BUFFER_TIMESTAMP (buf),
		GST_BUFFER_FLAGS (buf), buf);

    g_object_notify (G_OBJECT (openglsink), "last_message");
  }

  g_signal_emit (G_OBJECT (openglsink), gst_openglsink_signals[SIGNAL_HANDOFF], 0, buf, pad);

  if (openglsink->dump) {
    gst_util_dump_mem (GST_BUFFER_DATA (buf), GST_BUFFER_SIZE (buf));
  }

  gst_buffer_unref (buf);
}

gboolean
gst_openglsink_factory_init (GstElementFactory *factory)
{
  gst_element_factory_add_pad_template (factory, GST_PAD_TEMPLATE_GET (openglsink_sink_factory));

  return TRUE;
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

  factory = gst_element_factory_new ("openglsink", GST_TYPE_OPENGLSINK, &gst_openglsink_details);
  g_return_val_if_fail (factory != NULL, FALSE);

  gst_element_factory_add_pad_template (factory,
  openglsink_sink_factory ());

  gst_plugin_add_feature (plugin, GST_PLUGIN_FEATURE (factory));

  /* plugin initialisation succeeded */
  return TRUE;
}

/* this is the structure that gst-register looks for
* so keep the name plugin_desc, or you cannot get your plug-in registered */
GstPluginDesc plugin_desc = {
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  "openglsink",
  plugin_init
};
