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
#include <libgnome/gnome-i18n.h>
#include "e-weather-source-ccf.h"

#define DATA_SIZE 5000

EWeatherSource*
e_weather_source_ccf_new (const char *uri)
{
	/* uri is formatted as weather:ccf/SSS where SSS is the 3-letter NWS
	 * station identifier. Pretty much all we care about here is the station */
	EWeatherSourceCCF *source = E_WEATHER_SOURCE_CCF (g_object_new (e_weather_source_ccf_get_type (), NULL));

	source->station = g_strdup (strchr(uri, '/') + 1);
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
	while ((token = strtok_r (NULL, " \n/", &tokbuf)))
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

static int
decodePOP (char data)
{
	int ret;
	switch (data)
	{
		case '-': ret = 5;
		case '+': ret = 95;
		case '/': ret = -1;	/* missing data */
		default: ret = (data - '0') * 10;
	}
	return ret;
}

static void
decodeSnowfall (char *data, int *low, int *high)
{
	char num[3];
	num[2] = '\0';

	num[0] = data[0]; num[1] = data[1];
	*low = atoi (num);
	num[0] = data[2]; num[1] = data[3];
	*high = atoi (num);
}

static float
ftoc (char *data)
{
	int fahrenheit = atoi(data);
	if (fahrenheit >= 900)
		fahrenheit = (fahrenheit - 900) * -1;
	return ((float)(fahrenheit-32)) * 5.0f / 9.0f;
}

static void
e_weather_source_ccf_do_parse (EWeatherSourceCCF *source)
{
	/* CCF gives us either 2 or 7 days of forecast data. IFPS WFO's
	 * will produce 7 day forecasts, whereas pre-IFPS WFO's are only
	 * mandated 2 (but may do 7). The morning forecast will give us either 2
	 * or 7 days worth of data. The evening forecast will give us the evening's
	 * low temperature plus 2 or 7 days forecast.
	 *
	 * The CCF format is described in NWS directive 10-503, but it's usually
	 * easier to look at a summary put up by one of the stations:
	 * http://www.crh.noaa.gov/lmk/product_guide/products/forecast/ccf.htm
	 */
	WeatherForecast *forecasts = g_new0 (WeatherForecast, 7);
	GSList *tokens = tokenize (source->buffer);
	GSList *date;
	GSList *current = tokens;
	GList *fc = NULL;
	struct tm tms;
	int i;

	date = g_slist_nth (tokens, 3);
	date2tm (date->data, &tms);
	g_print ("date is %s\n", asctime (&tms));

	/* fast-forward to the particular station we're interested in */
	current = g_slist_nth (tokens, 5);
	while (strcmp(current->data, source->station))
		current = g_slist_next (current);

	for (i = 0; i < 7; i++)
		fc = g_list_append (fc, &forecasts[i]);
}

static void
e_weather_source_ccf_finish_read (GnomeVFSAsyncHandle *handle, GnomeVFSResult result, gpointer buffer,
                                  GnomeVFSFileSize requested, GnomeVFSFileSize body_len, gpointer data)
{
	EWeatherSourceCCF *source = (EWeatherSourceCCF*) data;
	char *body;

	g_return_if_fail (source != NULL);
	g_return_if_fail (handle == source->handle);

	body = (char *) buffer;
	body[body_len] = '\0';

	if (source->buffer == NULL)
		source->buffer = g_strdup (body);
	else
	{
		char *temp = g_strdup (source->buffer);
		g_free (source->buffer);
		source->buffer = g_strdup_printf ("%s%s", temp, body);
		g_free (temp);
	}

	if (result == GNOME_VFS_ERROR_EOF)
	{
		e_weather_source_ccf_do_parse (source);
	}
	else if (result != GNOME_VFS_OK)
	{
		g_print ("%s", gnome_vfs_result_to_string (result));
		g_warning (_("Failed to get CCF data.\n"));
	}
	else
	{
		gnome_vfs_async_read (handle, body, DATA_SIZE - 1, e_weather_source_ccf_finish_read, data);
		return;
	}
}

static void
e_weather_source_ccf_finish_open (GnomeVFSAsyncHandle *handle, GnomeVFSResult result, gpointer data)
{
	EWeatherSourceCCF *source = (EWeatherSourceCCF*) data;
	char *body;

	g_return_if_fail (source != NULL);
	g_return_if_fail (handle == source->handle);

	body = g_malloc0 (DATA_SIZE);
	if (source->buffer)
		g_free (source->buffer);
	source->buffer = NULL;

	if (result != GNOME_VFS_OK)
	{
		g_warning(_("Failed to get CCF data.\n"));
		source->handle = NULL;
		g_free (body);
	}
	else
	{
		gnome_vfs_async_read (handle, body, DATA_SIZE - 1, e_weather_source_ccf_finish_read, data);
	}
	return;
}

static void
e_weather_source_ccf_parse (EWeatherSource *source, SourceFinished done)
{
	EWeatherSourceCCF *ccfsource = (EWeatherSourceCCF*) source;
	char *url;

	url = g_strdup_printf ("http://www.crh.noaa.gov/data/%s/CCF%s", ccfsource->station, ccfsource->station);
	ccfsource->done = done;
	gnome_vfs_async_open (&ccfsource->handle, url, GNOME_VFS_OPEN_READ, 0, e_weather_source_ccf_finish_open, source);
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
	source->handle = NULL;
	source->buffer = NULL;
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
