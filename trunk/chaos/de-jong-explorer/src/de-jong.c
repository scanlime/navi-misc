/*
 * de-jong.c - The DeJong object stores all the parameters necessary for
 *             rendering the de Jong map, runs iterations to generate a
 *             point histogram, and can update a GdkPixbuf with an image
 *             generated from the histogram.
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "de-jong.h"

static GObjectClass *parent_class = NULL;

static void de_jong_class_init(DeJongClass *klass);
static void de_jong_init(DeJong *self);
static void de_jong_dispose(GObject *gobject);
static void de_jong_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void de_jong_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void de_jong_resize_from_string(DeJong *self, const gchar *s);

static void update_double_if_necessary(gdouble new_value, gboolean *dirty_flag, gdouble *param, gdouble epsilon);
static void update_uint_if_necessary(guint new_value, gboolean *dirty_flag, guint *param);
static void update_boolean_if_necessary(gboolean new_value, gboolean *dirty_flag, gboolean *param);
static void update_color_if_necessary(const gchar* new_value, gboolean *dirty_flag, GdkColor *param);
static gchar* describe_color(GdkColor *c);

enum {
  PROP_0,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_OVERSAMPLE,
  PROP_SIZE,
  PROP_A,
  PROP_B,
  PROP_C,
  PROP_D,
  PROP_ZOOM,
  PROP_XOFFSET,
  PROP_YOFFSET,
  PROP_ROTATION,
  PROP_BLUR_RADIUS,
  PROP_BLUR_RATIO,
  PROP_TILEABLE,
  PROP_EXPOSURE,
  PROP_GAMMA,
  PROP_FGCOLOR,
  PROP_BGCOLOR,
  PROP_FGALPHA,
  PROP_BGALPHA,
  PROP_CLAMPED,
  PROP_TARGET_DENSITY,
};


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType de_jong_get_type(void) {
  static GType dj_type = 0;

  if (!dj_type) {
    static const GTypeInfo dj_info = {
      sizeof(DeJongClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) de_jong_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(DeJong),
      0,
      (GInstanceInitFunc) de_jong_init,
    };

    dj_type = g_type_register_static(G_TYPE_OBJECT, "DeJong", &dj_info, 0);
  }

  return dj_type;
}

static void de_jong_class_init(DeJongClass *klass) {
  GObjectClass *object_class;

  parent_class = g_type_class_ref(G_TYPE_OBJECT);
  object_class = (GObjectClass*) klass;

  object_class->set_property = de_jong_set_property;
  object_class->get_property = de_jong_get_property;
  object_class->dispose      = de_jong_dispose;

  g_object_class_install_property(object_class,
				  PROP_WIDTH,
				  g_param_spec_uint("width",
						    "Width",
						    "Width of the rendered image, in pixels",
						    0, G_MAXUINT, 600,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_HEIGHT,
				  g_param_spec_uint("height",
						    "Height",
						    "Height of the rendered image, in pixels",
						    0, G_MAXUINT, 600,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_OVERSAMPLE,
				  g_param_spec_uint("oversample",
						    "Oversampling",
						    "Oversampling factor, 1 for no oversampling to 4 for heavy oversampling",
						    1, 4, 1,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_SIZE,
				  g_param_spec_string("size",
						      "Size",
						      "Image size as a WIDTH or WIDTHxHEIGHT string",
						      NULL,
						      G_PARAM_READWRITE));

  g_object_class_install_property(object_class,
				  PROP_A,
				  g_param_spec_double("a",
						      "A",
						      "de Jong parameter A",
						      -100, 100, 1.41914,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_B,
				  g_param_spec_double("b",
						      "B",
						      "de Jong parameter B",
						      -100, 100, -2.28413,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_C,
				  g_param_spec_double("c",
						      "C",
						      "de Jong parameter C",
						      -100, 100, 2.42754,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_D,
				  g_param_spec_double("d",
						      "D",
						      "de Jong parameter D",
						      -100, 100, -2.17719,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_ZOOM,
				  g_param_spec_double("zoom",
						      "Zoom",
						      "Zoom factor",
						      0, 1000, 1,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_XOFFSET,
				  g_param_spec_double("xoffset",
						      "X Offset",
						      "Horizontal image offset",
						      -10, 10, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_YOFFSET,
				  g_param_spec_double("yoffset",
						      "Y Offset",
						      "Vertical image offset",
						      -10, 10, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_ROTATION,
				  g_param_spec_double("rotation",
						      "Rotation",
						      "Rotation angle, in radians",
						      -10, 10, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_BLUR_RADIUS,
				  g_param_spec_double("blur_radius",
						      "Blur Radius",
						      "Gaussian blur radius",
						      0, 10, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_BLUR_RATIO,
				  g_param_spec_double("blur_ratio",
						      "Blur Ratio",
						      "Amount of blurred vs non-blurred rendering",
						      0, 1, 1,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_TILEABLE,
				  g_param_spec_boolean("tileable",
						       "Tileable",
						       "When set, the image is wrapped rather than clipped at the edges",
						       FALSE,
						       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_EXPOSURE,
				  g_param_spec_double("exposure",
						      "Exposure",
						      "The relative strength, darkness, or brightness of the image",
						      0, 10, 0.05,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_FGCOLOR,
				  g_param_spec_string("fgcolor",
						      "Foreground Color",
						      "The foreground color, as a color name or #RRGGBB hex triple",
						      "black",
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_BGCOLOR,
				  g_param_spec_string("bgcolor",
						      "Background Color",
						      "The background color, as a color name or #RRGGBB hex triple",
						      "white",
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_FGALPHA,
				  g_param_spec_uint("fgalpha",
						    "Foreground Alpha",
						    "The foreground color's opacity",
						    0, 65535, 65535,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_BGALPHA,
				  g_param_spec_uint("bgalpha",
						    "Background Alpha",
						    "The background color's opacity",
						    0, 65535, 65535,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_GAMMA,
				  g_param_spec_double("gamma",
						      "Gamma",
						      "A gamma correction applied while rendering the image",
						      0, 10, 1,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_CLAMPED,
				  g_param_spec_boolean("clamped",
						       "Clamped",
						       "When set, luminances are clamped to [0,1] before linear interpolation",
						       FALSE,
						       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(object_class,
				  PROP_TARGET_DENSITY,
				  g_param_spec_uint("target_density",
						    "Target Density",
						    "The peak image density to stop calculation at",
						    0, 10000000, 10000,
						    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));


}

static void de_jong_init(DeJong *self) {
}

static void de_jong_dispose(GObject *gobject) {
  DeJong *self = DE_JONG(gobject);

  if (self->histogram) {
    g_free(self->histogram);
    self->histogram = NULL;
  }
  if (self->image) {
    gdk_pixbuf_unref(self->image);
    self->image = NULL;
  }
  if (self->color_table) {
    g_free(self->color_table);
    self->color_table = NULL;
  }
}

DeJong* de_jong_new() {
  return DE_JONG(g_object_new(de_jong_get_type(), NULL));
}


/************************************************************************************/
/*********************************************************************** Properties */
/************************************************************************************/

static gchar* describe_color(GdkColor *c) {
  /* Convert a GdkColor back to a gdk_color_parse compatible hex value.
   * Returns a freshly allocated buffer that should be freed.
   */
  return g_strdup_printf("#%02X%02X%02X", c->red >> 8, c->green >> 8, c->blue >> 8);
}

static void de_jong_resize_from_string(DeJong *self, const gchar *s) {
  /* Set the current width and height from a WIDTH or WIDTHxHEIGHT in the given string */
  char *cptr;
  guint width, height;
  width = strtol(s, &cptr, 10);
  if (*cptr == 'x')
    height = atoi(cptr+1);
  else
    height = width;
  g_object_set(self, "width", width, "height", height, NULL);
}

static void update_double_if_necessary(double new_value, gboolean *dirty_flag, double *param, double epsilon) {
  if (fabs(new_value - *param) > epsilon) {
    *param = new_value;
    *dirty_flag = TRUE;
  }
}

static void update_uint_if_necessary(guint new_value, gboolean *dirty_flag, guint *param) {
  if (new_value != *param) {
    *param = new_value;
    *dirty_flag = TRUE;
  }
}

static void update_boolean_if_necessary(gboolean new_value, gboolean *dirty_flag, gboolean *param) {
  if (new_value != *param) {
    *param = new_value;
    *dirty_flag = TRUE;
  }
}

static void update_color_if_necessary(const gchar* new_value, gboolean *dirty_flag, GdkColor *param) {
  GdkColor new;
  gdk_color_parse(new_value, &new);
  if (new.red != param->red || new.green != param->green || new.blue != param->blue) {
    *param = new;
    *dirty_flag = TRUE;
  }
}

static void de_jong_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
  DeJong *self = DE_JONG(object);

  switch (prop_id) {

  case PROP_WIDTH:
    update_uint_if_necessary(g_value_get_uint(value), &self->size_dirty_flag, &self->width);
    break;

  case PROP_HEIGHT:
    update_uint_if_necessary(g_value_get_uint(value), &self->size_dirty_flag, &self->height);
    break;

  case PROP_OVERSAMPLE:
    update_uint_if_necessary(g_value_get_uint(value), &self->size_dirty_flag, &self->oversample);
    break;

  case PROP_SIZE:
    de_jong_resize_from_string(self, g_value_get_string(value));
    break;

  case PROP_A:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->a, 0.000009);
    break;

  case PROP_B:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->b, 0.000009);
    break;

  case PROP_C:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->c, 0.000009);
    break;

  case PROP_D:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->d, 0.000009);
    break;

  case PROP_ZOOM:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->zoom, 0.0009);
    break;

  case PROP_XOFFSET:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->xoffset, 0.0009);
    break;

  case PROP_YOFFSET:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->yoffset, 0.0009);
    break;

  case PROP_ROTATION:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->rotation, 0.0009);
    break;

  case PROP_BLUR_RADIUS:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->blur_radius, 0.00009);
    break;

  case PROP_BLUR_RATIO:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->blur_ratio, 0.00009);
    break;

  case PROP_TILEABLE:
    update_boolean_if_necessary(g_value_get_boolean(value), &self->calc_dirty_flag, &self->tileable);
    break;

  case PROP_EXPOSURE:
    update_double_if_necessary(g_value_get_double(value), &self->render_dirty_flag, &self->exposure, 0.00009);
    break;

  case PROP_GAMMA:
    update_double_if_necessary(g_value_get_double(value), &self->render_dirty_flag, &self->gamma, 0.00009);
    break;

  case PROP_FGCOLOR:
    update_color_if_necessary(g_value_get_string(value), &self->render_dirty_flag, &self->fgcolor);
    break;

  case PROP_BGCOLOR:
    update_color_if_necessary(g_value_get_string(value), &self->render_dirty_flag, &self->bgcolor);
    break;

  case PROP_FGALPHA:
    update_uint_if_necessary(g_value_get_uint(value), &self->render_dirty_flag, &self->fgalpha);
    break;

  case PROP_BGALPHA:
    update_uint_if_necessary(g_value_get_uint(value), &self->render_dirty_flag, &self->bgalpha);
    break;

  case PROP_CLAMPED:
    update_boolean_if_necessary(g_value_get_boolean(value), &self->render_dirty_flag, &self->clamped);
    break;

  case PROP_TARGET_DENSITY:
    self->target_density = g_value_get_ulong(value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void de_jong_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
  DeJong *self = DE_JONG(object);
}

void de_jong_reset_to_defaults(DeJong *self) {
}

gchar* de_jong_save_parameters(DeJong *self) {
}

void de_jong_load_parameters(DeJong *self, const gchar *params) {
  /* Load all recognized parameters from a string given in the same
   * format as the one produced by save_parameters()
   */
  gchar *copy, *line, *nextline;
  gchar *key, *value;

  /* Always start with defaults */
  set_defaults();

  /* Make a copy of the parameters, since we'll be modifying it */
  copy = g_strdup(params);

  /* Iterate over lines... */
  line = copy;
  while (line) {
    nextline = strchr(line, '\n');
    if (nextline) {
      *nextline = '\0';
      nextline++;
    }

    /* Separate it into key and value */
    key = g_malloc(strlen(line)+1);
    value = g_malloc(strlen(line)+1);
    if (sscanf(line, " %s = %s", key, value) == 2) {
      set_parameter(key, value);
    }
    g_free(key);
    line = nextline;
  }
  g_free(copy);
}


/************************************************************************************/
/************************************************************************* File I/O */
/************************************************************************************/

void de_jong_load_image_file(DeJong *self, const gchar *filename) {
  /* Try to open the given PNG file and load parameters from it */
  const gchar *params;
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
  params = gdk_pixbuf_get_option(pixbuf, "tEXt::de_jong_params");
  if (params)
    de_jong_load_parameters(self, params);
  else
    printf("No parameters chunk found\n");
  gdk_pixbuf_unref(pixbuf);
}

void de_jong_save_image_file(DeJong *self, const gchar *filename) {
  /* Save our current image to a .PNG file */
  gchar *params;

  /* Get a higher quality rendering */
  update_pixels();

  /* Save our current parameters in a tEXt chunk, using a format that
   * is both human-readable and easy to load parameters from automatically.
   */
  params = de_jong_save_parameters(self);
  gdk_pixbuf_save(self->image, filename, "png", NULL, "tEXt::de_jong_params", params, NULL);
  g_free(params);
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void de_jong_calculate(DeJong *self, guint iterations) {
}


/************************************************************************************/
/****************************************************************** Image Rendering */
/************************************************************************************/

void de_jong_update_image(DeJong *self) {
}

/* The End */
