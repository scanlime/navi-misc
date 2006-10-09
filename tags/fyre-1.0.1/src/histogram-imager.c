/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * histogram-imager.c - An object that stores a 2D histogram and generates
 *                      images from it. Supports oversampling, gamma correction,
 *                      color interpolation, and exposure adjustment.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include "histogram-imager.h"
#include "var-int.h"
#include "image-fu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static void histogram_imager_class_init (HistogramImagerClass *klass);
static void histogram_imager_init_size_params (GObjectClass *object_class);
static void histogram_imager_init_render_params (GObjectClass *object_class);
static void histogram_imager_dispose (GObject *gobject);
static void histogram_imager_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void histogram_imager_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void histogram_imager_resize_from_string (HistogramImager *self, const gchar *s);

static void histogram_imager_generate_color_table (HistogramImager *self, gboolean force);

static void histogram_imager_check_dirty_flags (HistogramImager *self);
static void histogram_imager_require_histogram (HistogramImager *self);
static void histogram_imager_require_image (HistogramImager *self);
static void histogram_imager_require_oversample_tables (HistogramImager *self);
static gulong histogram_imager_get_max_usable_density (HistogramImager *self);

static gboolean update_double_if_necessary (gdouble new_value, gboolean *dirty_flag, gdouble *param, gdouble epsilon);
static gboolean update_uint_if_necessary (guint new_value, gboolean *dirty_flag, guint *param);
static gboolean update_boolean_if_necessary (gboolean new_value, gboolean *dirty_flag, gboolean *param);
static gboolean update_color_if_necessary (const GdkColor* new_value, gboolean *dirty_flag, GdkColor *param);
static gchar* describe_color (GdkColor *c);

enum {
    PROP_0,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_OVERSAMPLE,
    PROP_OVERSAMPLE_ENABLED,
    PROP_SIZE,
    PROP_EXPOSURE,
    PROP_GAMMA,
    PROP_OVERSAMPLE_GAMMA,
    PROP_FGCOLOR,
    PROP_BGCOLOR,
    PROP_FGALPHA,
    PROP_BGALPHA,
    PROP_CLAMPED,
    PROP_FGCOLOR_GDK,
    PROP_BGCOLOR_GDK,
};

static gpointer parent_class = NULL;

#define fyre_histogram_imager_error_quark() (g_quark_from_string("FYRE_HISTOGRAM_IMAGER_ERROR"))
typedef enum {
    FYRE_HISTOGRAM_IMAGER_ERROR_NO_METADATA,
} FyreHistogramImagerError;


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType
histogram_imager_get_type (void)
{
    static GType dj_type = 0;

    if (!dj_type) {
	static const GTypeInfo dj_info = {
	    sizeof (HistogramImagerClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) histogram_imager_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof (HistogramImager),
	    0,
	    NULL, /* instance init */
	};

	dj_type = g_type_register_static (PARAMETER_HOLDER_TYPE, "HistogramImager", &dj_info, 0);
    }

    return dj_type;
}

static void
histogram_imager_class_init (HistogramImagerClass *klass)
{
    GObjectClass *object_class;

    parent_class = g_type_class_ref (G_TYPE_OBJECT);
    object_class = (GObjectClass*) klass;

    object_class->set_property = histogram_imager_set_property;
    object_class->get_property = histogram_imager_get_property;
    object_class->dispose      = histogram_imager_dispose;

    histogram_imager_init_size_params (object_class);
    histogram_imager_init_render_params (object_class);
}

static void
histogram_imager_init_size_params (GObjectClass *object_class)
{
    GParamSpec *spec;
    const gchar *current_group = "Image Size";

    spec = g_param_spec_uint         ("width",
				      "Width",
				      "Width of the rendered image, in pixels",
				      1, 32767, 600,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 1, 16, 0);
    g_object_class_install_property  (object_class, PROP_WIDTH, spec);

    spec = g_param_spec_uint         ("height",
				      "Height",
				      "Height of the rendered image, in pixels",
				      1, 32767, 600,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 1, 16, 0);
    g_object_class_install_property  (object_class, PROP_HEIGHT, spec);

    spec = g_param_spec_uint         ("oversample",
				      "Oversampling",
				      "Oversampling factor, 1 for no oversampling to 4 for heavy oversampling",
				      1, 4, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 1, 1, 0);
    g_object_class_install_property  (object_class, PROP_OVERSAMPLE, spec);

    spec = g_param_spec_boolean      ("oversample_enabled",
				      "Oversampling Enabled",
				      "Indicates when oversampling has been enabled with the 'oversample' property",
				      FALSE,
				      G_PARAM_READABLE);
    g_object_class_install_property  (object_class, PROP_OVERSAMPLE_ENABLED, spec);

    spec = g_param_spec_string       ("size",
				      "Size",
				      "Image size as a WIDTH or WIDTHxHEIGHT string",
				      NULL,
				      G_PARAM_READWRITE);
    g_object_class_install_property  (object_class, PROP_SIZE, spec);
}


static void
histogram_imager_init_render_params (GObjectClass *object_class)
{
    GParamSpec *spec;
    const gchar *current_group = "Rendering";

    spec = g_param_spec_double       ("exposure",
				      "Exposure",
				      "The relative strength, darkness, or brightness of the image",
				      0, 100, 0.05,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    g_object_class_install_property  (object_class, PROP_EXPOSURE, spec);

    spec = g_param_spec_double       ("gamma",
				      "Gamma",
				      "A gamma correction applied while rendering the image",
				      0, 10, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.01, 0.1, 3);
    g_object_class_install_property  (object_class, PROP_GAMMA, spec);

    spec = g_param_spec_double       ("oversample_gamma",
				      "Oversampling gamma",
				      "Gamma correction used when downconverting oversampled histograms",
				      0, 10, 1.66,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.01, 0.1, 3);
    param_spec_set_dependency        (spec, "oversample-enabled");
    g_object_class_install_property  (object_class, PROP_OVERSAMPLE_GAMMA, spec);

    spec = g_param_spec_string       ("fgcolor",
				      "Foreground",
				      "The foreground color, as a color name or #RRGGBB hex triple",
				      "#000000",
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED);
    g_object_class_install_property  (object_class, PROP_FGCOLOR, spec);

    spec = g_param_spec_string       ("bgcolor",
				      "Background",
				      "The background color, as a color name or #RRGGBB hex triple",
				      "#FFFFFF",
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED);
    g_object_class_install_property  (object_class, PROP_BGCOLOR, spec);

    spec = g_param_spec_boxed        ("fgcolor_gdk",
				      "Foreground",
				      "The foreground color, as a GdkColor",
				      GDK_TYPE_COLOR,
				      G_PARAM_READWRITE | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    g_param_spec_set_qdata           (spec, g_quark_from_static_string("opacity-property"), "fgalpha");
    g_object_class_install_property  (object_class, PROP_FGCOLOR_GDK, spec);

    spec = g_param_spec_boxed        ("bgcolor_gdk",
				      "Background",
				      "The background color, as a GdkColor",
				      GDK_TYPE_COLOR,
				      G_PARAM_READWRITE | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    g_param_spec_set_qdata           (spec, g_quark_from_static_string("opacity-property"), "bgalpha");
    g_object_class_install_property  (object_class, PROP_BGCOLOR_GDK, spec);

    spec = g_param_spec_uint         ("fgalpha",
				      "Foreground alpha",
				      "The foreground color's opacity",
				      0, 65535, 65535,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE);
    g_object_class_install_property  (object_class, PROP_FGALPHA, spec);

    spec = g_param_spec_uint         ("bgalpha",
				      "Background alpha",
				      "The background color's opacity",
				      0, 65535, 65535,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE);
    g_object_class_install_property  (object_class, PROP_BGALPHA, spec);

    spec = g_param_spec_boolean      ("clamped",
				      "Clamped",
				      "When set, luminances are clamped to [0,1] before linear interpolation",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    g_object_class_install_property  (object_class, PROP_CLAMPED, spec);
}

static void
histogram_imager_dispose (GObject *gobject)
{
    HistogramImager *self = HISTOGRAM_IMAGER (gobject);

    if (self->histogram) {
	g_free (self->histogram);
	self->histogram = NULL;
    }
    if (self->image) {
	gdk_pixbuf_unref (self->image);
	self->image = NULL;
    }
    if (self->color_table.table) {
	g_free (self->color_table.table);
	self->color_table.table = NULL;
    }
    if (self->color_table.quality) {
	g_free (self->color_table.quality);
	self->color_table.quality = NULL;
    }
    if (self->oversample_tables.linearize) {
	g_free (self->oversample_tables.linearize);
	self->oversample_tables.linearize = NULL;
    }
    if (self->oversample_tables.nonlinearize) {
	g_free (self->oversample_tables.nonlinearize);
	self->oversample_tables.nonlinearize = NULL;
    }

    G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

HistogramImager*
histogram_imager_new ()
{
    return HISTOGRAM_IMAGER (g_object_new (histogram_imager_get_type (), NULL));
}


/************************************************************************************/
/*********************************************************************** Properties */
/************************************************************************************/

static gchar*
describe_color (GdkColor *c)
{
    /* Convert a GdkColor back to a gdk_color_parse compatible hex value.
     * Returns a freshly allocated buffer that should be freed.
     */
    return g_strdup_printf ("#%02X%02X%02X", c->red >> 8, c->green >> 8, c->blue >> 8);
}

static void
histogram_imager_resize_from_string (HistogramImager *self, const gchar *s)
{
    /* Set the current width and height from a WIDTH or WIDTHxHEIGHT in the given string */
    char *cptr;
    guint width, height;
    width = strtol (s, &cptr, 10);
    if (*cptr == 'x')
	height = atoi (cptr+1);
    else
	height = width;
    g_object_set (self, "width", width, "height", height, NULL);
}

static gboolean
update_double_if_necessary (double new_value, gboolean *dirty_flag, double *param, double epsilon)
{
    if (fabs (new_value - *param) > epsilon) {
	*param = new_value;
	*dirty_flag = TRUE;
	return TRUE;
    }
    return FALSE;
}

static gboolean
update_uint_if_necessary (guint new_value, gboolean *dirty_flag, guint *param)
{
    if (new_value != *param) {
	*param = new_value;
	*dirty_flag = TRUE;
	return TRUE;
    }
    return FALSE;
}

static gboolean
update_boolean_if_necessary (gboolean new_value, gboolean *dirty_flag, gboolean *param)
{
    if (new_value != *param) {
	*param = new_value;
	*dirty_flag = TRUE;
	return TRUE;
    }
    return FALSE;
}

static gboolean
update_color_if_necessary (const GdkColor* new_value, gboolean *dirty_flag, GdkColor *param)
{
    if (new_value->red != param->red || new_value->green != param->green || new_value->blue != param->blue) {
	*param = *new_value;
	*dirty_flag = TRUE;
	return TRUE;
    }
    return FALSE;
}

static void
histogram_imager_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    HistogramImager *self = HISTOGRAM_IMAGER(object);
    GdkColor gdkc;

    switch (prop_id) {

    case PROP_WIDTH:
	update_uint_if_necessary (g_value_get_uint (value), &self->size_dirty_flag, &self->width);
	break;

    case PROP_HEIGHT:
	update_uint_if_necessary (g_value_get_uint (value), &self->size_dirty_flag, &self->height);
	break;

    case PROP_OVERSAMPLE:
	if (update_uint_if_necessary (g_value_get_uint (value), &self->size_dirty_flag, &self->oversample))
	    g_object_notify (object, "oversample-enabled");
	break;

    case PROP_SIZE:
	histogram_imager_resize_from_string (self, g_value_get_string (value));
	break;

    case PROP_EXPOSURE:
	update_double_if_necessary (g_value_get_double (value), &self->render_dirty_flag, &self->exposure, 0.00009);
	break;

    case PROP_GAMMA:
	update_double_if_necessary (g_value_get_double (value), &self->render_dirty_flag, &self->gamma, 0.00009);
	break;

    case PROP_OVERSAMPLE_GAMMA:
	update_double_if_necessary (g_value_get_double (value), &self->render_dirty_flag, &self->oversample_gamma, 0.00009);
	break;

    case PROP_FGCOLOR:
	/* Convert to a GdkColor and set the fgcolor-gdk property. This is necessary
	 * so that notify signals attached to fgcolor-gdk are sent properly, and in
	 * general makes it cleaner.
	 */
	gdk_color_parse (g_value_get_string (value), &gdkc);
	g_object_set (self, "fgcolor-gdk", &gdkc, NULL);
	break;

    case PROP_BGCOLOR:
	/* And the same goes for background... */
	gdk_color_parse (g_value_get_string (value), &gdkc);
	g_object_set (self, "bgcolor-gdk", &gdkc, NULL);
	break;

    case PROP_FGCOLOR_GDK:
	update_color_if_necessary ((GdkColor*) g_value_get_boxed (value), &self->render_dirty_flag, &self->fgcolor);
	break;

    case PROP_BGCOLOR_GDK:
	update_color_if_necessary ((GdkColor*) g_value_get_boxed (value), &self->render_dirty_flag, &self->bgcolor);
	break;

    case PROP_FGALPHA:
	update_uint_if_necessary (g_value_get_uint (value), &self->render_dirty_flag, &self->fgalpha);
	break;

    case PROP_BGALPHA:
	update_uint_if_necessary (g_value_get_uint (value), &self->render_dirty_flag, &self->bgalpha);
	break;

    case PROP_CLAMPED:
	update_boolean_if_necessary (g_value_get_boolean (value), &self->render_dirty_flag, &self->clamped);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}

static void
histogram_imager_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    HistogramImager *self = HISTOGRAM_IMAGER (object);

    switch (prop_id) {

    case PROP_WIDTH:
	g_value_set_uint (value, self->width);
	break;

    case PROP_HEIGHT:
	g_value_set_uint (value, self->height);
	break;

    case PROP_OVERSAMPLE:
	g_value_set_uint (value, self->oversample);
	break;

    case PROP_OVERSAMPLE_ENABLED:
	g_value_set_boolean (value, self->oversample > 1);
	break;

    case PROP_CLAMPED:
	g_value_set_boolean (value, self->clamped);
	break;

    case PROP_EXPOSURE:
	g_value_set_double (value, self->exposure);
	break;

    case PROP_GAMMA:
	g_value_set_double (value, self->gamma);
	break;

    case PROP_OVERSAMPLE_GAMMA:
	g_value_set_double (value, self->oversample_gamma);
	break;

    case PROP_FGALPHA:
	g_value_set_uint (value, self->fgalpha);
	break;

    case PROP_BGALPHA:
	g_value_set_uint (value, self->bgalpha);
	break;

    case PROP_SIZE:
	g_value_set_string_take_ownership (value, g_strdup_printf ("%dx%d", self->width, self->height));
	break;

    case PROP_FGCOLOR:
	g_value_set_string_take_ownership (value, describe_color (&self->fgcolor));
	break;

    case PROP_BGCOLOR:
	g_value_set_string_take_ownership (value, describe_color (&self->bgcolor));
	break;

    case PROP_FGCOLOR_GDK:
	g_value_set_boxed (value, &self->fgcolor);
	break;

    case PROP_BGCOLOR_GDK:
	g_value_set_boxed (value, &self->bgcolor);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}


/************************************************************************************/
/************************************************************************ Image I/O */
/************************************************************************************/

void
histogram_imager_load_image_file (HistogramImager *self, const gchar *filename, GError **error)
{
    /* Try to open the given PNG file and load parameters from it */
    const gchar *params;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filename, error);
    if (!pixbuf)
	return;

    params = gdk_pixbuf_get_option (pixbuf, "tEXt::fyre_params");

    /* For backward compatibility with de Jong Explorer and early versions of Fyre */
    if (!params)
	params = gdk_pixbuf_get_option (pixbuf, "tEXt::de_jong_params");

    if (params) {
	parameter_holder_load_string (PARAMETER_HOLDER (self), params);
    } else {
	if (error != NULL) {
	    GError *nerror = g_error_new (fyre_histogram_imager_error_quark(),
			                  FYRE_HISTOGRAM_IMAGER_ERROR_NO_METADATA,
				          "The image does not contain Fyre metadata");
	    *error = nerror;
	}
    }
    gdk_pixbuf_unref (pixbuf);
}

void
histogram_imager_save_image_file (HistogramImager *self, const gchar *filename, GError **error)
{
    /* Save our current image to a .PNG file */
    gchar *params;

    histogram_imager_update_image (self);

    /* Save our current parameters in a tEXt chunk, using a format that
     * is both human-readable and easy to load parameters from automatically.
     */
    params = parameter_holder_save_string (PARAMETER_HOLDER(self));
    gdk_pixbuf_save (self->image, filename, "png", error, "tEXt::fyre_params", params, NULL);
    g_free (params);
}

GdkPixbuf*
histogram_imager_make_thumbnail (HistogramImager *self, guint max_width, guint max_height)
{
    float aspect = ((float)self->width) / ((float)self->height);
    guint width, height;
    GdkPixbuf *thumb;

    /* Make sure the histogram is up to date */
    histogram_imager_update_image (self);

    /* Scale it down aspect-correctly */
    if (aspect > 1) {
	width = max_width;
	height = width / aspect;
    }
    else {
	height = max_height;
	width = height * aspect;
    }
    width = MAX(width, 5);
    height = MAX(height, 5);
    thumb = gdk_pixbuf_scale_simple (self->image, width, height, GDK_INTERP_BILINEAR);

    /* Do an in-place composite of a checkerboard behind this image, to make alpha visible */
    image_add_checkerboard(thumb);

    /* If the image is particularly small, enhance its visibility */
    if (width < 128 || height < 128)
	image_adjust_levels(thumb);

    /* Put a standard frame around it */
    image_add_thumbnail_frame(thumb);

    return thumb;
}


/************************************************************************************/
/************************************************************************* Plotting */
/************************************************************************************/

void
histogram_imager_get_hist_size (HistogramImager *self,
				int             *hist_width,
				int             *hist_height)
{
    if (hist_width)
	*hist_width = self->width * self->oversample;
    if (hist_height)
	*hist_height = self->height * self->oversample;
}

void
histogram_imager_prepare_plots (HistogramImager *self,
				HistogramPlot   *plot)
{
    histogram_imager_check_dirty_flags(self);
    histogram_imager_require_histogram(self);
    plot->histogram = self->histogram;
    plot->hist_width = self->width * self->oversample;
    plot->density = 0;
    plot->plot_count = 0;
}

void
histogram_imager_finish_plots (HistogramImager *self,
			       HistogramPlot   *plot)
{
    self->total_points_plotted += plot->plot_count;
    if (plot->density > self->peak_density)
	self->peak_density = plot->density;
}


/************************************************************************************/
/************************************************************************ Rendering */
/************************************************************************************/

void
histogram_imager_update_image (HistogramImager *self)
{
    /* Convert our histogram counts to an 8-bit ARGB image data using our color lookup table,
     * downsampling by combining all count buckets that represent each of our output pixels.
     */
    histogram_imager_check_dirty_flags (self);
    histogram_imager_require_histogram (self);
    histogram_imager_require_image (self);
    histogram_imager_generate_color_table (self, TRUE);

    {
	guint32 *pixel_p;
	guint32* const color_table = self->color_table.table;
	guint *hist_p, *sample_p;
	guint count, hist_clamp;
	const guint oversample = self->oversample;
	int x, y;

	pixel_p = (guint32*) gdk_pixbuf_get_pixels (self->image);
	hist_p = self->histogram;

	/* Clamp count values to the size of our color table.
	 * Assuming the color table generator did it's job
	 * correctly, any count values higher than the maximum
	 * one in the table would generate the same color as
	 * the highest one.
	 */
	hist_clamp = self->color_table.filled_size - 1;

	if (oversample > 1) {
	    /* Nice ugly loop that downsamples multiple (oversample^2)
	     * histogram buckets to each pixel
	     */

	    const int sample_stride = (self->width * oversample) - oversample;
	    const int sample_y_stride = (self->width * oversample) * (oversample - 1);
	    guint* linearize_table;
	    guint8* nonlinearize_table;
	    int sample_x, sample_y;
	    int ch0, ch1, ch2, ch3;
	    union {
		guint32 word;
		struct {
		    guchar ch0, ch1, ch2, ch3;
		} channels;
	    } sample_pixel;

	    histogram_imager_require_oversample_tables (self);
	    linearize_table = self->oversample_tables.linearize;
	    nonlinearize_table = self->oversample_tables.nonlinearize;

	    for (y=self->height; y; y--) {
		for (x=self->width; x; x--) {

		    /* Convert each oversampled input point to a color separately, then
		     * average the resulting colors using the ch0 through ch3 channel
		     * accumulators. Note that which channel is which depends on the
		     * machine's endianness, so we can't name them red, green, blue,
		     * and alpha here. This can be though of as dividing each pixel into
		     * and oversample-by-oversample grid of squares and plotting one
		     * histogram bucket in each, with antialiasing.
		     */

		    ch0 = ch1 = ch2 = ch3 = 0;
		    sample_p = hist_p;

		    for (sample_y=oversample; sample_y; sample_y--) {
			for (sample_x=oversample; sample_x; sample_x--) {

			    count = *(sample_p++);
			    if (count > hist_clamp)
				sample_pixel.word = color_table[hist_clamp];
			    else
				sample_pixel.word = color_table[count];

			    ch0 += linearize_table[sample_pixel.channels.ch0];
			    ch1 += linearize_table[sample_pixel.channels.ch1];
			    ch2 += linearize_table[sample_pixel.channels.ch2];
			    ch3 += linearize_table[sample_pixel.channels.ch3];
			}
			sample_p += sample_stride;
		    }
		    hist_p += oversample;

		    sample_pixel.channels.ch0 = nonlinearize_table[ch0];
		    sample_pixel.channels.ch1 = nonlinearize_table[ch1];
		    sample_pixel.channels.ch2 = nonlinearize_table[ch2];
		    sample_pixel.channels.ch3 = nonlinearize_table[ch3];
		    *(pixel_p++) = sample_pixel.word;
		}
		hist_p += sample_y_stride;
	    }
	}
	else {
	    /* A much simpler and faster loop to use when oversampling is disabled */

	    for (y=self->height; y; y--) {
		for (x=self->width; x; x--) {
		    count = *(hist_p++);
		    if (count > hist_clamp)
			*(pixel_p++) = color_table[hist_clamp];
		    else
			*(pixel_p++) = color_table[count];
		}
	    }
	}
    }
}

static void
histogram_imager_resize_color_table (HistogramImager *self, gulong size)
{
    /* Resize the color table to exactly 'size' entries. Upon completion,
     * self->color_table.filled_size will equal 'size', and
     * self->color_table.allocated_size will be at least 'size'.
     * If the current table is too small, this reallocates one twice as
     * large as necessary, since the required size usually grows. However,
     * if the required size is less than 1/10 the current size, the table will
     * shrink to twice the current minimum size.
     */

    self->color_table.filled_size = size;

    /* Just to reduce allocation overhead during those crucial first few frames,
     * put a lower limit on the amount of memory we're going to allocate.
     */
    if (size < 1024)
	size = 1024;

    if ((self->color_table.allocated_size < size) ||
	(self->color_table.allocated_size > 10 * size)) {
	if (self->color_table.table)
	    g_free (self->color_table.table);
	if (self->color_table.quality)
	    g_free (self->color_table.quality);

	/* Allocate it to double the size we need now, as we expect our needs to grow. */
	self->color_table.allocated_size = size * 2;
	self->color_table.table = g_malloc (self->color_table.allocated_size *
					    sizeof(self->color_table.table[0]));
	self->color_table.quality = g_malloc (self->color_table.allocated_size *
						sizeof(self->color_table.quality[0]));
    }
}

float
histogram_imager_get_pixel_scale (HistogramImager *self)
{
    /* Calculate the scale factor for converting histogram counts to
     * luminance values between 0 and 1.
     */
    float density, fscale;

    /* Avoid dividing by zero if we've plotted nothing */
    if (!self->total_points_plotted)
	return 0;

    /* Calculate the average histogram density */
    density = self->total_points_plotted / (self->width * self->height * self->oversample * self->oversample);

    /* fscale is a floating point number that, when multiplied by a raw
     * counts[] value, gives values between 0 and 1 corresponding to full
     * white and full black.
     */
    fscale = self->exposure / density;

    /* The very first frame we render will often be very underexposed.
     * If fscale > 0.5, this makes countsclamp negative and we get incorrect
     * results. The lowest usable value of countsclamp is 1.
     */
    if (fscale > 0.5)
	fscale = 0.5;

    return fscale;
}

static void
histogram_imager_generate_color_table (HistogramImager *self, gboolean force)
{
    /* Regenerate the contents of the color mapping table, a mapping from all
     * possible histogram values to the corresponding ARGB color, in the current image.
     */
    guint count;
    float pixel_scale = histogram_imager_get_pixel_scale (self);
    gulong usable_density = histogram_imager_get_max_usable_density (self);
    float luma;
    double one_over_gamma = 1/self->gamma;
    float distance = 0;
    gulong color_table_size;
    struct {
	int r, g, b, a;
    } current, previous;

    /* Our actual color table size should be either the maximum
     * usable density, or our histogram's current peak density,
     * whichever is smaller.
     */
    if (usable_density > self->peak_density)
	usable_density = self->peak_density;

    /* If the table is already the right size and we aren't being
     * forced to regenerate it, stop now.
     */
    color_table_size = usable_density + 1;
    if ((!force) && self->color_table.filled_size == color_table_size)
	return;

    /* Make sure our table is appropriately sized */
    histogram_imager_resize_color_table (self, color_table_size);

    /* Generate one color for every currently-possible count value that
     * doesn't fully saturate our image, as determined by histogram_imager_get_max_usable_density
     */
    for (count=0; count < self->color_table.filled_size; count++) {

	/* Scale and gamma-correct */
	luma = count * pixel_scale;
	luma = pow(luma, one_over_gamma);

	/* Optionally clamp before interpolating */
	if (self->clamped && luma > 1)
	    luma = 1;

	/* Linearly interpolate between fgcolor and bgcolor */
	current.r = ((int)(self->bgcolor.red   * (1-luma) + self->fgcolor.red   * luma)) >> 8;
	current.g = ((int)(self->bgcolor.green * (1-luma) + self->fgcolor.green * luma)) >> 8;
	current.b = ((int)(self->bgcolor.blue  * (1-luma) + self->fgcolor.blue  * luma)) >> 8;
	current.a = ((int)(self->bgalpha       * (1-luma) + self->fgalpha       * luma)) >> 8;

	/* Always clamp color components */
	if (current.r<0) current.r = 0;  if (current.r>255) current.r = 255;
	if (current.g<0) current.g = 0;  if (current.g>255) current.g = 255;
	if (current.b<0) current.b = 0;  if (current.b>255) current.b = 255;
	if (current.a<0) current.a = 0;  if (current.a>255) current.a = 255;

	/* Colors are always ARGB order in little endian */
	self->color_table.table[count] = IMAGEFU_COLOR(current.a, current.r, current.g, current.b);

	/* Update our elapsed distance */
	if (count > 0) {
	    distance += sqrt( (current.r - previous.r) * (current.r - previous.r) +
			      (current.g - previous.g) * (current.g - previous.g) +
			      (current.b - previous.b) * (current.b - previous.b) +
			      (current.a - previous.a) * (current.a - previous.a) );
	}
	previous = current;

	/* "distance" is the distance we've traveled from the background to this
	 * point in the color hypercube. Our quality metric is based on the number
	 * of histogram samples per color cube samples: our 'quality' table stores
	 * the current count divided by its corresponding distance.
	 */
	if (distance > 0) {
	    self->color_table.quality[count] = count / distance;
	}
	else {
	    /* We shouldn't ever use quality entries where the distance
	     * is zero- this value is pretty arbitrary.
	     */
	    self->color_table.quality[count] = 0;
	}
    }
}

static gulong
histogram_imager_get_max_usable_density (HistogramImager *self)
{
    /* This determines the highest histogram count value that will
     * produce any change in the color value of a pixel. This can be
     * used as an upper limit on the size of the color table. Note that
     * its value will change as the image is calculated, because it
     * depends on the pixel_scale.
     *
     * This function is basically the inverse of our color table's
     * function, mapping the most exposed color possible back to
     * a count value.
     */
    double max_luma, max_usable;

    if (self->clamped) {
	/* If clamping is on, the maximum useful luminance will always be 1. easy! */
	max_luma = 1;
    }
    else {
	/* But without clamping, it's possible for larger luminances to push
	 * our color 'past' the foreground color. Envision a vector in 4 dimensional
	 * RGBA space, pointing from background color to foreground color.
	 *
	 * This vector is represented by v_r, v_g, v_b, and v_a. We want to see how
	 * far we can extend this vector until the resulting color is fully saturated.
	 *
	 * The value of each component in the fully saturated color depends on the sign
	 * of the corresponding component in the vector. If it is zero, the resulting
	 * color will always equal the background color, If it's negative, it will
	 * eventually fall to 0, If it's positive, it will eventually reach 65535.
	 *
	 * Once we know what the fully saturated color is, we can easily use the
	 * per-channel deltas to find out how much luminance it would take to
	 * saturate each channel. The final max_luma is just the highest channel
	 * saturation luminance.
	 */
	int delta_r, delta_g, delta_b, delta_a;
	int clamped_r, clamped_g, clamped_b, clamped_a;
	double max_luma_r, max_luma_g, max_luma_b, max_luma_a;

	delta_r = self->fgcolor.red   - self->bgcolor.red;
	delta_g = self->fgcolor.green - self->bgcolor.green;
	delta_b = self->fgcolor.blue  - self->bgcolor.blue;
	delta_a = self->fgalpha       - self->bgalpha;

	if (delta_r > 0) clamped_r = 65535; else if (delta_r < 0) clamped_r = 0;
	else clamped_r = self->bgcolor.red;
	if (delta_g > 0) clamped_g = 65535; else if (delta_g < 0) clamped_g = 0;
	else clamped_g = self->bgcolor.green;
	if (delta_b > 0) clamped_b = 65535; else if (delta_b < 0) clamped_b = 0;
	else clamped_b = self->bgcolor.blue;
	if (delta_a > 0) clamped_a = 65535; else if (delta_a < 0) clamped_a = 0;
	else clamped_a = self->bgalpha;

	if (delta_r == 0) max_luma_r = 0;
	else max_luma_r = (((double)clamped_r) - self->bgcolor.red)   / delta_r;
	if (delta_g == 0) max_luma_g = 0;
	else max_luma_g = (((double)clamped_g) - self->bgcolor.green) / delta_g;
	if (delta_b == 0) max_luma_b = 0;
	else max_luma_b = (((double)clamped_b) - self->bgcolor.blue)  / delta_b;
	if (delta_a == 0) max_luma_a = 0;
	else max_luma_a = (((double)clamped_a) - self->bgalpha)       / delta_a;

	max_luma = 0;
	if (max_luma_r > max_luma) max_luma = max_luma_r;
	if (max_luma_g > max_luma) max_luma = max_luma_g;
	if (max_luma_b > max_luma) max_luma = max_luma_b;
	if (max_luma_a > max_luma) max_luma = max_luma_a;
    }

    /* Put max_luma through the inverse of our color table's gamma operation */
    max_luma = pow(max_luma, self->gamma);

    /* And now we can finally get the count value by dividing out our pixel scale */
    max_usable = max_luma / histogram_imager_get_pixel_scale(self) + 1;

    /* For some input values, it's possible for this to generate really
     * truly gigantic numbers. We clamp these to something a bit more
     * reasonable, to avoid arithmetic overflow later on.
     */
    if (max_usable > G_MAXINT/2)
	max_usable = G_MAXINT/2;
    return (gulong) max_usable;
}

gdouble
histogram_imager_compute_quality (HistogramImager *self)
{
    /* Compute a quality metric for the current histogram.
     * The algorithm is described in more detail in histogram-imager.h
     */
    histogram_imager_check_dirty_flags (self);
    histogram_imager_require_histogram (self);
    histogram_imager_generate_color_table (self, FALSE);
    {
	guint *row = self->histogram;
	guint *hist_p;
	float *qual_p = self->color_table.quality;
	guint count;
	guint hist_clamp = self->color_table.filled_size - 1;
	int width = self->width * self->oversample;
	int height = self->height * self->oversample;
	int x, y, x_scale, y_scale;
	guint stride;

	gulong denominator = 0;
	gulong num_saturated = 0;
	double numerator = 0;

	if (self->color_table.filled_size < 1)
	    return G_MAXDOUBLE;

	/* Sample the histogram at a reduced resolution, calculated
	 * such that we get about a 256x256 grid.
	 */
	x_scale = MAX(1, width >> 8);
	y_scale = MAX(1, height >> 8);

	y = height;
	stride = y_scale * width;
	while (y > 0) {

	    hist_p = row;
	    x = width;
	    while (x > 0) {
		count = *hist_p;

		/* We average only those buckets that fall within the output device's dynamic range */
		if (count > hist_clamp) {
		    num_saturated++;
		}
		else if (count > 0) {
		    numerator += qual_p[count];
		    denominator++;
		}

		x -= x_scale;
		hist_p += x_scale;
	    }

	    y -= y_scale;
	    row += stride;
	}

	if (!denominator)
	    return G_MAXDOUBLE;

	/* If the number of samples we have is less than 1% of the saturated
	 * samples, this is probably a very highly saturated image like a silhouette
	 * and it's done rendering.
	 */
	if (denominator < num_saturated/100)
	    return G_MAXDOUBLE;

	return numerator / denominator;
    }
}


/************************************************************************************/
/***************************************************************** Stream Buffering */
/************************************************************************************/

gsize
histogram_imager_export_stream (HistogramImager *self,
				guchar          *buffer,
				gsize            buffer_size)
{
    /* This encodes the contents of our histogram buffer
     * in a platform-independent and compact format suitable
     * for loading later with histogram_imager_merge_stream().
     *
     * This format is a form of run-length encoding, using var-int
     * to store our integers compactly. All numbers are left-shifted
     * one bit, and the least significant bit indicates meaning.
     * Values with an LSB of 0 indicate a number of buckets to skip,
     * and an LSB of 1 indicates a number of times to increment
     * the current bucket before skipping it.
     */

    guchar *output_p;
    int output_remaining;
    gint *hist_p;
    int hist_remaining;
    guint skipped = 0;
    int bucket;
    int i;

    histogram_imager_check_dirty_flags(self);
    histogram_imager_require_histogram(self);

    hist_p = self->histogram;
    hist_remaining = self->width * self->height *
	self->oversample * self->oversample;

    output_p = buffer;
    output_remaining = buffer_size - VAR_INT_MAX_SIZE;

    while (hist_remaining > 0 && output_remaining > 0) {
	bucket = *hist_p;
	if (bucket) {
	    /* We found a non-zero bucket */

	    if (skipped) {
		/* Output a skip value, if we skipped any
		 * buckets prior to the current one.
		 */
		i = var_int_write (output_p, skipped << 1);
		output_p += i;
		output_remaining -= i;
		if (output_remaining < 0)
		    break;
		skipped = 0;
	    }

	    /* Output this bucket's value, then clear it */
	    i = var_int_write (output_p, (bucket << 1) | 1);
	    output_p += i;
	    output_remaining -= i;
	    *hist_p = 0;
	}
	else {
	    skipped++;
	}

	hist_p++;
	hist_remaining--;
    }

    return output_p - buffer;
}

void
histogram_imager_merge_stream (HistogramImager *self,
			       const guchar    *buffer,
			       gsize            buffer_size)
{

    /* The inverse of histogram_imager_export_stream(). This follows
     * the skip/plot instructions in the given buffer, merging the
     * results with whatever happens to be in the histogram buffer.
     */

    const guchar *input_p;
    gsize input_remaining;
    gint *hist_p;
    gsize hist_remaining;
    guint token, bucket;
    HistogramPlot plot;
    int i;

    histogram_imager_prepare_plots (self, &plot);

    hist_p = self->histogram;
    hist_remaining = self->width * self->height *
	self->oversample * self->oversample;

    input_p = buffer;
    input_remaining = buffer_size;

    while (hist_remaining > 0 && input_remaining > 0) {
	i = var_int_read (input_p, &token);
	input_p += i;
	input_remaining -= i;

	if (token & 1) {
	    /* Plot into the current bucket */

	    token >>= 1;
	    plot.plot_count += token;
	    bucket = *hist_p;
	    bucket += token;
	    *hist_p = bucket;
	    if (bucket > plot.density)
		plot.density = bucket;
	    hist_p++;
	}
	else {
	    /* Skip buckets */

	    token >>= 1;
	    hist_p += token;
	}
    }

    histogram_imager_finish_plots (self, &plot);
}


/************************************************************************************/
/************************************************************************ Utilities */
/************************************************************************************/

static void
histogram_imager_check_dirty_flags (HistogramImager *self)
{
    /* Check dirty flags, invalidating stale data */

    if (self->size_dirty_flag) {
	/* We've resized. Deallocate the histogram and image
	 * if they've been allocated, and set the render and
	 * calc dirty flags.
	 */
	if (self->histogram) {
	    g_free (self->histogram);
	    self->histogram = NULL;
	}
	if (self->image) {
	    gdk_pixbuf_unref (self->image);
	    self->image = NULL;
	}

	self->render_dirty_flag = TRUE;
	self->size_dirty_flag = FALSE;
    }
}

static void
histogram_imager_require_histogram (HistogramImager *self)
{
    /* Allocate a histogram if we don't have one already */
    if (!self->histogram) {
	self->histogram = g_malloc (sizeof (self->histogram[0]) *
				    self->width * self->height *
				    self->oversample * self->oversample);
	histogram_imager_clear (self);
    }
}

void
histogram_imager_clear (HistogramImager *self)
{
    histogram_imager_check_dirty_flags (self);
    if (self->histogram) {
	memset (self->histogram, 0, sizeof (self->histogram[0]) *
	        self->width * self->height *
	        self->oversample * self->oversample);
    }
    self->histogram_clear_flag = TRUE;
    self->render_dirty_flag = TRUE;
    self->total_points_plotted = 0;
    self->peak_density = 0;
    g_get_current_time (&self->render_start_time);
}

gdouble
histogram_imager_get_elapsed_time (HistogramImager *self)
{
    GTimeVal now;
    g_get_current_time (&now);
    return ((now.tv_usec - self->render_start_time.tv_usec) / 1000000.0 +
	    (now.tv_sec  - self->render_start_time.tv_sec ));
}

static void
histogram_imager_require_image (HistogramImager *self)
{
    /* Allocate an image pixbuf if we don't have one already */
    if (!self->image) {
	self->image = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, self->width, self->height);
	self->render_dirty_flag = TRUE;
    }
}

static void
histogram_imager_require_oversample_tables (HistogramImager *self)
{
    /* Allocate or regenerate the oversample tables as necessary. */
    gboolean need_realloc = FALSE;
    gboolean need_regenerate = FALSE;

    /* Bits of precision to use in the intermediate linear values.
     * Increasing this increases the accuracy of the oversampled image,
     * to some extent, but exponentially increases memory usage for
     * the nonlinearize table.
     */
    const guint32 linear_bits = 12;

    const guint32 nonlinearize_table_size = (1<<linear_bits) * self->oversample * self->oversample;

    if (self->oversample_tables.oversample != self->oversample)
	need_realloc = TRUE;
    if (self->oversample_tables.gamma != self->oversample_gamma)
	need_regenerate = TRUE;
    if (!self->oversample_tables.linearize || !self->oversample_tables.nonlinearize)
	need_realloc = TRUE;

    if (need_realloc) {
	if (self->oversample_tables.linearize)
	    g_free (self->oversample_tables.linearize);
	self->oversample_tables.linearize = g_new(guint, 256);

	if (self->oversample_tables.nonlinearize)
	    g_free (self->oversample_tables.nonlinearize);
	self->oversample_tables.nonlinearize = g_new(guint8, nonlinearize_table_size);

	self->oversample_tables.oversample = self->oversample;
	need_regenerate = TRUE;
    }

    if (need_regenerate) {
	int i;
	guint* ip;
	guint8* bp;
	gdouble gamma = self->oversample_gamma;
	gdouble inv_gamma = 1/gamma;

	/* Generate the first mapping, from 8-bit nonlinear to linear_bits linear */
	ip = self->oversample_tables.linearize;
	for (i=0; i<256; i++)
	    *(ip++) = (int) (pow(i / 255.0, gamma) * ((1<<linear_bits) - 1) + 0.5);

	/* Generate the second mapping, which goes from oversample^2 summed results
	 * of the above table back to an 8-bit nonlinear result.
	 */
	bp = self->oversample_tables.nonlinearize;
	for (i=0; i<nonlinearize_table_size; i++)
	    *(bp++) = (int) (pow(i / (nonlinearize_table_size-1.0), inv_gamma) * 255 + 0.5);
    }
}

/* The End */
