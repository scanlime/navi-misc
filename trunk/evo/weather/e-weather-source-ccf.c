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

#include "e-weather-source-ccf.h"

static GList*
e_weather_source_ccf_parse (EWeatherSource *source, const char *buffer)
{
	WeatherForecast *forecasts;

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
	 */
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
