/*
 * de-jong.c - The DeJong object builds on the ParameterHolder and HistogramRender
 *             objects to provide a rendering of the DeJong map into a histogram image.
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
#include "math-util.h"
#include <stdlib.h>
#include <math.h>

static void de_jong_class_init(DeJongClass *klass);
static void de_jong_init(DeJong *self);
static void de_jong_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void de_jong_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void de_jong_reset_calc(DeJong *self);

static void update_double_if_necessary(gdouble new_value, gboolean *dirty_flag, gdouble *param, gdouble epsilon);
static void update_boolean_if_necessary(gboolean new_value, gboolean *dirty_flag, gboolean *param);

enum {
  PROP_0,
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

    dj_type = g_type_register_static(HISTOGRAM_IMAGER_TYPE, "DeJong", &dj_info, 0);
  }

  return dj_type;
}

static void de_jong_class_init(DeJongClass *klass) {
  GObjectClass *object_class;
  object_class = (GObjectClass*) klass;

  object_class->set_property = de_jong_set_property;
  object_class->get_property = de_jong_get_property;

  g_object_class_install_property(object_class,
				  PROP_A,
				  g_param_spec_double("a",
						      "A",
						      "de Jong parameter A",
						      -G_MAXDOUBLE, G_MAXDOUBLE, 1.41914,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_B,
				  g_param_spec_double("b",
						      "B",
						      "de Jong parameter B",
						      -G_MAXDOUBLE, G_MAXDOUBLE, -2.28413,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_C,
				  g_param_spec_double("c",
						      "C",
						      "de Jong parameter C",
						      -G_MAXDOUBLE, G_MAXDOUBLE, 2.42754,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_D,
				  g_param_spec_double("d",
						      "D",
						      "de Jong parameter D",
						      -G_MAXDOUBLE, G_MAXDOUBLE, -2.17719,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_ZOOM,
				  g_param_spec_double("zoom",
						      "Zoom",
						      "Zoom factor",
						      0.2, 1000, 1,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_XOFFSET,
				  g_param_spec_double("xoffset",
						      "X Offset",
						      "Horizontal image offset",
						      -G_MAXDOUBLE, G_MAXDOUBLE, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_YOFFSET,
				  g_param_spec_double("yoffset",
						      "Y Offset",
						      "Vertical image offset",
						      -G_MAXDOUBLE, G_MAXDOUBLE, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_ROTATION,
				  g_param_spec_double("rotation",
						      "Rotation",
						      "Rotation angle, in radians",
						      -G_MAXDOUBLE, G_MAXDOUBLE, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_BLUR_RADIUS,
				  g_param_spec_double("blur_radius",
						      "Blur Radius",
						      "Gaussian blur radius",
						      0, 100, 0,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_BLUR_RATIO,
				  g_param_spec_double("blur_ratio",
						      "Blur Ratio",
						      "Amount of blurred vs non-blurred rendering",
						      0, 1, 1,
						      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_SERIALIZED |
						      G_PARAM_LAX_VALIDATION | G_PARAM_INTERPOLATE));

  g_object_class_install_property(object_class,
				  PROP_TILEABLE,
				  g_param_spec_boolean("tileable",
						       "Tileable",
						       "When set, the image is wrapped rather than clipped at the edges",
						       FALSE,
						       G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
						       G_PARAM_SERIALIZED | G_PARAM_INTERPOLATE));
}

static void de_jong_init(DeJong *self) {
  /* Nothing to do here yet, everything's set up by our G_PARAM_CONSTRUCT properties */
}

DeJong* de_jong_new() {
  return DE_JONG(g_object_new(de_jong_get_type(), NULL));
}


/************************************************************************************/
/*********************************************************************** Properties */
/************************************************************************************/

static void update_double_if_necessary(double new_value, gboolean *dirty_flag, double *param, double epsilon) {
  if (fabs(new_value - *param) > epsilon) {
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

static void de_jong_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec) {
  DeJong *self = DE_JONG(object);

  switch (prop_id) {

  case PROP_A:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->param.a, 0.000009);
    break;

  case PROP_B:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->param.b, 0.000009);
    break;

  case PROP_C:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->param.c, 0.000009);
    break;

  case PROP_D:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->param.d, 0.000009);
    break;

  case PROP_ZOOM:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->zoom, 0.0009);
    break;

  case PROP_XOFFSET:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->xoffset, 0.000001);
    break;

  case PROP_YOFFSET:
    update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->yoffset, 0.000001);
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

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void de_jong_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
  DeJong *self = DE_JONG(object);

  switch (prop_id) {

  case PROP_A:
    g_value_set_double(value, self->param.a);
    break;

  case PROP_B:
    g_value_set_double(value, self->param.b);
    break;

  case PROP_C:
    g_value_set_double(value, self->param.c);
    break;

  case PROP_D:
    g_value_set_double(value, self->param.d);
    break;

  case PROP_ZOOM:
    g_value_set_double(value, self->zoom);
    break;

  case PROP_XOFFSET:
    g_value_set_double(value, self->xoffset);
    break;

  case PROP_YOFFSET:
    g_value_set_double(value, self->yoffset);
    break;

  case PROP_ROTATION:
    g_value_set_double(value, self->rotation);
    break;

  case PROP_BLUR_RADIUS:
    g_value_set_double(value, self->blur_radius);
    break;

  case PROP_BLUR_RATIO:
    g_value_set_double(value, self->blur_ratio);
    break;

  case PROP_TILEABLE:
    g_value_set_boolean(value, self->tileable);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void de_jong_calculate(DeJong *self, guint iterations) {
  /* Copy frequently used parameters to local variables */
  const gboolean tileable = self->tileable;
  const DeJongParams param = self->param;

  /* Histogram state */
  HistogramPlot plot;
  int hist_width, hist_height;

  /* Toggles to disable features that aren't needed */
  const gboolean rotation_enabled = self->rotation > 0.0001 || self->rotation < -0.0001;
  const gboolean blur_enabled = self->blur_ratio > 0.0001 && self->blur_radius > 0.00001;

  /* Blurring variables */
  int blur_table_size;
  int blur_ratio_period;
  int blur_index, blur_ratio_index, blur_ratio_threshold;
  float *blur_table;

  /* Rotation variables */
  double sine_rotation, cosine_rotation;

  /* Iteration and projection variables */
  double x, y, point_x, point_y;
  double scale, xcenter, ycenter;
  int i, ix, iy;

  /* Reset calculation if we need to */
  if (self->calc_dirty_flag || HISTOGRAM_IMAGER(self)->histogram_clear_flag)
    de_jong_reset_calc(self);

  /* Ask the histogram imager to prepare a group of plots */
  histogram_imager_prepare_plots(HISTOGRAM_IMAGER(self), &plot);
  histogram_imager_get_hist_size(HISTOGRAM_IMAGER(self), &hist_width, &hist_height);

  /* Calculate the scale and offset in histogram coordinates */
  scale = hist_width / 5.0 * self->zoom;
  xcenter = hist_width / 2.0 + self->xoffset * scale;
  ycenter = hist_height / 2.0 + self->yoffset * scale;

  /* Precalculate the sine and cosine of the rotation angle, if we'll need it */
  if (rotation_enabled) {
    sine_rotation = sin(self->rotation);
    cosine_rotation = cos(self->rotation);
  }

  /* Initialize the blur table with a set of precalculated normally distributed
   * random numbers. Larger blur tables just increase the independence between
   * blocks of iterations. Since a new blur table is calculated at each run_iterations,
   * at infinity the image still has the same effect, but each iteration runs much faster.
   */
  if (blur_enabled) {
    /* Find a good size for the blur table. Our current heuristic finds
     * the smallest power of two that's still larger than 1/50 our iteration count.
     */
    blur_table_size = find_upper_pow2(iterations / 50);

    /* Allocate and fill the blur table */
    blur_table = alloca(blur_table_size * sizeof(blur_table[0]));
    for (i=0; i<blur_table_size; i++)
      blur_table[i] = normal_variate() * self->blur_radius;
    blur_index = 0;

    /* Initialize the blur ratio counter and threshold */
    blur_ratio_index = 0;
    blur_ratio_period = 1024;
    blur_ratio_threshold = self->blur_ratio * blur_ratio_period;
  }

  point_x = self->point_x;
  point_y = self->point_y;

  for(i=iterations; i; --i) {
    /* These are the actual Peter de Jong map equations. The new point value
     * gets stored into 'point', then we go on and mess with x and y before plotting.
     */
    x = sin(param.a * point_y) - cos(param.b * point_x);
    y = sin(param.c * point_x) - cos(param.d * point_y);
    point_x = x;
    point_y = y;

    /* If rotation is enabled, rotate each point around
     * the origin by self->rotation radians.
     */
    if (rotation_enabled) {
      x =  cosine_rotation * point_x + sine_rotation   * point_y;
      y = -sine_rotation   * point_x + cosine_rotation * point_y;
    }

    /* If blurring is enabled, use blur_ratio to decide how often to perturb
     * the apparent point position, and blur_radius to determine how much.
     * By perturbing the point using a normal variate, we create a true gaussian
     * blur as the number of iterations approaches infinity.
     */
    if (blur_enabled) {
      if (blur_ratio_index < blur_ratio_threshold) {
	x += blur_table[blur_index];
	blur_index = (blur_index+1) & (blur_table_size-1);
	y += blur_table[blur_index];
	blur_index = (blur_index+1) & (blur_table_size-1);
      }
      blur_ratio_index = (blur_ratio_index+1) & (blur_ratio_period-1);
    }

    /* Scale and translate our (x,y) coordinates into pixel coordinates */
    x = x * scale + xcenter;
    y = y * scale + ycenter;

    /* Convert (x,y) to integers.
     * Note that just casting to int here is incorrect! We want the behaviour
     * of floor(), always rounding toward -inf rather than zero. This should
     * behave identically to floor(), but a little faster.
     */
    if (x<0)
      ix = x-1;
    else
      ix = x;
    if (y<0)
      iy = y-1;
    else
      iy = y;

    if (tileable) {
      /* In tileable rendering, we wrap at the edges */
      ix %= hist_width;
      iy %= hist_height;
      if (ix < 0) ix += hist_width;
      if (iy < 0) iy += hist_height;
    }
    else {
      /* Otherwise, clip off the edges.
       * Cast ix and iy to unsigned so our comparison against
       * the width/height also implicitly compares against zero.
       */
      if (((unsigned int)ix) >= hist_width  ||
	  ((unsigned int)iy) >= hist_height)
	continue;
    }

    HISTOGRAM_IMAGER_PLOT(plot, ix, iy);
  }

  histogram_imager_finish_plots(HISTOGRAM_IMAGER(self), &plot);
  self->iterations += iterations;
  self->point_x = point_x;
  self->point_y = point_y;
}

void de_jong_calculate_motion(DeJong               *self,
			      guint                 iterations,
			      gboolean              continuation,
			      ParameterInterpolator *interp,
			      gpointer              interp_data) {
  /* The equivalent of de_jong_calculate, but for an object moving according
   * to a given interpolation function.
   *
   * The given number of iterations are divided into smaller blocks, each of
   * which are run with different random coordinates between a and b. This gives
   * us accurate motion blur almost for free. Since the existing interpolated
   * parameters of this DeJong object are ignored, the 'continuation' flag must be
   * set on all but the first call for rendering to be reset properly.
   */
  const guint blocksize = iterations / 10;
  guint count;

  for (count=0; count<iterations; count+=blocksize) {
    interp(PARAMETER_HOLDER(self), uniform_variate(), interp_data);
    self->calc_dirty_flag = !continuation;
    de_jong_calculate(self, blocksize);
  }
}

static void de_jong_reset_calc(DeJong *self) {
  /* Reset the histogram and calculation state */
  histogram_imager_clear(HISTOGRAM_IMAGER(self));
  self->iterations = 0;

  /* Random starting point */
  self->point_x = uniform_variate();
  self->point_y = uniform_variate();

  HISTOGRAM_IMAGER(self)->histogram_clear_flag = FALSE;
  self->calc_dirty_flag = FALSE;
}

/* The End */
