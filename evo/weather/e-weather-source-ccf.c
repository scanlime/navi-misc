/* Evolution calendar - weather backend source class for parsing
 * 	CCF (coded cities forecast) formatted NWS reports
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

#include <string.h>
#include <stdlib.h>
#include "e-weather-source-ccf.h"

EWeatherSource*
e_weather_source_ccf_new (const char *station)
{
	EWeatherSourceCCF *source = E_WEATHER_SOURCE_CCF (g_object_new (e_weather_source_ccf_get_type (), NULL));

	source->station = g_strdup (station);
	return E_WEATHER_SOURCE (source);
}

static GSList*
tokenize (const char *buffer)
{
	char *token;
	char *buffer2 = g_strdup (buffer);
	char *tokbuf = g_strdup (buffer);
	GSList *ret;

	token = strtok_r (buffer2, " \n", &tokbuf);
	ret = g_slist_append (NULL, g_strdup (token));
	while ((token = strtok_r (NULL, " \n", &tokbuf)))
		ret = g_slist_append (ret, g_strdup (token));
	g_free (buffer2);
	g_free (tokbuf);
	return ret;
}

static void
date2tm (char *date, struct tm *times)
{
	char tmp[3];
	time_t curtime = time(NULL);
	tmp[2] = '\0';

	localtime_r (&curtime, times);

	tmp[0] = date[0]; tmp[1] = date[1];
	times->tm_mday = atoi(tmp);
	tmp[0] = date[2]; tmp[1] = date[3];
	times->tm_hour = atoi(tmp);
	tmp[0] = date[4]; tmp[1] = date[5];
	times->tm_min = atoi(tmp);
}

static WeatherConditions
decodeConditions (char code)
{
	switch (code)
	{
		case 'A': return WEATHER_FAIR;
		case 'B': return WEATHER_PARTLY_CLOUDY;
		case 'C': return WEATHER_CLOUDY;
		case 'D': return WEATHER_DUST;
		case 'E': return WEATHER_MOSTLY_CLOUDY;
		case 'F': return WEATHER_FOGGY;
		case 'G': return WEATHER_VERY_HOT_OR_HOT_HUMID;
		case 'H': return WEATHER_HAZE;
		case 'I': return WEATHER_VERY_COLD_WIND_CHILL;
		case 'J': return WEATHER_SNOW_SHOWERS;
		case 'K': return WEATHER_SMOKE;
		case 'L': return WEATHER_DRIZZLE;
		case 'M': return WEATHER_SNOW_SHOWERS;
		case 'N': return WEATHER_WINDY;
		case 'O': return WEATHER_RAIN_OR_SNOW_MIXED;
		case 'P': return WEATHER_BLIZZARD;
		case 'Q': return WEATHER_BLOWING_SNOW;
		case 'R': return WEATHER_RAIN;
		case 'S': return WEATHER_SNOW;
		case 'T': return WEATHER_THUNDERSTORMS;
		case 'U': return WEATHER_SUNNY;
		case 'V': return WEATHER_CLEAR;
		case 'W': return WEATHER_RAIN_SHOWERS;
		case 'X': return WEATHER_SLEET;
		case 'Y': return WEATHER_FREEZING_RAIN;
		case 'Z': return WEATHER_FREEZING_DRIZZLE;
		/* hmm, this should never happen. */
		default: return WEATHER_SUNNY;
	}
}

static GList*
e_weather_source_ccf_parse (EWeatherSource *source, const char *buffer)
{
	/* CCF gives us either 7 or 8 days into the future, depending on whether
	 * we've fetched the morning or afternoon forecast. There are actually
	 * 14 data points, but we'll just have to "average" them since there's
	 * no real way to create a "nighttime" event. In general, we can just
	 * display the daytime precipitation, high temperature and conditions
	 * and the nighttime low temperature, and the current day's weather will
	 * update to be the nighttime version when it fetches the afternoon forecast.
	 *
	 * Supposedly, the CCF format is described in NOAA document 10-503, but I've
	 * been unable to find a copy of this online. The real meat of the format
	 * is summarized pretty well at
	 * http://www.crh.noaa.gov/lmk/product_guide/products/forecast/ccf.htm
	 *
	 * There are a few fields left out of the summary which are located at the
	 * top of the file. These are:
	 *
	 * 000
	 * FPUSxx KSSS DDHHMM
	 * CCFSSS
	 *
	 * The first line is zeroed in all the files I've seen. FPUSxx is some
	 * identifier, but without real documentation it will remain forever a
	 * mystery to us non-NWS folk. KSSS is the location code for the station,
	 * and CCFSSS indicates that the file is a CCF product put out by station
	 * SSS. DDHHMM is a date indicator showing when the product was released,
	 * with day-of-month, hour and minute in UTC.
	 *
	 * Note that the station in this header will usually be one of the stations
	 * represented in the file, but this is not always the case.
	 */
	EWeatherSourceCCF *ccfsource = (EWeatherSourceCCF*) source;
	WeatherForecast *forecasts = g_new0 (WeatherForecast, 7);
	GSList *tokens = tokenize (buffer);
	GSList *date;
	GSList *current = tokens;
	struct tm tms;

	date = g_slist_nth (tokens, 3);
	date2tm (date->data, &tms);
	g_print ("date is %s\n", asctime (&tms));

	/* fast-forward to the particular station we're interested in */
	current = g_slist_nth (tokens, 5);
	while (strcmp(current->data, ccfsource->station))
		current = g_slist_next (current);

	current = g_slist_next (current);
	forecasts[0].conditions = decodeConditions(((char *)(current->data))[0]);
	forecasts[1].conditions = decodeConditions(((char *)(current->data))[1]);

	return NULL;
}

static void
e_weather_source_ccf_class_init (EWeatherSourceCCFClass *class)
{
	EWeatherSourceClass *source_class;

	source_class = (EWeatherSourceClass *) class;

	source_class->parse = e_weather_source_ccf_parse;
}

static void
e_weather_source_ccf_init (EWeatherSourceCCF *source)
{
	/* nothing to do here */
}

GType
e_weather_source_ccf_get_type (void)
{
	static GType e_weather_source_ccf_type = 0;

	if (!e_weather_source_ccf_type)
	{
		static GTypeInfo info = {
			sizeof (EWeatherSourceCCFClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) e_weather_source_ccf_class_init,
			NULL, NULL,
			sizeof (EWeatherSourceCCF),
			0,
			(GInstanceInitFunc) e_weather_source_ccf_init
		};
		e_weather_source_ccf_type = g_type_register_static (E_TYPE_WEATHER_SOURCE, "EWeatherSourceCCF", &info, 0);
	}

	return e_weather_source_ccf_type;
}
