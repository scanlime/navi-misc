/*
 * mean-filtered-sensor.h - A subclass of FieldSensor which filters
 *                          the input signal using a sliding window
 *                          mean.
 *
 * Copyright (C) 2004 David Trowbridge
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

#ifndef __MEAN_FILTERED_SENSOR_H__
#define __MEAN_FILTERED_SENSOR_H__

#include "field-sensor.h"

G_BEGIN_DECLS

#define MEAN_FILTERED_SENSOR_TYPE            (mean_filtered_sensor_get_type ())
#define MEAN_FILTERED_SENSOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEAN_FILTERED_SENSOR_TYPE, MeanFilteredSensor))
#define MEAN_FILTERED_SENSOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MEAN_FILTERED_SENSOR_TYPE, MeanFilteredSensorClass))
#define IS_MEAN_FILTERED_SENSOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEAN_FILTERED_SENSOR_TYPE))
#define IS_MEAN_FILTERED_SENSOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MEAN_FILTERED_SENSOR_TYPE))

typedef struct _MeanFilteredSensor        MeanFilteredSensor;
typedef struct _MeanFilteredSensorClass   MeanFilteredSensorClass;
typedef struct _MeanFilteredSensorPrivate MeanFilteredSensorPrivate;

GType               mean_filtered_sensor_get_type ();
MeanFilteredSensor* mean_filtered_sensor_new ();
void                mean_filtered_sensor_get_data (MeanFilteredSensor *sensor, guchar channels[8]);

G_END_DECLS

#endif /* __MEAN_FILTERED_SENSOR_H__ */
