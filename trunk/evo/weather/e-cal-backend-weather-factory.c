/* Evolution calendar - weather backend factory
 *
 * Copyright (C) 2000 Ximian, Inc.
 * Copyright (C) 2003 David Trowbridge
 *
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pthread.h>
#include <string.h>

#include "e-cal-backend-weather-factory.h"
#include "e-cal-backend-weather.h"

typedef struct {
	ECalBackendFactory parent_object;
} ECalBackendWeatherFactory;

typedef struct {
	ECalBackendWeatherClass parent_class;
} ECalBackendWeatherFactoryClass;

static void
e_cal_backend_weather_factory_instance_init (ECalBackendWeatherFactory *factory)
{
}

static const char *
_get_protocol (ECalBackendFactory *factory)
{
	return "weather";
}

static ECalBackend*
_events_new_backend (ECalBackendFactory *factory, ESource *source)
{
	return g_object_new (e_cal_backend_weather_get_type (),
	                     "source", source,
			     "kind", ICAL_VEVENT_COMPONENT,
			     NULL);
}

static icalcomponent_kind
_events_get_kind (ECalBackendFactory *factory)
{
	return ICAL_VEVENT_COMPONENT;
}

static void
events_backend_factory_class_init (ECalBackendWeatherFactoryClass *klass)
{
	E_CAL_BACKEND_FACTORY_CLASS (klass)->get_protocol = _get_protocol;
	E_CAL_BACKEND_FACTORY_CLASS (klass)->get_kind     = _events_get_kind;
	E_CAL_BACKEND_FACTORY_CLASS (klass)->new_backend  = _events_new_backend;
}

static GType
events_backend_factory_get_type (GTypeModule *module)
{
	GType type;
	GTypeInfo info = {
		sizeof (ECalBackendWeatherFactoryClass),
		NULL, /* base_class_init */
		NULL, /* base_class_finalize */
		(GClassInitFunc) events_backend_factory_class_init,
		NULL, /* class_finalize */
		NULL, /* class_data */
		sizeof (ECalBackend),
		0, /* n_preallocs */
		(GInstanceInitFunc) e_cal_backend_weather_factory_instance_init
	};
	type = g_type_module_register_type (module,
	                                    E_TYPE_CAL_BACKEND_FACTORY,
					    "ECalBackendWeatherEventsFactory",
					    &info, 0);

	return type;
}

static GType weather_type;

void
eds_module_initialize (GTypeModule *module)
{
	weather_type = events_backend_factory_get_type (module);
}

void
eds_module_shutdown (void)
{
}

void
eds_module_list_types (const GType **types, int *num_types)
{
	*types = &weather_type;
	*num_types = 1;
}
