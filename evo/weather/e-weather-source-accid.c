/* Evolution calendar - weather backend source class for parsing
 * 	ACCID-based data from msnbc.com (a-la mythweather)
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
 *
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
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

#include <string.h>
#include <stdlib.h>
#include <libgnome/gnome-i18n.h>
#include "e-weather-source-ccf.h"

#define DATA_SIZE 5000

EWeatherSource*
e_weather_source_accid_new (const char *uri)
{
	/* Our URI is weather://accid/AAAAnnnn/City, where AAAAnnnn is the ACCID code */
	EWeatherSourceACCID *source = E_WEATHER_SOURCE_ACCID (g_object_new (e_weather_source_accid_get_type (), NULL));
}

static float
ftoc (char *data)
{
	int fahrenheit = atoi (data);
	return ((float)(fahrenheit-32)) * 5.0f / 9.0f;
}

static time_t
decodeDate (char *data)
{
	struct tm tms;
	strptime (data, "%m/%d/%Y", &tms);
	return mktime (&tms);
}

static WeatherConditions
decodeConditions (char *data)
{
	int type = atoi (data);

	switch (type) {
	case   1: /* cloudy */
	case  33: /* cloudy / wind */
		return WEATHER_CLOUDY;
	case   3: /* mostly cloudy */
	case  35: /* mostly cloudy / wind */
		return WEATHER_MOSTLY_CLOUDY;
	case   4: /* partly cloudy */
	case  26: /* AM clouds / PM sun */
	case  37: /* partly cloudy / windy */
	case 126: /* AM clouds / PM sun / wind */
	case 988: /* partly cloudy / windy */
	case 998: /* a few clouds */
		return WEATHER_PARTLY_CLOUDY;
	case  13: /* light rain */
	case  14: /* showers */
	case  19: /* AM showers */
	case  21: /* few showers */
	case  29: /* PM showers */
	case  30: /* PM showers / wind */
	case  40: /* light rain / wind */
	case  41: /* showers / wind */
	case  43: /* t-showers */
	case  59: /* AM t-showers */
	case  67: /* AM showers / wind */
	case  80: /* AM light rain */
	case  81: /* PM light rain */
	case  88: /* few showers / wind */
	case 123: /* sprinkles */
	case 164: /* PM light rain */
	case 201: /* AM light rain / wind */
	case 204: /* AM rain / wind */
	case 231: /* rain showers */
	case 987: /* showers in the vicinity */
	case 989: /* light rain shower */
	case 990: /* light rain with thunder */
	case 991: /* light drizzle */
		return WEATHER_RAIN_SHOWERS;
	case  18: /* rain */
	case  47: /* rain / wind */
	case  62: /* AM rain */
	case  82: /* PM rain */
	case 137: /* heavy rain */
	case 154: /* heavy rain / wind */
		return WEATHER_RAIN;
	case  20: /* fog */
		return WEATHER_FOGGY;
	case  24: /* sunny */
	case  55: /* sunny / windy */
	case  22: /* mostly sunny */
	case  44: /* mostly sunny / wind */
		return WEATHER_SUNNY;
	case  27: /* isolated t-storms */
	case  28: /* scattered t-storms */
	case  36: /* rain / thunder */
	case  50: /* scattered strong storms */
	case  51: /* PM t-storms */
	case  53: /* t-storms */
	case  56: /* AM t-storms */
	case  64: /* isolated t-storms / wind */
	case  66: /* scattered t-storms / wind */
	case 118: /* t-storms / wind */
	case 173: /* strong storms */
	case 189: /* strong storms / windy */
	case 986: /* heavy t-storms */
		return WEATHER_THUNDERSTORMS;
	case  31: /* rain / snow showers */
	case  38: /* AM rain / snow showers */
	case  65: /* rain / snow */
	case  77: /* snow to rain */
	case  85: /* rain to snow */
	case  86: /* PM rain/snow */
	case  91: /* PM rain/snow showers */
	case  92: /* PM rain/snow/wind */
	case  93: /* rain/snow showers/wind */
	case  94: /* rain/snow/wind */
	case 104: /* wintry mix */
	case 105: /* AM wintry mix */
	case 109: /* PM rain/snow/wind */
	case 130: /* rain to snow / wind */
	case 132: /* snow to wintry mix */
	case 135: /* snow and ice to rain */
	case 152: /* AM light wintry mix */
	case 158: /* snow to rain / wind */
	case 172: /* wintry mix / wind */
	case 223: /* wintry mix to snow */
		return WEATHER_RAIN_OR_SNOW_MIXED;
	case 993: /* smoke */
		return WEATHER_SMOKE;
	case 994: /* haze */
		return WEATHER_HAZE;
	case 997: /* clear */
		return WEATHER_CLEAR;
	case 999: /* fair */
		return WEATHER_FAIR;
	case 106: /* heavy rain / freezing rain */
	case 114: /* rain / freezing rain */
	case 128: /* AM rain / snow / wind */
	case 138: /* AM rain / ice */
	case 259: /* heavy rain / freezing rain */
		return WEATHER_FREEZING_RAIN;
	case 178: /* AM drizzle */
	case 193: /* PM drizzle */
	case 194: /* drizzle */
		return WEATHER_DRIZZLE;
	case  25: /* scattered flurries */
	case  32: /* few snow showers */
	case  34: /* flurries / wind */
	case  45: /* flurries */
	case  49: /* scattered flurries / wind */
	case  70: /* scattered snow showers */
	case  84: /* snow showers */
	case  98: /* light snow */
	case 101: /* few snow showers */
	case 103: /* light snow/wind */
	case 108: /* AM light snow */
	case 125: /* AM snow showers */
	case 133: /* PM snow showers / wind */
	case 145: /* AM snow showers / wind */
	case 146: /* AM light snow / wind */
	case 150: /* PM light rain / wind */
	case 153: /* PM light snow / wind */
	case 155: /* PM snow shower */
	case 175: /* PM light snow */
	case 271: /* snow showers / windy */
	case 995: /* light snow shower */
	case 996: /* light snow shower / windy */
		return WEATHER_SNOW_SHOWERS;
	case  71: /* snow to ice / wind */
	case  90: /* snow / wind */
	case 100: /* PM snow */
	case 167: /* AM snow */
	case 171: /* snow to ice */
		return WEATHER_SNOW;
	case 992: /* mist */
	}
}

static void
e_weather_source_accid_do_parse (EWeatherSourceACCID *source, char *buffer)
{
	/* This is kind of hackish. The msnbc page gives us vbscript,
	 * from which we extract the useful information */
	char *base;
	char **tokens;
	WeatherForecast *forecasts = g_new0 (WeatherForecast, 5);
	GList *fc;

	base = strstr (buffer, "this.swFore");
	if (base == NULL) {
		source->done (NULL, source->finished_data);
		return;
	}

	base += 15;
	tokens = g_strsplit (base, '|', 45);

	for (i = 5; i < 10; i++)
		forecasts[i - 5].date = decodeDate (tokens[i]);
	for (i = 15; i < 20; i++)
		forecasts[i - 5].conditions = decodeConditions (tokens[i]);
	for (i = 20; i < 25; i++)
		forecasts[i - 20].high = ftoc (tokens[i]);
	for (i = 40; i < 45; i++)
		forecasts[i - 40].low = ftoc (tokens[i]);

	g_strfreev (tokens);

	for (i = 0; i < n; i++)
		fc = g_list_append (fc, &forecasts[i]);
	source->done (fc, source->finished_data);
	g_free (forecasts);
	g_list_free (fc);
}

static void
retrieval_done (SoupMessage *message, EWeatherSourceACCID *source)
{
	char *str;
	const char *newuri;

	/* Handle redirection ourselves */
	if (SOUP_STATUS_IS_REDIRECTION (message->status_code)) {
		newuri = soup_message_get_header (message->response_headers, "Location");

		if (newuri) {
			SoupMessage *soup_message = soup_message_new (SOUP_METHOD_GET, newuri);
			soup_message_set_flags (soup_message, SOUP_MESSAGE_NO_REDIRECT);
			soup_session_queue_message (source->soup_session, soup_message, (SoupMessageCallbackFn) retrieval_done, source);
			return;
		} else {
			source->done (NULL, source->finished_data);
		}
		return;
	}

	/* check status code */
	if (!SOUP_STATUS_IS_SUCCESSFUL (message->status_code)) {
		source->done (NULL, source->finished_data);
		return;
	}

	str = g_malloc0 (message->response_length + 1);
	strncpy (str, message->response.body, message->response.length);
	e_weather_source_accid_do_parse (source, str);
	g_free (str);
}

static void
e_weather_source_accid_parse (EWeatherSource *source, EWeatherSourceFinished done, gpointer data)
{
	EWeatherSourceACCID *accidsource = (EWeatherSourceACCID*) source;
	SoupMessage *soup_message;
	char *url;

	accidsource->finished_data = data;
	accidsource->done = done;

	if (!accidsource->soup_session)
		accidsource->soup_session = soup_session_async_new ();

	url = g_strdup_printf ("http://www.msnbc.com/m/chnk/d/weather_d_src.asp?acid=%s", accidsource->station);
	soup_message = soup_message_new (SOUP_METHOD_GET, url);
	soup_message_set_flags (soup_message, SOUP_MESSAGE_NO_REDIRECT);
	soup_session_queue_message (accidsource->soup_session, soup_message, (SoupMessageCallbackFn) retrieval_done, source);
	g_free (url);
}

static void
e_weather_source_accid_class_init (EWeatherSourceACCIDClass *klass)
{
	EWeatherSourceClass *source_class = (EWeatherSourceClass *) klass;
	source_class->parse = e_weather_source_accid_parse;
}

static void
e_weather_source_accid_init (EWeatherSourceACCID *source)
{
	source->station = NULL;
	source->soup_session = NULL;
}

GType
e_weather_source_accid_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static GTypeInfo info = {
			sizeof (EWeatherSourceACCIDClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) e_weather_source_accid_class_init,
			NULL, NULL,
			sizeof (EWeatherSourceACCID),
			0,
			(GInstanceInitFunc) e_weather_source_accid_init,
		};
		type = g_type_register_static (E_TYPE_WEATHER_SOURCE, "EWeatherSourceACCID", &info, 0);
	}

	return type;
}
