/*
 * field-sensor.h - An object that provides an interface to fieldsensor data
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

#ifndef __FIELD_SENSOR_H__
#define __FIELD_SENSOR_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define FIELD_SENSOR_TYPE            (field_sensor_get_type ())
#define FIELD_SENSOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FIELD_SENSOR_TYPE, FieldSensor))
#define FIELD_SENSOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FIELD_SENSOR_TYPE, FieldSensorClass))
#define IS_FIELD_SENSOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FIELD_SENSOR_TYPE))
#define IS_FIELD_SENSOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FIELD_SENSOR_TYPE))

typedef struct _FieldSensor      FieldSensor;
typedef struct _FieldSensorClass FieldSensorClass;
typedef struct _FieldSensorPrivate FieldSensorPrivate;

#define FS_TX_0 (1 << 0)
#define FS_TX_1 (1 << 1)
#define FS_TX_2 (1 << 2)
#define FS_TX_3 (1 << 3)
#define FS_RX_0 0x81
#define FS_RX_1 0x89

struct _FieldSensor {
	GObject parent;

	FieldSensorPrivate *priv;
};

struct _FieldSensorClass {
	GObjectClass parent;
};

GType        field_sensor_get_type ();
FieldSensor* field_sensor_new ();
void         field_sensor_set_result_index (FieldSensor *sensor, guint channel, guchar index);
void         field_sensor_set_lc_port_xor  (FieldSensor *sensor, guint channel, guchar value);
void         field_sensor_set_adcon_select (FieldSensor *sensor, guint channel, guchar reciever);
void         field_sensor_set_period       (FieldSensor *sensor, guint channel, guchar value);
void         field_sensor_set_phase        (FieldSensor *sensor, guint channel, guchar value);
void         field_sensor_set_half_periods (FieldSensor *sensor, guint channel, guchar value);
void         field_sensor_set_lc_tristate  (FieldSensor *sensor, guint channel, guchar transmitters);
void         field_sensor_set_lc_port      (FieldSensor *sensor, guint channel, guchar value);
void         field_sensor_take_reading     (FieldSensor *sensor, guchar buffer[8]);

G_END_DECLS

#endif /* __FIELD_SENSOR_H__ */
