/* Evolution calendar - weather backend source class
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

#ifndef E_WEATHER_SOURCE_H
#define E_WEATHER_SOURCE_H

#include <glib-object.h>
#include <time.h>

G_BEGIN_DECLS



typedef enum {
	WEATHER_FAIR,
	WEATHER_SNOW_SHOWERS,
	WEATHER_SNOW,
	WEATHER_PARTLY_CLOUDY,
	WEATHER_SMOKE,
	WEATHER_THUNDERSTORMS,
	WEATHER_CLOUDY,
	WEATHER_DRIZZLE,
	WEATHER_SUNNY,
	WEATHER_DUST,
	WEATHER_CLEAR,
	WEATHER_MOSTLY_CLOUDY,
	WEATHER_WINDY,
	WEATHER_RAIN_SHOWERS,
	WEATHER_FOGGY,
	WEATHER_RAIN_OR_SNOW_MIXED,
	WEATHER_SLEET,
	WEATHER_VERY_HOT_OR_HOT_HUMID,
	WEATHER_BLIZZARD,
	WEATHER_FREEZING_RAIN,
	WEATHER_HAZE,
	WEATHER_BLOWING_SNOW,
	WEATHER_FREEZING_DRIZZLE,
	WEATHER_VERY_COLD_WIND_CHILL,
	WEATHER_RAIN,
} WeatherConditions;

typedef struct {
	/* date, in UTC */
	time_t curtime;
	/* expected conditions */
	WeatherConditions conditions;
	/* internal storage is always in celcius and should be
	 * converted based on the user's current locale setting */
	float high, low;
	/* probability of precipitation */
	int pop;
} WeatherForecast;

#define E_TYPE_WEATHER_SOURCE            (e_weather_source_get_type ())
#define E_WEATHER_SOURCE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), E_TYPE_WEATHER_SOURCE, EWeatherSource))
#define E_WEATHER_SOURCE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), E_TYPE_WEATHER_SOURCE, EWeatherSource))
#define E_IS_WEATHER_SOURCE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), E_TYPE_WEATHER_SOURCE))
#define E_IS_WEATHER_SOURCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), E_TYPE_WEATHER_SOURCE))

typedef struct _EWeatherSource EWeatherSource;
typedef struct _EWeatherSourceClass EWeatherSourceClass;

/* This class is an abstract base-class for any weather data source.
 * All the URL fetching is handled outside of this, and all this has
 * to know how to do is parse the specific format. */
struct _EWeatherSource {
	GObject object;
};

struct _EWeatherSourceClass {
	GObjectClass parent_class;

	/* Overrideable methods */

	/* Returns a list of WeatherForecast objects containing the
	 * data for the forecast. */
	GList* (*parse)	(EWeatherSource *source, const char *buffer);
};

GType	e_weather_source_get_type (void);
GList*	e_weather_source_parse (EWeatherSource *source, const char *buffer);



G_END_DECLS

#endif
