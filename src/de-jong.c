/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * de-jong.c - The DeJong object builds on the ParameterHolder and HistogramRender
 *             objects to provide a rendering of the DeJong map into a histogram image.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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
static void de_jong_init_calc_params(GObjectClass *object_class);
static void de_jong_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void de_jong_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void de_jong_reset_calc(DeJong *self);
static void de_jong_calculate(IterativeMap *self, guint iterations);
static void de_jong_calculate_motion(IterativeMap *self, guint iterations, gboolean continuation, ParameterInterpolator *interp, gpointer interp_data);
static ToolInfoPH *de_jong_get_tools();

static void update_double_if_necessary(gdouble new_value, gboolean *dirty_flag, gdouble *param, gdouble epsilon);
static void update_boolean_if_necessary(gboolean new_value, gboolean *dirty_flag, gboolean *param);

enum {
    PROP_0,
    PROP_FUNCTION,
    PROP_A,
    PROP_B,
    PROP_C,
    PROP_D,
    PROP_ZOOM,
    PROP_ASPECT,
    PROP_XOFFSET,
    PROP_YOFFSET,
    PROP_ROTATION,
    PROP_BLUR_RADIUS,
    PROP_BLUR_RATIO,
    PROP_TILEABLE,
    PROP_EMPHASIZE_TRANSIENT,
    PROP_TRANSIENT_ITERATIONS,
    PROP_INITIAL_CONDITIONS,
    PROP_INITIAL_XSCALE,
    PROP_INITIAL_YSCALE,
    PROP_INITIAL_XOFFSET,
    PROP_INITIAL_YOFFSET,
};

typedef void (*initial_conditions_t)(gdouble *x, gdouble *y);

void initial_func_square_uniform    (gdouble *x, gdouble *y);
void initial_func_gaussian          (gdouble *x, gdouble *y);
void initial_func_circular_uniform  (gdouble *x, gdouble *y);
void initial_func_radial            (gdouble *x, gdouble *y);
void initial_func_sphere            (gdouble *x, gdouble *y);

static const
GEnumValue initial_conditions_enum[] =
    {
	{ 0, "circular_uniform",  "Circular uniform"  },
	{ 1, "square_uniform",    "Square uniform"    },
	{ 2, "gaussian",          "Gaussian"          },
	{ 3, "radial",            "Radial"            },
	{ 4, "sphere",            "Sphere"            },
	{ 0 },
    };

static const
initial_conditions_t initial_conditions_table[] =
    {
	initial_func_circular_uniform,
	initial_func_square_uniform,
	initial_func_gaussian,
	initial_func_radial,
	initial_func_sphere,
    };

static GType initial_conditions_enum_get_type(void);

static void tool_grab(ParameterHolder *self, ToolInput *i);
static void tool_blur(ParameterHolder *self, ToolInput *i);
static void tool_zoom(ParameterHolder *self, ToolInput *i);
static void tool_rotate(ParameterHolder *self, ToolInput *i);
static void tool_exposure_gamma(ParameterHolder *self, ToolInput *i);
static void tool_a_b(ParameterHolder *self, ToolInput *i);
static void tool_a_c(ParameterHolder *self, ToolInput *i);
static void tool_a_d(ParameterHolder *self, ToolInput *i);
static void tool_b_c(ParameterHolder *self, ToolInput *i);
static void tool_b_d(ParameterHolder *self, ToolInput *i);
static void tool_c_d(ParameterHolder *self, ToolInput *i);
static void tool_ab_cd(ParameterHolder *self, ToolInput *i);
static void tool_ac_bd(ParameterHolder *self, ToolInput *i);

static const ToolInfoPH tool_table[] = {
    {"Grab",        tool_grab,           TOOL_USE_MOTION_EVENTS},
    {"Blur",        tool_blur,           TOOL_USE_MOTION_EVENTS},
    {"Zoom",        tool_zoom,           TOOL_USE_IDLE},
    {"Rotate",      tool_rotate,         TOOL_USE_MOTION_EVENTS},
    {"Gamma",       tool_exposure_gamma, TOOL_USE_MOTION_EVENTS},
    {"<separator>",},
    {"A / B",       tool_a_b,            TOOL_USE_MOTION_EVENTS},
    {"A / C",       tool_a_c,            TOOL_USE_MOTION_EVENTS},
    {"A / D",       tool_a_d,            TOOL_USE_MOTION_EVENTS},
    {"B / C",       tool_b_c,            TOOL_USE_MOTION_EVENTS},
    {"B / D",       tool_b_d,            TOOL_USE_MOTION_EVENTS},
    {"C / D",       tool_c_d,            TOOL_USE_MOTION_EVENTS},
    {"<separator>",},
    {"AB / CD",     tool_ab_cd,          TOOL_USE_MOTION_EVENTS},
    {"AC / BD",     tool_ac_bd,          TOOL_USE_MOTION_EVENTS},
    {NULL,},
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

	dj_type = g_type_register_static(ITERATIVE_MAP_TYPE, "DeJong", &dj_info, 0);
    }

    return dj_type;
}

static GType initial_conditions_enum_get_type(void) {
    static GType t = 0;

    if (!t)
	t = g_enum_register_static ("InitialConditions", initial_conditions_enum);

    return t;
}

static void de_jong_class_init(DeJongClass *klass) {
    GObjectClass *object_class;
    IterativeMapClass *im_class;
    ParameterHolderClass *ph_class;

    object_class = (GObjectClass*) klass;
    im_class = (IterativeMapClass*) klass;
    ph_class = (ParameterHolderClass*) klass;

    object_class->set_property = de_jong_set_property;
    object_class->get_property = de_jong_get_property;

    im_class->calculate = de_jong_calculate;
    im_class->calculate_motion = de_jong_calculate_motion;

    ph_class->get_tools = de_jong_get_tools;

    de_jong_init_calc_params(object_class);
}

static void de_jong_init_calc_params(GObjectClass *object_class) {
    GParamSpec *spec;
    const gchar *current_group = "Computation";

    spec = g_param_spec_string       ("function",
				      "Function",
				      "Function Name",
				      "Peter de Jong Map",
				      G_PARAM_READABLE);
    g_object_class_install_property  (object_class, PROP_FUNCTION, spec);

    spec = g_param_spec_double       ("a",
				      "A",
				      "de Jong parameter A",
				      -100, 100, 2.38767,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 5);
    g_object_class_install_property  (object_class, PROP_A, spec);

    spec = g_param_spec_double       ("b",
				      "B",
				      "de Jong parameter B",
				      -100, 100, -1.22713,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 5);
    g_object_class_install_property  (object_class, PROP_B, spec);

    spec = g_param_spec_double       ("c",
				      "C",
				      "de Jong parameter C",
				      -100, 100, -0.39595,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 5);
    g_object_class_install_property  (object_class, PROP_C, spec);

    spec = g_param_spec_double       ("d",
				      "D",
				      "de Jong parameter D",
				      -100, 100, -4.67104,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 5);
    g_object_class_install_property  (object_class, PROP_D, spec);

    spec = g_param_spec_double       ("zoom",
				      "Zoom",
				      "Zoom factor",
				      0.01, 1000, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.01, 0.1, 3);
    g_object_class_install_property  (object_class, PROP_ZOOM, spec);

    spec = g_param_spec_double       ("aspect",
				      "Aspect",
				      "Aspect ratio",
				      0.01, 100, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.1, 3);
    g_object_class_install_property  (object_class, PROP_ASPECT, spec);

    spec = g_param_spec_double       ("xoffset",
				      "X offset",
				      "Horizontal image offset",
				      -100, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    g_object_class_install_property  (object_class, PROP_XOFFSET, spec);

    spec = g_param_spec_double       ("yoffset",
				      "Y offset",
				      "Vertical image offset",
				      -100, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    g_object_class_install_property  (object_class, PROP_YOFFSET, spec);

    spec = g_param_spec_double       ("rotation",
				      "Rotation",
				      "Rotation angle, in radians",
				      -100, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    g_object_class_install_property  (object_class, PROP_ROTATION, spec);

    spec = g_param_spec_double       ("blur_radius",
				      "Blur radius",
				      "Gaussian blur radius",
				      0, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.0001, 0.001, 4);
    g_object_class_install_property  (object_class, PROP_BLUR_RADIUS, spec);

    spec = g_param_spec_double       ("blur_ratio",
				      "Blur ratio",
				      "Amount of blurred vs non-blurred rendering",
				      0, 1, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.01, 0.1, 4);
    g_object_class_install_property  (object_class, PROP_BLUR_RATIO, spec);

    spec = g_param_spec_boolean      ("tileable",
				      "Tileable",
				      "When set, the image is wrapped rather than clipped at the edges",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    g_object_class_install_property  (object_class, PROP_TILEABLE, spec);

    spec = g_param_spec_boolean      ("emphasize_transient",
				      "Emphasize transient",
				      "Re-randomize the point periodically to emphasize transients",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    g_object_class_install_property  (object_class, PROP_EMPHASIZE_TRANSIENT, spec);

    spec = g_param_spec_uint         ("transient_iterations",
				      "Transient iterations",
				      "Number of iterations between re-randomization, when 'Emphasize transient' is enabled",
				      1, 100000, 50,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 1, 10, 0);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_TRANSIENT_ITERATIONS, spec);

    spec = g_param_spec_enum         ("initial_conditions",
				      "Initial conditions",
				      "Selects the function used to generate initial conditions, when 'Emphasize transient' is enabled",
				      initial_conditions_enum_get_type(),
				      0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_INITIAL_CONDITIONS, spec);

    spec = g_param_spec_double       ("initial_xscale",
				      "Initial X scale",
				      "Horizontal initial condition scale factor",
				      0, 1000, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_INITIAL_XSCALE, spec);

    spec = g_param_spec_double       ("initial_yscale",
				      "Initial Y scale",
				      "Vertical initial condition scale factor",
				      0, 1000, 1,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_INITIAL_YSCALE, spec);

    spec = g_param_spec_double       ("initial_xoffset",
				      "Initial X offset",
				      "Horizontal initial condition offset",
				      -100, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_INITIAL_XOFFSET, spec);

    spec = g_param_spec_double       ("initial_yoffset",
				      "Initial Y offset",
				      "Vertical initial condition offset",
				      -100, 100, 0,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | PARAM_SERIALIZED |
				      G_PARAM_LAX_VALIDATION | PARAM_INTERPOLATE | PARAM_IN_GUI);
    param_spec_set_group             (spec, current_group);
    param_spec_set_increments        (spec, 0.001, 0.01, 3);
    param_spec_set_dependency        (spec, "emphasize-transient");
    g_object_class_install_property  (object_class, PROP_INITIAL_YOFFSET, spec);
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

static void update_uint_if_necessary(guint new_value, gboolean *dirty_flag, guint *param) {
    if (new_value != *param) {
	*param = new_value;
	*dirty_flag = TRUE;
    }
}

static void update_enum_if_necessary(gint new_value, gboolean *dirty_flag, gint *param) {
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

    case PROP_ASPECT:
	update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->aspect, 0.0009);
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

    case PROP_EMPHASIZE_TRANSIENT:
	update_boolean_if_necessary(g_value_get_boolean(value), &self->calc_dirty_flag, &self->emphasize_transient);
	break;

    case PROP_TRANSIENT_ITERATIONS:
	update_uint_if_necessary(g_value_get_uint(value), &self->calc_dirty_flag, &self->transient_iterations);
	break;

    case PROP_INITIAL_CONDITIONS:
	update_enum_if_necessary(g_value_get_enum(value), &self->calc_dirty_flag, &self->initial_conditions);
	break;

    case PROP_INITIAL_XOFFSET:
	update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->initial_xoffset, 0.000001);
	break;

    case PROP_INITIAL_YOFFSET:
	update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->initial_yoffset, 0.000001);
	break;

    case PROP_INITIAL_XSCALE:
	update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->initial_xscale, 0.0009);
	break;

    case PROP_INITIAL_YSCALE:
	update_double_if_necessary(g_value_get_double(value), &self->calc_dirty_flag, &self->initial_yscale, 0.0009);
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

    case PROP_ASPECT:
	g_value_set_double(value, self->aspect);
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

    case PROP_EMPHASIZE_TRANSIENT:
	g_value_set_boolean(value, self->emphasize_transient);
	break;

    case PROP_TRANSIENT_ITERATIONS:
	g_value_set_uint(value, self->transient_iterations);
	break;

    case PROP_INITIAL_CONDITIONS:
	g_value_set_enum(value, self->initial_conditions);
	break;

    case PROP_INITIAL_XOFFSET:
	g_value_set_double(value, self->initial_xoffset);
	break;

    case PROP_INITIAL_YOFFSET:
	g_value_set_double(value, self->initial_yoffset);
	break;

    case PROP_INITIAL_XSCALE:
	g_value_set_double(value, self->initial_xscale);
	break;

    case PROP_INITIAL_YSCALE:
	g_value_set_double(value, self->initial_yscale);
	break;

    default:
	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	break;
    }
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void de_jong_calculate(IterativeMap *map, guint iterations) {
    /* Copy frequently used parameters to local variables */
    DeJong *self = DE_JONG(map);
    HistogramImager *hi = HISTOGRAM_IMAGER(map);
    const gboolean tileable = self->tileable;
    const DeJongParams param = self->param;

    /* Histogram state */
    HistogramPlot plot;
    int hist_width, hist_height;

    /* Toggles to disable features that aren't needed */
    const gboolean rotation_enabled = self->rotation > 0.0001 || self->rotation < -0.0001;
    const gboolean blur_enabled = self->blur_ratio > 0.0001 && self->blur_radius > 0.00001;
    const gboolean aspect_enabled = self->aspect > 1.0001 || self->aspect < 0.9999;
    const gboolean matrix_enabled = aspect_enabled || rotation_enabled;
    const gboolean emphasize_transient = self->emphasize_transient;
    const gboolean oversample_enabled = hi->oversample > 1;

    /* Blurring variables */
    int blur_table_size = 0;
    int blur_ratio_period = 0;
    int blur_index = 0, blur_ratio_index = 0, blur_ratio_threshold = 0;
    float *blur_table = NULL;

    /* Oversampling irregularity table.
     * Fixed size for now. Must be a power of two! 
     */
    const int oversample_table_size = 32;
    int oversample_index = 0;
    float oversample_table[oversample_table_size];

    /* Rotation/aspect matrix variables */
    double sine_rotation, cosine_rotation, mat_a = 0, mat_b = 0, mat_c = 0, mat_d = 0;

    /* Iteration and projection variables */
    double x, y, point_x, point_y;
    double scale, xcenter, ycenter;
    int i, ix, iy;
    guint remaining_transient_iterations;
    initial_conditions_t initial_func;

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

    /* Set up the matrix used for rotation and aspect ratio adjustment */
    if (matrix_enabled) {
	if (rotation_enabled) {
	    sine_rotation = sin(self->rotation);
	    cosine_rotation = cos(self->rotation);
	    mat_a = cosine_rotation * self->aspect;
	    mat_b = sine_rotation / self->aspect;
	    mat_c = -sine_rotation * self->aspect;
	    mat_d = cosine_rotation / self->aspect;
	}
	else {
	    mat_a = self->aspect;
	    mat_b = 0;
	    mat_c = 0;
	    mat_d = 1/self->aspect;
	}
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
	for (i=0; i<blur_table_size; i+=2) {
	    double a, b;
	    normal_variate_pair(&a, &b);
	    blur_table[i] = a * self->blur_radius;
	    blur_table[i+1] = b * self->blur_radius;
	}
	blur_index = 0;

	/* Initialize the blur ratio counter and threshold */
	blur_ratio_index = 0;
	blur_ratio_period = 1024;
	blur_ratio_threshold = self->blur_ratio * blur_ratio_period;
    }

    /* Initialize the oversample irregularity table. When we're oversampling, we
     * add +/- 1 subpixel of noise to the image in order to achieve the same effect
     * as irregular-grid FSAA: avoiding unpleasant interference patterns by filtering
     * out very high-frequency details that will generate aliasing artifacts.
     */
    if (oversample_enabled) {
	for (i=0; i<oversample_table_size; i++)
	    oversample_table[i] = uniform_variate() * 2 - 1;
	oversample_index = 0;
    }	

    point_x = self->point_x;
    point_y = self->point_y;
    remaining_transient_iterations = self->remaining_transient_iterations;
    initial_func = initial_conditions_table[self->initial_conditions];

    for(i=iterations; i; --i) {

	/* If transient emphasis is enabled, we periodically re-randomize
	 * the point. When remaining_transient_iterations hits zero, we
	 * re-randomize and set it back to the transient iteration count
	 * set by the user.
	 */
	if (emphasize_transient) {
	    if (remaining_transient_iterations) {
		remaining_transient_iterations--;
	    }
	    else {
		remaining_transient_iterations = self->transient_iterations-1;
		initial_func(&point_x, &point_y);
		point_x = self->initial_xscale * point_x + self->initial_xoffset;
		point_y = self->initial_yscale * point_y + self->initial_yoffset;
	    }
	}

	/* These are the actual Peter de Jong map equations. The new point value
	 * gets stored into 'point', then we go on and mess with x and y before plotting.
	 */
	x = sin(param.a * point_y) - cos(param.b * point_x);
	y = sin(param.c * point_x) - cos(param.d * point_y);
	/*
	  x = sin(param.a * point_y) + param.c * cos(param.a * point_x);
	  y = sin(param.b * point_x) + param.d * cos(param.b * point_y);
	*/
	/*
	  x = sin(param.a * point_y) - tanh(param.b * point_x);
	  y = sin(param.c * point_x) - tanh(param.d * point_y);
	*/
	/*
	  x = sin(point_y * param.b) + param.c * sin(point_x * param.b);
	  y = sin(point_x * param.a) + param.d * sin(point_y * param.a);
	*/
	point_x = x;
	point_y = y;

	if (matrix_enabled) {
	    x = point_x * mat_a + point_y * mat_b;
	    y = point_x * mat_c + point_y * mat_d;
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

	/* Apply the random oversampling jitter, if applicable */
	if (oversample_enabled) {
	    x += oversample_table[oversample_index];
	    oversample_index = (oversample_index+1) & (oversample_table_size-1);
	    y += oversample_table[oversample_index];
	    oversample_index = (oversample_index+1) & (oversample_table_size-1);
	}


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
    ITERATIVE_MAP(self)->iterations += iterations;
    self->point_x = point_x;    self->point_y = point_y;
    self->remaining_transient_iterations = remaining_transient_iterations;
}

void de_jong_calculate_motion(IterativeMap         *self,
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
	DE_JONG(self)->calc_dirty_flag = !continuation;
	de_jong_calculate(self, blocksize);
    }
}

static void de_jong_reset_calc(DeJong *self) {
    /* Reset the histogram and calculation state */
    histogram_imager_clear(HISTOGRAM_IMAGER(self));
    ITERATIVE_MAP(self)->iterations = 0;
    self->remaining_transient_iterations = 0;

    /* Random starting point, use a simple uniform variate
     * for this. We have more complex initial condition controls
     * we use when emphasize_transient is on, but when it's off
     * the initial conditions have no effect on the image as iterations
     * approach infinity.
     */
    self->point_x = uniform_variate();
    self->point_y = uniform_variate();

    HISTOGRAM_IMAGER(self)->histogram_clear_flag = FALSE;
    self->calc_dirty_flag = FALSE;
}


/************************************************************************************/
/*************************************************************** Initial Conditions */
/************************************************************************************/

void initial_func_square_uniform (gdouble *x, gdouble *y) {
    /* From -1 to +1. The default used to be 0 to 1, which produced
     * some neat effects, but made a silly default. This, by default,
     * looks a lot like circular_uniform but with corners.
     */
    *x = uniform_variate()*2 - 1;
    *y = uniform_variate()*2 - 1;
}

void initial_func_gaussian (gdouble *x, gdouble *y) {
    /* Just a unit normal in each axis */
    normal_variate_pair(x, y);
}

void initial_func_circular_uniform (gdouble *x, gdouble *y) {
    /* A uniform distribution in each axis, but discarding
     * all values that fall outside the unit circle. This
     * gives a similar look to square_uniform, but with smooth
     * edges rather than corners.
     */
    gdouble i, j;
    do {
	i = uniform_variate()*2 - 1;
	j = uniform_variate()*2 - 1;
    } while ( (i*i + j*j) > 1 );
    *x = i;
    *y = j;
}

void initial_func_radial (gdouble *x, gdouble *y) {
    /* Pick a radius and angle uniformly, then convert to cartesian
     * coordinates. This also produces a unit circle, but it isn't
     * uniform- it has a strong dense spot in the center that fades
     * off toward the edges. Unlike gaussian, this still has distinct
     * edges.
     */
    gdouble theta = uniform_variate() * M_PI * 2;
    gdouble radius = uniform_variate();
    *x = cos(theta) * radius;
    *y = sin(theta) * radius;
}

void initial_func_sphere (gdouble *x, gdouble *y) {
    /* The opposite of radial's effect- a circle that's dense at
     * the edges and light in the center. This creates a distribution
     * uniform along the surface of a sphere, then flattens it.
     *
     * We currently implement this by normalizing the vector produced
     * by three normal variates- this is really slow.
     */
    gdouble vx, vy, vz, vq;

    normal_variate_pair(&vx, &vy);
    normal_variate_pair(&vz, &vq);

    gdouble mag = sqrt(vx*vx + vy*vy + vz*vz);
    *x = vx / mag;
    *y = vy / mag;
}


/************************************************************************************/
/**************************************************************************** Tools */
/************************************************************************************/

static ToolInfoPH *de_jong_get_tools() {
    return (ToolInfoPH*) tool_table;
}

static void tool_grab(ParameterHolder *self, ToolInput *i) {}
static void tool_blur(ParameterHolder *self, ToolInput *i) {}
static void tool_zoom(ParameterHolder *self, ToolInput *i) {}
static void tool_rotate(ParameterHolder *self, ToolInput *i) {}
static void tool_exposure_gamma(ParameterHolder *self, ToolInput *i) {}
static void tool_a_b(ParameterHolder *self, ToolInput *i) {}
static void tool_a_c(ParameterHolder *self, ToolInput *i) {}
static void tool_a_d(ParameterHolder *self, ToolInput *i) {}
static void tool_b_c(ParameterHolder *self, ToolInput *i) {}
static void tool_b_d(ParameterHolder *self, ToolInput *i) {}
static void tool_c_d(ParameterHolder *self, ToolInput *i) {}
static void tool_ab_cd(ParameterHolder *self, ToolInput *i) {}
static void tool_ac_bd(ParameterHolder *self, ToolInput *i) {}

/* The End */
