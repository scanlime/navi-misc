/*
 * parameter-holder.h - A base class for GObjects whose properties include
 *                      algorithm parameters that can be serialized to key/value
 *                      pairs and interpolated between.
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

#ifndef __PARAMETER_HOLDER_H__
#define __PARAMETER_HOLDER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PARAMETER_HOLDER_TYPE            (parameter_holder_get_type ())
#define PARAMETER_HOLDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PARAMETER_HOLDER_TYPE, ParameterHolder))
#define PARAMETER_HOLDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PARAMETER_HOLDER_TYPE, ParameterHolderClass))
#define IS_PARAMETER_HOLDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PARAMETER_HOLDER_TYPE))
#define IS_PARAMETER_HOLDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PARAMETER_HOLDER_TYPE))

typedef struct _ParameterHolder      ParameterHolder;
typedef struct _ParameterHolderClass ParameterHolderClass;

struct _ParameterHolder {
  GObject object;
};

struct _ParameterHolderClass {
  GObjectClass parent_class;
};

typedef void (ParameterInterpolator)(ParameterHolder  *self,
				     double   alpha,
				     gpointer user_data);

#define PARAMETER_INTERPOLATOR(x)   ((ParameterInterpolator*)(x))

typedef struct {
  ParameterHolder *a, *b;
} ParameterHolderPair;


/* Custom G_PARAM flags */
#define G_PARAM_SERIALIZED   (1 << (G_PARAM_USER_SHIFT + 0))    /* Parameters we're interested in serializing */
#define G_PARAM_INTERPOLATE  (1 << (G_PARAM_USER_SHIFT + 1))    /* Parameters we're interested in interpolating */


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType             parameter_holder_get_type           ();
ParameterHolder*  parameter_holder_new                ();

void              parameter_holder_reset_to_defaults  (ParameterHolder *self);

void              parameter_holder_set                (ParameterHolder *self,
						       const gchar     *property,
						       const gchar     *value);

void              parameter_holder_load_string        (ParameterHolder *self,
						       const gchar     *params);

gchar*            parameter_holder_save_string        (ParameterHolder *self);

void              parameter_holder_interpolate_linear (ParameterHolder     *self,
						       gdouble              alpha,
						       ParameterHolderPair *p);

G_END_DECLS

#endif /* __PARAMETER_HOLDER_H__ */

/* The End */
