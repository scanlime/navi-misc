/*
 * field-sensor.c - An object that provides an interface to fieldsensor data
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

#include "field-sensor.h"
#include "../include/efs_protocol.h"
#include <usb.h>

static void field_sensor_class_init (FieldSensorClass *klass);
static void field_sensor_init       (FieldSensor *self);
static void field_sensor_finalize   (GObject *obj);

struct _FieldSensorPrivate {
	usb_dev_handle *dev;
};

GType
field_sensor_get_type ()
{
	static GType fs_type = 0;
	if (!fs_type) {
		static const GTypeInfo fs_info = {
			sizeof (FieldSensorClass),
			NULL, NULL,
			(GClassInitFunc) field_sensor_class_init,
			NULL, NULL,
			sizeof (FieldSensor),
			0,
			(GInstanceInitFunc) field_sensor_init,
		};

		fs_type = g_type_register_static (G_TYPE_OBJECT, "FieldSensor", &fs_info, 0);

		usb_init ();
	}

	return fs_type;
}

FieldSensor*
field_sensor_new ()
{
	return FIELD_SENSOR (g_object_new (field_sensor_get_type (), NULL));
}

static void
field_sensor_class_init (FieldSensorClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = field_sensor_finalize;
}

static void
field_sensor_init (FieldSensor *self)
{
	FieldSensorPrivate *priv;
	struct usb_bus *busses;
	struct usb_bus *bus;
	struct usb_device *dev;

	priv = g_new0 (FieldSensorPrivate, 1);

	usb_find_busses ();
	usb_find_devices ();
	busses = usb_get_busses ();

	for (bus = busses; bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == EFS_VENDOR_ID &&
			    dev->descriptor.idProduct == EFS_PRODUCT_ID) {
				priv->dev = usb_open (dev);
			}
		}
	}
	g_assert (priv->dev != NULL);

	self->priv = priv;
}

static void
field_sensor_finalize (GObject *obj)
{
	FieldSensor *self = FIELD_SENSOR (obj);
	if (self->priv)
		g_free (self->priv);
	self->priv = NULL;
}

static void
set_param_byte (usb_dev_handle *d, guint address, guint value)
{
	if (usb_control_msg (d, USB_TYPE_VENDOR, EFS_CTRL_SET_PARAM_BYTE, value, address, NULL, 0, 500) < 0)
		perror ("usb_control_msg");
}

void
field_sensor_set_result_index (FieldSensor *sensor, guint channel, guchar index)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_RESULT_NUM, index);
}

void
field_sensor_set_lc_port_xor (FieldSensor *sensor, guint channel, guchar value)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_LC_PORT_XOR, value);
}

void
field_sensor_set_adcon_select (FieldSensor *sensor, guint channel, guchar reciever)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_ADCON_INIT, reciever);
}

void
field_sensor_set_period (FieldSensor *sensor, guint channel, guchar value)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_PERIOD, 255 - value);
}

void
field_sensor_set_phase (FieldSensor *sensor, guint channel, guchar value)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_PHASE, 255 - value);
}

void
field_sensor_set_half_periods (FieldSensor *sensor, guint channel, guchar value)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_NUM_HALF_PERIODS, value);
}

void
field_sensor_set_lc_tristate (FieldSensor *sensor, guint channel, guchar transmitters)
{
	guchar value = 0xff;
	if (transmitters & FS_TX_0)
		value &= 0xfc;
	if (transmitters & FS_TX_1)
		value &= 0xf3;
	if (transmitters & FS_TX_2)
		value &= 0xcf;
	if (transmitters & FS_TX_3)
		value &= 0x3f;
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_LC_TRIS_INIT, value);
}

void
field_sensor_set_lc_port (FieldSensor *sensor, guint channel, guchar value)
{
	set_param_byte (sensor->priv->dev, 8 * channel + EFS_PARAM_LC_PORT_INIT, value);
}

void
field_sensor_take_reading (FieldSensor *sensor, guchar buffer[8])
{
	if (usb_control_msg (sensor->priv->dev, USB_TYPE_VENDOR | USB_ENDPOINT_IN, EFS_CTRL_READ_SENSORS, 0, 0, buffer, 8, 500) < 0)
		perror ("usb_control_msg");
}
