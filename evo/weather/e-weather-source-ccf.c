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
	/* Our URI is formatted as weather://ccf/AAA[/BBB] - AAA is the 3-letter station
	 * code for identifying the providing station (subdirectory within the crh data
	 * repository). BBB is an optional additional station ID for the station within
	 * the CCF file. If not present, BBB is assumed to be the same station as AAA.
	 */
	EWeatherSourceCCF *source = E_WEATHER_SOURCE_CCF (g_object_new (e_weather_source_ccf_get_type (), NULL));
	char *station;

	station = g_strdup (strchr (uri, '/') + 1);
	if (strchr (station, '/'))
	{
		/* we've got the extended format station code */
		source->substation = g_strdup (strchr (station, '/') + 1);
		source->station = g_strndup (station, 3);
		g_free (station);
	}
	else
	{
		source->station = station;
		source->substation = g_strdup (station);
	}
	return E_WEATHER_SOURCE (source);
}

static GSList*
tokenize (char *buffer)
{
	char *token;
	char *tokbuf = g_strdup (buffer);
	char *buffer2 = tokbuf;
	GSList *ret;

	token = strtok_r (buffer, " \n", &tokbuf);
	ret = g_slist_append (NULL, g_strdup (token));
	while ((token = strtok_r (NULL, " \n/", &tokbuf)))
		ret = g_slist_append (ret, g_strdup (token));
	g_free (buffer2);
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
		case '-':
			ret = 5;
			break;
		case '+':
			ret = 95;
			break;
		case '/':
			ret = -1;	/* missing data */
			break;
		default:
			ret = (data - '0') * 10;
	}
	return ret;
}

static void
decodeSnowfall (char *data, float *low, float *high)
{
	char num[3];
	num[2] = '\0';

	num[0] = data[0]; num[1] = data[1];
	*low = atof (num);
	num[0] = data[2]; num[1] = data[3];
	*high = atof (num);
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
e_weather_source_ccf_do_parse (EWeatherSourceCCF *source, char *buffer)
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
	GSList *tokens = tokenize (buffer);
	GSList *date;
	GSList *current = tokens;
	GList *fc = NULL;
	struct tm tms;
	int i;
	time_t base;

	date = g_slist_nth (tokens, 3);
	date2tm (date->data, &tms);
	g_print ("date is %s\n", asctime (&tms));

	/* fast-forward to the particular station we're interested in */
	current = g_slist_nth (tokens, 5);
	while (strcmp(current->data, source->substation))
		current = g_slist_next (current);
	current = g_slist_next (current);
	/* pick up the first two conditions reports */
	forecasts[0].conditions = decodeConditions (((char*)(current->data))[0]);
	forecasts[1].conditions = decodeConditions (((char*)(current->data))[1]);

	current = g_slist_next (current);
	if (tms.tm_hour < 12)
	{
		forecasts[0].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[0].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[1].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[1].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[2].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[0].pop = decodePOP (((char*)(current->data))[2]);
		forecasts[1].pop = decodePOP (((char*)(current->data))[4]);
	}
	else
	{
		current = g_slist_next (current);
		forecasts[0].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[0].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[1].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[1].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[0].pop = decodePOP (((char*)(current->data))[3]);
	}

	current = g_slist_next (current);
	if (strlen (current->data) == 4)
	{
		/* we've got the optional snowfall field */
		if (tms.tm_hour < 12)
		{
			decodeSnowfall (current->data, &forecasts[0].low, &forecasts[0].high);
			current = g_slist_next (g_slist_next (current));
			decodeSnowfall (current->data, &forecasts[1].low, &forecasts[1].high);
		}
		else
		{
			current = g_slist_next (current);
			decodeSnowfall (current->data, &forecasts[0].low, &forecasts[0].high);
		}
		current = g_slist_next (current);
	}

	/* set dates */
	base = mktime (&tms);
	if (tms.tm_hour >= 12)
		base += 43200;
	for (i = 0; i < 7; i++)
		forecasts[i].date = base + 86400*i;

	if (current == NULL || strlen (current->data) == 3)
	{
		/* We've got a pre-IFPS station. Realloc and return */
		WeatherForecast *f = g_new0(WeatherForecast, 2);
		memcpy (f, forecasts, sizeof (WeatherForecast) * 2);
		g_free (forecasts);
		fc = g_list_append (fc, &f[0]);
		fc = g_list_append (fc, &f[1]);
		source->done (fc, source->finished_data);
	}

	/* Grab the conditions for the next 5 days */
	forecasts[2].conditions = decodeConditions (((char*)(current->data))[0]);
	forecasts[3].conditions = decodeConditions (((char*)(current->data))[1]);
	forecasts[4].conditions = decodeConditions (((char*)(current->data))[2]);
	forecasts[5].conditions = decodeConditions (((char*)(current->data))[3]);
	forecasts[6].conditions = decodeConditions (((char*)(current->data))[4]);

	/* Temperature forecasts */
	current = g_slist_next (current);
	if (tms.tm_hour < 12)
	{
		forecasts[2].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[3].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[3].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[4].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[4].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[5].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[5].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[6].high = ftoc (current->data);
		forecasts[6].low  = forecasts[6].high;
		current = g_slist_next (current);
		forecasts[2].pop = decodePOP (((char*)(current->data))[1]);
		forecasts[3].pop = decodePOP (((char*)(current->data))[3]);
		forecasts[4].pop = decodePOP (((char*)(current->data))[5]);
		forecasts[5].pop = decodePOP (((char*)(current->data))[7]);
		forecasts[6].pop = decodePOP (((char*)(current->data))[9]);
	}
	else
	{
		forecasts[2].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[2].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[3].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[3].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[4].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[4].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[5].high = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[5].low  = ftoc (current->data);
		current = g_slist_next (current);
		forecasts[6].high = ftoc (current->data);
		forecasts[6].low  = forecasts[6].high;
		current = g_slist_next (current);
		forecasts[1].pop = decodePOP (((char*)(current->data))[0]);
		forecasts[2].pop = decodePOP (((char*)(current->data))[2]);
		forecasts[3].pop = decodePOP (((char*)(current->data))[4]);
		forecasts[4].pop = decodePOP (((char*)(current->data))[6]);
		forecasts[5].pop = decodePOP (((char*)(current->data))[8]);
		forecasts[6].pop = decodePOP (((char*)(current->data))[10]);
	}

	for (i = 0; i < 7; i++)
	{
		fc = g_list_append (fc, &forecasts[i]);
	}
	source->done (fc, source->finished_data);
}

static void
retrieval_done (SoupMessage *message, EWeatherSourceCCF *source)
{
	char *str;
	const char *newuri;

	g_print ("Retrieval done.\n");

	/* Handle redirection ourselves */
	if (SOUP_STATUS_IS_REDIRECTION (message->status_code))
	{
		newuri = soup_message_get_header (message->response_headers, "Location");

		g_print ("Redirected to %s\n", newuri);

		if (newuri)
		{
			SoupMessage *soup_message;
			soup_message = soup_message_new (SOUP_METHOD_GET, newuri);
			soup_message_set_flags (soup_message, SOUP_MESSAGE_NO_REDIRECT);
			soup_session_queue_message (source->soup_session, soup_message, (SoupMessageCallbackFn) retrieval_done, source);
			return;
		}
		else
		{
			source->done (NULL, source->finished_data);
		}

		return;
	}

	/* check status code */
	if (!SOUP_STATUS_IS_SUCCESSFUL (message->status_code))
	{
		source->done (NULL, source->finished_data);
		return;
	}

	str = g_malloc0 (message->response.length + 1);
	strncpy (str, message->response.body, message->response.length);
	e_weather_source_ccf_do_parse (source, str);
	g_free (str);
}

static void
e_weather_source_ccf_parse (EWeatherSource *source, EWeatherSourceFinished done, gpointer data)
{
	EWeatherSourceCCF *ccfsource = (EWeatherSourceCCF*) source;
	SoupMessage *soup_message;
	char *url;

	ccfsource->finished_data = data;

	g_print ("e_weather_source_ccf_parse ()\n");
	url = g_strdup_printf ("http://www.crh.noaa.gov/data/%s/CCF%s", ccfsource->station, ccfsource->station);
	g_print ("    trying to open %s\n", url);
	ccfsource->done = done;

	if (!ccfsource->soup_session)
		ccfsource->soup_session = soup_session_async_new ();
	soup_message = soup_message_new (SOUP_METHOD_GET, url);
	soup_message_set_flags (soup_message, SOUP_MESSAGE_NO_REDIRECT);
	soup_session_queue_message (ccfsource->soup_session, soup_message, (SoupMessageCallbackFn) retrieval_done, source);
	g_print ("Retrieval started.\n");
	g_free (url);
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
	source->station = NULL;
	source->substation = NULL;
	source->soup_session = NULL;
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
